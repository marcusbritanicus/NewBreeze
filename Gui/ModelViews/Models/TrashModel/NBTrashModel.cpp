/*
	*
	* NBTrashModel.cpp - NewBreeze NBTrashModel Class
	*
*/

#include <NBTrashModel.hpp>

NBTrashModel::NBTrashModel() : QAbstractItemModel() {

	oldRoots.clear();
	curIndex = 0;

	rootNode = new NBTrashNode();

	setupModelData();
};

NBTrashModel::~NBTrashModel() {

};

bool NBTrashModel::isCategorizationEnabled() {

	return mCategorizationEnabled;
};

void NBTrashModel::setCategorizationEnabled( bool enabled ) {

	Q_UNUSED( enabled );

	mCategorizationEnabled = true;
	// sort();
};

int NBTrashModel::rowCount( const QModelIndex & parent ) const {

	if ( parent.column() > 0 )
		return 0;

	if ( not parent.isValid() )
		return rootNode->childCount();

	else
		return static_cast<NBTrashNode*>( parent.internalPointer() )->childCount();
};

int NBTrashModel::rowCount( QString mCategory ) const {

	if ( not rootNode->categoryList().contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return 0;

	return categoryRowMap[ mCategory ].count();
};

int NBTrashModel::categoryCount() const {

	return rootNode->categoryCount();
};

int NBTrashModel::columnCount( const QModelIndex & parent ) const {

	Q_UNUSED( parent );
	return 4;
};

Qt::ItemFlags NBTrashModel::flags( const QModelIndex & index ) const {

	if ( not index.isValid() )
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
};

QVariant NBTrashModel::data( const QModelIndex &index, int role ) const {

	if ( not index.isValid() )
		return QVariant();

	NBTrashNode *node = static_cast<NBTrashNode*>( index.internalPointer() );
	switch( role ) {

		case Qt::DisplayRole: {
			return node->name();
		}

		case Qt::DecorationRole: {
			return node->icon();
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
			return node->data( index.column() );
		}

		case Qt::ToolTipRole: {
			return QString(
						"<center>%1<br>%2<br>%3</center>"
					).arg( node->name() ).arg( node->size() ).arg( node->deletionDate().toString( "MMM dd, yyyy hh:mm:ss" ) );
		}

		/* Original Location */
		case Qt::UserRole + 1: {
			return node->originalPath();
		}

		/* Size */
		case Qt::UserRole + 2: {
			return node->size();
		}

		/* Deletion Date */
		case Qt::UserRole + 3: {
			return node->deletionDate().toString( "MMM dd, yyyy hh:mm:ss" );
		}

		/* Trash Path */
		case Qt::UserRole + 4: {
			return node->trashPath();
		}

		/* Trash Info Path */
		case Qt::UserRole + 5: {
			return node->trashInfoPath();
		}

		default: {
			return QVariant();
		}
	}
};

QVariant NBTrashModel::headerData( int section, Qt::Orientation orientation, int role ) const {

	QStringList headerList = QStringList() << "FileName" << "Original Path" << "Size" << "Deleteion Date";

	if ( ( orientation == Qt::Horizontal ) and ( role == Qt::DisplayRole ) )
		return headerList.at( section );

	else if ( ( orientation == Qt::Horizontal ) and ( role == Qt::TextAlignmentRole ) )
		return ( 0x0001 | 0x0080 );

	else
		return QVariant();
};

bool NBTrashModel::setData( const QModelIndex &index, QString value, int role ) {

	bool ok = rootNode->child( index.row() )->setData( role, value );
	if ( ok )
		emit dataChanged( index, index );

	return ok;
};

bool NBTrashModel::insertNode( QString nodeName ) {

	Q_UNUSED( nodeName );

	return true;
};

void NBTrashModel::updateNode( QString nodeName ) {

	Q_UNUSED( nodeName );

	return;
};

bool NBTrashModel::removeNode( QString nodeName ) {

	Q_UNUSED( nodeName );

	return false;
};

QModelIndex NBTrashModel::index( int row, int column, const QModelIndex &parent ) const {

	if ( not hasIndex( row, column, parent ) )
		return QModelIndex();

	NBTrashNode *parentNode;
	if ( not parent.isValid() )
		parentNode = rootNode;

	else
		parentNode = (NBTrashNode *)parent.internalPointer();

	NBTrashNode *childNode = parentNode->child( row );
	if ( childNode )
		return createIndex( row, column, childNode );

	else
		return QModelIndex();
};

QModelIndex NBTrashModel::index( QString name, const QModelIndex &parent ) const {

	if ( not __childNames.contains( name ) )
		return QModelIndex();

	NBTrashNode *parentNode;
	if ( not parent.isValid() )
		parentNode = rootNode;

	else
		parentNode = (NBTrashNode *)parent.internalPointer();

	int row = parentNode->child( name )->row();
	NBTrashNode *childNode = parentNode->child( name );

	if ( childNode )
		return createIndex( row, 0, childNode );

	return QModelIndex();
};

QModelIndex NBTrashModel::parent( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QModelIndex();

	NBTrashNode *childNode = ( NBTrashNode * )index.internalPointer();
	NBTrashNode *parentNode = childNode->parent();

	if ( parentNode == rootNode )
		return QModelIndex();

	return createIndex( parentNode->row(), 0, parentNode );
};

QString NBTrashModel::category( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QString();

	NBTrashNode *childItem = ( NBTrashNode * )index.internalPointer();
	return childItem->category();
};

int NBTrashModel::categoryIndex( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return -1;

	NBTrashNode *childItem = ( NBTrashNode * )index.internalPointer();
	return childItem->categoryIndex();
};

QStringList NBTrashModel::categories() const {

	return rootNode->categoryList();
};

void NBTrashModel::hideCategory( QString mCategory ) {

	if ( rootNode->categoryList().contains( mCategory ) )
		categoryVisibilityMap[ mCategory ] = false;
};

void NBTrashModel::showCategory( QString mCategory ) {

	if ( rootNode->categoryList().contains( mCategory ) )
		categoryVisibilityMap[ mCategory ] = true;
};

bool NBTrashModel::isCategoryVisible( QString mCategory ) const {

	if ( rootNode->categoryList().contains( mCategory ) )
		return categoryVisibilityMap[ mCategory ];

	return false;
};

int NBTrashModel::indexListCountForCategory( QString mCategory ) const {

	if ( not rootNode->categoryList().contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return 0;

	return categoryRowMap[ mCategory ].count();
};

QModelIndexList NBTrashModel::indexListForCategory( QString mCategory ) const {

	QModelIndexList mList;

	if ( not rootNode->categoryList().contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return mList;

	foreach( int row, categoryRowMap[ mCategory ] ) {
		mList << index( row, 0, QModelIndex() );
	}

	return mList;
};

Qt::DropActions NBTrashModel::supportedDragActions() const {

	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
};

Qt::DropActions NBTrashModel::supportedDropActions() const {

	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
};

Qt::ItemFlags NBTrashModel::flags( const QModelIndex index ) const {

	Q_UNUSED( index );

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
};

QStringList NBTrashModel::mimeTypes() const {

	QStringList types;
	types << "text/uri-list";
	return types;
};

bool NBTrashModel::filter( Filters filter ) {

	Q_UNUSED( filter );

	return false;
};

void NBTrashModel::setFilter( Filters filter, bool on ) {

	Q_UNUSED( filter );
	Q_UNUSED( on );

	return;
};

QStringList NBTrashModel::nameFilters() {

	return __nameFilters;
};

void NBTrashModel::setNameFilters( QStringList filters ) {

	__nameFilters.clear();
	__nameFilters << filters;

	// setupModelData();
};

void NBTrashModel::clearNameFilters() {

	__nameFilters.clear();
	setupModelData();
};

void NBTrashModel::reload() {

	setupModelData();
};

void NBTrashModel::restore( QModelIndexList toBeRestored ) {

	QModelIndexList failed;
	QSettings trashInfo( "NewBreeze", "TrashInfo" );

	Q_FOREACH( QModelIndex idx, toBeRestored ) {
		NBTrashNode *node = static_cast<NBTrashNode*>( idx.internalPointer() );
		__childNames.removeOne( node->name() );

		if ( exists( node->originalPath() ) )
			failed << idx;

		if( rename( qPrintable( node->trashPath() ), qPrintable( node->originalPath() ) ) )
			failed << idx;

		remove( qPrintable( node->trashInfoPath() ) );
		trashInfo.remove( QUrl::toPercentEncoding( node->name() ) );
		trashInfo.sync();

		rootNode->removeChild( node );
	};

	setupModelData();
};

void NBTrashModel::removeFromDisk( QModelIndexList toBeDeleted ) {

	QModelIndexList failed;
	QSettings trashInfo( "NewBreeze", "TrashInfo" );

	Q_FOREACH( QModelIndex idx, toBeDeleted ) {
		NBTrashNode *node = static_cast<NBTrashNode*>( idx.internalPointer() );
		__childNames.removeOne( node->name() );

		if ( exists( node->originalPath() ) )
			failed << idx;

		if( remove( qPrintable( node->trashPath() ) ) )
			failed << idx;

		remove( qPrintable( node->trashInfoPath() ) );
		trashInfo.remove( QUrl::toPercentEncoding( node->name() ) );
		trashInfo.sync();

		rootNode->removeChild( node );
	};

	setupModelData();
};

void NBTrashModel::setupModelData() {

	__childNames.clear();
	rootNode->clearChildren();
	curentLoadStatus.loading = true;

	emit trashLoading();

	QSettings trashInfo( "NewBreeze", "TrashInfo" );

	beginResetModel();
	/* Loading home trash */
	QString trashLoc = NBXdg::trashLocation( NBXdg::home() );
	if ( not trashLoc.isEmpty() ) {
		QDir trash( trashLoc + "/info/" );
		trash.setNameFilters( QStringList() << "*.trashinfo" );
		Q_FOREACH( QString entry, trash.entryList() ) {

			if ( curentLoadStatus.stopLoading ) {
				endResetModel();
				curentLoadStatus.stopLoading = false;
				return;
			}

			QSettings trashInfo( trash.absoluteFilePath( entry ), QSettings::NativeFormat );
			QString origPath = trashInfo.value( "Trash Info/Path" ).toString();
			QString delDate = trashInfo.value( "Trash Info/DeletionDate" ).toString();
			QString trashPath = QString( trashInfo.fileName() ).replace( "/info/", "/files/" ).replace( ".trashinfo", "" );

			QStringList data = QStringList() << baseName( origPath ) << origPath << delDate << trashPath;

			rootNode->addChild( new NBTrashNode( data, getCategory( data.at( 2 ) ), rootNode ) );
			__childNames << baseName( origPath );
		}
	}

	/* Loading all other trash */
	NBDeviceManager devMgr;
	Q_FOREACH( NBDeviceInfo devInfo, devMgr.allDevices() ) {
		/* We don't want to check the home directory for trash once more */
		if ( devInfo.mountPoint() == NBXdg::home() )
			continue;

		trashLoc = NBXdg::trashLocation( devInfo.mountPoint() );
		if ( not trashLoc.isEmpty() ) {
			QDir trash( trashLoc + "/info/" );
			trash.setNameFilters( QStringList() << "*.trashinfo" );
			Q_FOREACH( QString entry, trash.entryList() ) {

				if ( curentLoadStatus.stopLoading ) {
					endResetModel();
					curentLoadStatus.stopLoading = false;
					return;
				}

				QSettings trashInfo( trash.absoluteFilePath( entry ), QSettings::NativeFormat );
				QString origPath = trashInfo.value( "Trash Info/Path" ).toString();
				QString delDate = trashInfo.value( "Trash Info/DeletionDate" ).toString();
				QString trashPath = QString( trashInfo.fileName() ).replace( "/info/", "/files/" ).replace( ".trashinfo", "" );

				QStringList data = QStringList() << baseName( origPath ) << origPath << delDate << trashPath;

				rootNode->addChild( new NBTrashNode( data, getCategory( data.at( 2 ) ), rootNode ) );
				__childNames << baseName( origPath );
			}
		}
	}
	endResetModel();

	sort();

	curentLoadStatus.loading = false;

	emit trashLoaded();
};

QString NBTrashModel::getCategory( QString dateStr ) {

	QDateTime date = QDateTime::fromString( dateStr, "yyyyMMddThh:mm:ss" );
	if ( date.isValid() ) {
		if ( date.date() == QDate::currentDate() )
			return "Today";

		else if ( date.date().weekNumber() == QDate::currentDate().weekNumber() )
			return "This Week";

		else if (  date.date().weekNumber() == QDate::currentDate().weekNumber() - 1 )
			return "Last Week";

		else if (  date.date().month() == QDate::currentDate().month() )
			return "This Month";

		else if (  date.date().month()== QDate::currentDate().month() - 1 )
			return "Last Month";

		else
			return date.toString( "MMMM yyyy" );
	}

	return QString( "Uncategorized" );
};

void NBTrashModel::recategorize() {

	foreach( NBTrashNode *node, rootNode->children() )
		node->setCategory( getCategory( node->allData().at( 2 ) ) );

	rootNode->updateCategories();
};

void NBTrashModel::handleNodeCreated( QString node ) {

	insertNode( baseName( node ) );
};

void NBTrashModel::handleNodeChanged( QString node ) {

	updateNode( baseName( node ) );
};

void NBTrashModel::handleNodeDeleted( QString node ) {

	removeNode( baseName( node ) );
};

void NBTrashModel::sort() {

	recategorize();
	rootNode->sort();

	// Create a map of categoryIdndex versus rows
	categoryRowMap.clear();
	foreach( NBTrashNode *item, rootNode->children() )
		categoryRowMap[ item->category() ] << item->row();

	foreach( QString mCategory, rootNode->categoryList() )
		categoryVisibilityMap[ mCategory ] = true;

	emit trashLoaded();
};
