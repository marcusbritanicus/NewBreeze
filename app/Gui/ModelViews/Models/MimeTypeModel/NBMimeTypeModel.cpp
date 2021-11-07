/*
	*
	* NBMimeTypeModel.cpp - NewBreeze NBMimeTypeModel Class
	*
*/

#include "NBMimeTypeModel.hpp"

static QMutex mutex;

NBMimeIconUpdater::NBMimeIconUpdater( QString root, QStringList entries, bool *term ) : QThread() {

	mTerminate = term;

	entryList.clear();
	entryList << entries;

	rootPath.clear();
	rootPath = root;
	if ( not root.endsWith( "/" ) )
		rootPath += "/";
};

NBMimeIconUpdater::~NBMimeIconUpdater() {

	terminate();
	wait();
};

void NBMimeIconUpdater::run() {

	if ( rootPath != "/dev/" ) {
		foreach( QString entry, entryList ) {
			if ( *mTerminate )
				break;

			QString mName = mimeDb.mimeTypeForFile( rootPath + entry ).name();
			emit updated( rootPath, entry, NBIconManager::instance()->icon( mName ) );
		}
	}
};

NBMimeTypeModel::NBMimeTypeModel() : QAbstractItemModel() {

	/* Switch for temination of data gathering */
	mTerminate = false;

	/* Useless swtiches */
	currentLoadStatus.loading = false;
	currentLoadStatus.stopLoading = false;

	/* Root node */
	rootNode = new NBMimeTypeNode();
};

NBMimeTypeModel::~NBMimeTypeModel() {

};

bool NBMimeTypeModel::isCategorizationEnabled() {

	return true;
};

int NBMimeTypeModel::rowCount( const QModelIndex & parent ) const {

	if ( parent.column() > 0 )
		return 0;

	if ( not parent.isValid() )
		return rootNode->childCount();

	else
		return static_cast<NBMimeTypeNode*>( parent.internalPointer() )->childCount();
};

int NBMimeTypeModel::rowCount( QString mCategory ) const {

	if ( not mCategoryList.contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return 0;

	return categoryRowMap[ mCategory ].count();
};

int NBMimeTypeModel::categoryCount() const {

	return mCategoryList.count();
};

int NBMimeTypeModel::columnCount( const QModelIndex & parent ) const {

	Q_UNUSED( parent );
	return 1;
};

Qt::ItemFlags NBMimeTypeModel::flags( const QModelIndex & index ) const {

	if ( not index.isValid() )
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
};

QVariant NBMimeTypeModel::data( const QModelIndex &index, int role ) const {

	if ( not index.isValid() or index.column() )
		return QVariant();

	NBMimeTypeNode *node = static_cast<NBMimeTypeNode*>( index.internalPointer() );
	switch( role ) {

		case Qt::DisplayRole:
			return node->name();

		case Qt::DecorationRole:
			return node->icon();

		case Qt::TextAlignmentRole:
			return ( 0x0001 | 0x0080 );

		case Qt::InitialSortOrderRole:
			return Qt::AscendingOrder;

		case Qt::AccessibleTextRole:
			return node->name();

		case Qt::ToolTipRole:
			return QString( "%1/%2" ).arg( node->category() ).arg( node->name() );

		default:
			return QVariant();
	}
};

QModelIndex NBMimeTypeModel::index( int row, int column, const QModelIndex &parent ) const {

    if ( row < 0 || column < 0 )
        return QModelIndex();

    if ( not ( ( row < rowCount( parent ) ) and ( column < columnCount( parent ) ) ) )
		return QModelIndex();

	NBMimeTypeNode *parentNode;
	if ( not parent.isValid() )
		parentNode = rootNode;

	else
		parentNode = (NBMimeTypeNode *)parent.internalPointer();

	NBMimeTypeNode *childNode = parentNode->child( row );
	if ( childNode )
		return createIndex( row, column, childNode );

	else
		return QModelIndex();
};

QModelIndex NBMimeTypeModel::index( QString name, const QModelIndex &parent ) const {

	if ( not mChildNames.contains( name ) )
		return QModelIndex();

	NBMimeTypeNode *parentNode;
	if ( not parent.isValid() )
		parentNode = rootNode;

	else
		parentNode = (NBMimeTypeNode *)parent.internalPointer();

	int row = parentNode->child( name )->row();
	NBMimeTypeNode *childNode = parentNode->child( name );

	if ( childNode )
		return createIndex( row, 0, childNode );

	return QModelIndex();
};

QModelIndex NBMimeTypeModel::parent( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QModelIndex();

	NBMimeTypeNode *childNode = ( NBMimeTypeNode * )index.internalPointer();
	NBMimeTypeNode *parentNode = childNode->parent();

	if ( parentNode == rootNode )
		return QModelIndex();

	return createIndex( parentNode->row(), 0, parentNode );
};

QString NBMimeTypeModel::category( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QString( "" );

	NBMimeTypeNode *childItem = ( NBMimeTypeNode * )index.internalPointer();
	return childItem->category();
};

int NBMimeTypeModel::categoryIndex( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return -1;

	NBMimeTypeNode *childItem = ( NBMimeTypeNode * )index.internalPointer();
	return mCategoryList.indexOf( childItem->category() );
};

QStringList NBMimeTypeModel::categories() const {

	return mCategoryList;
};

QPixmap NBMimeTypeModel::pixmapForCategory( QString categoryName ) const {

	return QIcon::fromTheme( categoryName + "-x-generic", QIcon::fromTheme( "application-octet-stream" ) ).pixmap( 24, 24 );
};

void NBMimeTypeModel::foldCategory( QString categoryName ) {

	categoryVisibilityMap[ categoryName ] = false;
};

void NBMimeTypeModel::openCategory( QString categoryName ) {

	categoryVisibilityMap[ categoryName ] = true;
};

bool NBMimeTypeModel::isCategoryOpen( QString categoryName ) const {

	return categoryVisibilityMap[ categoryName ];
};

int NBMimeTypeModel::indexListCountForCategory( QString mCategory ) const {

	if ( not mCategoryList.contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return 0;

	return categoryRowMap[ mCategory ].count();
};

QModelIndexList NBMimeTypeModel::indexListForCategory( QString mCategory ) const {

	QModelIndexList mList;

	if ( not mCategoryList.contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return mList;

	Q_FOREACH( int row, categoryRowMap[ mCategory ] )
		mList << index( row, 0, QModelIndex() );

	return mList;
};

QModelIndexList NBMimeTypeModel::categorySiblings( QModelIndex idx ) const {

	if ( not idx.isValid() )
		return QModelIndexList();

	QModelIndexList mList = indexListForCategory( category( idx ) );
	mList.removeAll( idx );

	return mList;
};

QStringList NBMimeTypeModel::nameFilters() {

	return __nameFilters;
};

void NBMimeTypeModel::setNameFilters( QStringList filters ) {

	__nameFilters.clear();
	__nameFilters << filters;

	setupModelData();
};

void NBMimeTypeModel::clearNameFilters() {

	__nameFilters.clear();
	setupModelData();
};

void NBMimeTypeModel::sort() {

	rootNode->sort();

	// Create a map of categoryIndex versus rows
	categoryRowMap.clear();
	foreach( NBMimeTypeNode *item, rootNode->children() )
		categoryRowMap[ item->category() ] << item->row();

	emit layoutChanged();
};

void NBMimeTypeModel::reload() {

	setupModelData();
};

void NBMimeTypeModel::setupModelData() {

	mChildNames.clear();
	rootNode->clearChildren();
	currentLoadStatus.loading = true;

	beginResetModel();
	Q_FOREACH( QMimeType mType, mimeDb.allMimeTypes() ) {
		QStringList cnList( mType.name().split( "/" ) );

		if ( not mCategoryList.contains( cnList.at( 0 ) ) )
			mCategoryList << cnList.at( 0 );

		mChildNames << cnList.at( 1 );
		rootNode->addChild( new NBMimeTypeNode( mType, rootNode ) );
	}
	endResetModel();

	currentLoadStatus.loading = false;

	emit modelLoaded();
};

void NBMimeTypeModel::terminateInfoGathering() {

	mTerminate = true;
};

void NBMimeTypeModel::updateAllNodes() {

};

void NBMimeTypeModel::saveInfo( QString, QString, QStringList ) {

};
