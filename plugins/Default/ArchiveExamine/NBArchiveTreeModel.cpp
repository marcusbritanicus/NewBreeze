/*
	*
	* NBArchiveTreeModel.cpp - NewBreeze NBArchiveTreeModel Class
	*
*/

#include "NBArchiveTreeModel.hpp"

#include <archive.h>
#include <archive_entry.h>

NBArchiveTreeModel::NBArchiveTreeModel( QString path ) : QAbstractItemModel() {

	/* Archive name and path */
	archiveName = path;

	/* By default we don't show hidden files */
	__showHidden = false;

	/* Switch for temination of data gathering */
	__terminate = false;

	/* Useless swtiches */
	currentLoadStatus.loading = false;
	currentLoadStatus.stopLoading = false;

	/* Root node */
	growTree();
};

NBArchiveTreeModel::~NBArchiveTreeModel() {

};

int NBArchiveTreeModel::rowCount( const QModelIndex &parent ) const {

	NBTreeBranch *parentItem;
	if ( parent.column() > 0 )
		return 0;

	if ( not parent.isValid() )
		parentItem = tree;

	else
		parentItem = static_cast<NBTreeBranch*>( parent.internalPointer() );

	return parentItem->branchCount();
};

int NBArchiveTreeModel::columnCount( const QModelIndex & ) const {

	return 1;
};

Qt::ItemFlags NBArchiveTreeModel::flags( const QModelIndex &idx ) const {

	if ( not idx.isValid() )
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled;
};

QVariant NBArchiveTreeModel::data( const QModelIndex &index, int role ) const {

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

QVariant NBArchiveTreeModel::headerData( int section, Qt::Orientation orientation, int role ) const {

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

QModelIndex NBArchiveTreeModel::index( int row, int column, const QModelIndex &parent ) const {

    if ( row < 0 or column < 0 )
        return QModelIndex();

    if ( ( row > rowCount( parent ) ) and ( column > columnCount( parent ) ) )
		return QModelIndex();

	NBTreeBranch *parentNode;
	if ( not parent.isValid() )
		parentNode = tree;

	else
		parentNode = (NBTreeBranch *)parent.internalPointer();

	NBTreeBranch *branch = parentNode->branch( row );
	if ( branch )
		return createIndex( row, column, branch );

	else
		return QModelIndex();
};

QModelIndex NBArchiveTreeModel::index( QString name, const QModelIndex &parent ) const {

	NBTreeBranch *parentNode;
	if ( not parent.isValid() )
		parentNode = tree;

	else
		parentNode = static_cast<NBTreeBranch*>( parent.internalPointer() );

	NBTreeBranch *branch = parentNode->branch( name );
	if ( branch )
		return createIndex( branch->row(), 0, branch );

	return QModelIndex();
};

QModelIndex NBArchiveTreeModel::parent( const QModelIndex &idx ) const {

	if ( not idx.isValid() )
		return QModelIndex();

	NBTreeBranch *branch = ( NBTreeBranch * )idx.internalPointer();
	NBTreeBranch *parentNode = branch->parent();

	if ( parentNode == tree )
		return QModelIndex();

	return createIndex( parentNode->row(), 0, parentNode );
};

bool NBArchiveTreeModel::hasBranches( const QModelIndex &idx ) const {

	if ( not idx.isValid() )
		return true;

	else {
		NBTreeBranch *branch = static_cast<NBTreeBranch*>( idx.internalPointer() );
		return ( branch->branchCount() > 0 );
	}
};

bool NBArchiveTreeModel::showHidden() const {

	return __showHidden;
};

void NBArchiveTreeModel::setShowHidden( bool ) {

	// plantTree();
};

Qt::DropActions NBArchiveTreeModel::supportedDragActions() const {

	return Qt::IgnoreAction;
};

Qt::DropActions NBArchiveTreeModel::supportedDropActions() const {

	return Qt::IgnoreAction;
};

QStringList NBArchiveTreeModel::mimeTypes() const {

	QStringList types;
	types << "text/uri-list";
	return types;
};

QStringList NBArchiveTreeModel::nameFilters() {

	return __nameFilters;
};

void NBArchiveTreeModel::setNameFilters( QStringList filters ) {

	__nameFilters.clear();
	__nameFilters << filters;
};

void NBArchiveTreeModel::clearNameFilters() {

	__nameFilters.clear();
};

void NBArchiveTreeModel::reload() {

	// plantTree();
};

QString NBArchiveTreeModel::nodeName( const QModelIndex idx ) const {

	return idx.data( 0 ).toString();
};

QString NBArchiveTreeModel::nodePath( const QModelIndex idx ) const {

	if ( not idx.isValid() )
		return QString();

	return idx.data( Qt::UserRole + 1 ).toString();
};

QFileInfo NBArchiveTreeModel::nodeInfo( const QModelIndex idx ) const {

	if ( not idx.isValid() )
		return QFileInfo();

	return QFileInfo( idx.data( Qt::UserRole + 1 ).toString() );
};

void NBArchiveTreeModel::growTree() {

	beginResetModel();

	struct archive *a;
	struct archive_entry *entry;
	int r;

	// Source Archive
	a = archive_read_new();
	archive_read_support_format_all( a );
	archive_read_support_filter_all( a );

	tree = new NBTreeBranch( "Name", QIcon::fromTheme( "folder" ), 0 );

	if ( ( r = archive_read_open_filename( a, archiveName.toLatin1().data(), 10240 ) ) )
		return;

	while ( r = archive_read_next_header( a, &entry ) != ARCHIVE_EOF ) {

		if ( r < ARCHIVE_OK )
			fprintf( stderr, "%s\n", archive_error_string( a ) );

		if ( r < ARCHIVE_WARN )
			return;

		QString name;
		name = QString::fromUtf8( archive_entry_pathname( entry ) );
		int type = archive_entry_filetype( entry );

		QStringList tokens = QStringList() << name.split( "/", QString::SkipEmptyParts );
		tree->addBranch( new NBTreeBranch( tokens.value( 0 ), QIcon::fromTheme( "folder" ), tree ) );
		if ( tokens.size() == 1 )
			continue;

		NBTreeBranch *branch = tree;
		for( int i = 1; i < tokens.size(); i++ ) {
			branch = branch->branch( tokens.value( i - 1 ) );
			QString iconName = ( ( type == AE_IFDIR ) ? "folder" : mimeDb.mimeTypeForFile( tokens.value( i ) ).iconName() );
			branch->addBranch( new NBTreeBranch( tokens.value( i ), QIcon::fromTheme( iconName ), branch ) );
		}

		archive_read_data_skip( a );
		qApp->processEvents();
	}

	archive_read_close( a );
	archive_read_free( a );

	endResetModel();
};

void NBArchiveTreeModel::terminateInfoGathering() {

	__terminate = true;
};
