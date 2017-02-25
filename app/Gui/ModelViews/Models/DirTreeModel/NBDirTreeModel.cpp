/*
	*
	* NBDirTreeModel.cpp - NewBreeze NBDirTreeModel Class
	*
*/

#include "NBDirTreeModel.hpp"

NBDirTreeModel::NBDirTreeModel() : QAbstractItemModel() {

	/* By default we don't show hidden files */
	__showHidden = false;

	/* Switch for temination of data gathering */
	__terminate = false;

	/* Useless swtiches */
	currentLoadStatus.loading = false;
	currentLoadStatus.stopLoading = false;

	/* Root node */
	plantTree();
};

NBDirTreeModel::~NBDirTreeModel() {

};

int NBDirTreeModel::rowCount( const QModelIndex &parent ) const {

	NBTreeBranch *parentItem;
	if ( parent.column() > 0 )
		return 0;

	if ( not parent.isValid() )
		parentItem = tree;

	else
		parentItem = static_cast<NBTreeBranch*>( parent.internalPointer() );

	if ( parent.isValid() and not exploredIndexes.contains( parent ) ) {

		parentItem->explore();
		exploredIndexes << parent;
	}

	return parentItem->childCount();
};

int NBDirTreeModel::columnCount( const QModelIndex & ) const {

	return 1;
};

Qt::ItemFlags NBDirTreeModel::flags( const QModelIndex &idx ) const {

	if ( not idx.isValid() )
		return Qt::NoItemFlags;

	QString path( idx.data( Qt::UserRole + 1 ).toString() );
	if ( isReadable( path ) and isWritable( path ) )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	else if ( isReadable( path ) )
		return Qt::ItemIsEnabled;

	else
		return Qt::NoItemFlags;
};

QVariant NBDirTreeModel::data( const QModelIndex &index, int role ) const {

	if ( not index.isValid() )
		return QVariant();

	NBTreeBranch *node = static_cast<NBTreeBranch*>( index.internalPointer() );
	switch( role ) {

		case Qt::DisplayRole: {
			if ( index.column() == 0 )
				return node->name();

			return QVariant();
		}

		case Qt::DecorationRole: {
			if ( index.column() == 0 )
				return node->icon();

			return QVariant();
		}

		case Qt::TextAlignmentRole: {
			if ( index.column() == 0 )
				return ( 0x0001 | 0x0080 );

			else if ( index.column() == 1 )
				return ( 0x0002 | 0x0080 );

			else
				return Qt::AlignCenter;
		}

		case Qt::InitialSortOrderRole: {
			return Qt::AscendingOrder;
		}

		case Qt::AccessibleTextRole: {
			return node->name();
		}

		case Qt::ToolTipRole: {
			if ( index.column() == 0 )
				return node->data( Qt::UserRole + 1 );

			return QString();
		}

		case Qt::UserRole + 1: {

			return node->data( Qt::UserRole + 1 );
		}

		default: {

			return QVariant();
		}
	}
};

QVariant NBDirTreeModel::headerData( int section, Qt::Orientation orientation, int role ) const {

	if ( section or orientation != Qt::Horizontal )
		return QVariant();

	switch( role ) {
		case Qt::DisplayRole:
			return "Name";

		case Qt::TextAlignmentRole:
			return Qt::AlignCenter;

		default:
			return QVariant();
	}
};

QModelIndex NBDirTreeModel::index( int row, int column, const QModelIndex &parent ) const {

    if ( row < 0 or column < 0 )
        return QModelIndex();

    if ( ( row > rowCount( parent ) ) and ( column > columnCount( parent ) ) )
		return QModelIndex();

	NBTreeBranch *parentNode;
	if ( not parent.isValid() )
		parentNode = tree;

	else
		parentNode = (NBTreeBranch *)parent.internalPointer();

	if ( parent.isValid() and not exploredIndexes.contains( parent ) ) {

		parentNode->explore();
		exploredIndexes << parent;
	}

	NBTreeBranch *childNode = parentNode->child( row );
	if ( childNode )
		return createIndex( row, column, childNode );

	else
		return QModelIndex();
};

QModelIndex NBDirTreeModel::index( QString name, const QModelIndex &parent ) const {

	NBTreeBranch *parentNode;
	if ( not parent.isValid() )
		parentNode = tree;

	else
		parentNode = static_cast<NBTreeBranch*>( parent.internalPointer() );

	if ( parent.isValid() and not exploredIndexes.contains( parent ) ) {

		parentNode->explore();
		exploredIndexes << parent;
	}

	NBTreeBranch *childNode = parentNode->child( name );
	if ( childNode )
		return createIndex( childNode->row(), 0, childNode );

	return QModelIndex();
};

QModelIndex NBDirTreeModel::parent( const QModelIndex &idx ) const {

	if ( not idx.isValid() )
		return QModelIndex();

	NBTreeBranch *childNode = ( NBTreeBranch * )idx.internalPointer();
	NBTreeBranch *parentNode = childNode->parent();

	if ( parentNode == tree )
		return QModelIndex();

	return createIndex( parentNode->row(), 0, parentNode );
};

bool NBDirTreeModel::hasChildren( const QModelIndex &idx ) const {

	if ( not idx.isValid() )
		return true;

	else {
		if ( exploredIndexes.contains( idx ) ) {
			NBTreeBranch *branch = static_cast<NBTreeBranch*>( idx.internalPointer() );
			return ( branch->childCount() > 0 );
		}

		if ( not isReadable( idx.data( Qt::UserRole + 1 ).toString() ) ) {

			return false;
		}

		return true;
	}
};

bool NBDirTreeModel::showHidden() const {

	return __showHidden;
};

void NBDirTreeModel::setShowHidden( bool shown ) {

	// plantTree();
};

Qt::DropActions NBDirTreeModel::supportedDragActions() const {

	return Qt::IgnoreAction;
};

Qt::DropActions NBDirTreeModel::supportedDropActions() const {

	return Qt::IgnoreAction;
};

Qt::ItemFlags NBDirTreeModel::flags( const QModelIndex idx ) const {

	QString path( idx.data( Qt::UserRole + 1 ).toString() );

	/* We have read and write access */
	if ( isReadable( path ) and isWritable( path ) )
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	/* We have just read access */
	else if ( isReadable( path ) )
		return Qt::ItemIsEnabled;

	/* We have no access */
	else
		return Qt::NoItemFlags;
};

QStringList NBDirTreeModel::mimeTypes() const {

	QStringList types;
	types << "text/uri-list";
	return types;
};

QStringList NBDirTreeModel::nameFilters() {

	return __nameFilters;
};

void NBDirTreeModel::setNameFilters( QStringList filters ) {

	__nameFilters.clear();
	__nameFilters << filters;
};

void NBDirTreeModel::clearNameFilters() {

	__nameFilters.clear();
};

void NBDirTreeModel::reload() {

	// plantTree();
};

QString NBDirTreeModel::nodeName( const QModelIndex idx ) const {

	return idx.data( 0 ).toString();
};

QString NBDirTreeModel::nodePath( const QModelIndex idx ) const {

	if ( not idx.isValid() )
		return QString();

	return idx.data( Qt::UserRole + 1 ).toString();
};

QFileInfo NBDirTreeModel::nodeInfo( const QModelIndex idx ) const {

	if ( not idx.isValid() )
		return QFileInfo();

	return QFileInfo( idx.data( Qt::UserRole + 1 ).toString() );
};

void NBDirTreeModel::plantTree() {

	beginResetModel();

	tree = new NBTreeBranch( "Name", QIcon::fromTheme( "folder" ), 0 );
	tree->addChild( new NBTreeBranch( "/", QIcon::fromTheme( "folder" ), tree ) );

	endResetModel();
};

void NBDirTreeModel::terminateInfoGathering() {

	__terminate = true;
};
