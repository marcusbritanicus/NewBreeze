/*
	*
	* NBFileSystemModel.cpp - NewBreeze NBFileSystemModel Class
	*
*/

#include <NBFileSystemModel.hpp>

inline bool matchesFilter( QStringList filters, QString text ) {

	foreach( QString filter, filters )
		if ( text.contains( QRegExp( filter, Qt::CaseInsensitive, QRegExp::Wildcard ) ) )
			return true;

	return false;
};

NBFileSystemModel::NBFileSystemModel() : QAbstractItemModel() {

	__showHidden = false;
	__readOnly = true;

	oldRoots.clear();
	curIndex = 0;

	rootNode = new NBFileSystemNode();

	quickDataGatherer = new NBQuickFileInfoGatherer();

	watcher = new NBFileSystemWatcher();

	connect( watcher, SIGNAL( nodeCreated( QString ) ), this, SLOT( handleNodeCreated( QString ) ) );
	connect( watcher, SIGNAL( nodeChanged( QString ) ), this, SLOT( handleNodeChanged( QString ) ) );
	connect( watcher, SIGNAL( nodeDeleted( QString ) ), this, SLOT( handleNodeDeleted( QString ) ) );
	connect( watcher, SIGNAL( watchPathDeleted() ), this, SLOT( loadHome() ) );

	connect( this, SIGNAL( loadFileInfo() ), this, SLOT( gatherFileInfo() ) );
	connect( this, SIGNAL( updatedAllNodes() ), this, SLOT( sort() ) );
};

NBFileSystemModel::~NBFileSystemModel() {

};

bool NBFileSystemModel::isCategorizationEnabled() {

	return mCategorizationEnabled;
};

void NBFileSystemModel::setCategorizationEnabled( bool enabled ) {

	mCategorizationEnabled = enabled;
	sort( prevSort.column, prevSort.cs, enabled );
};

int NBFileSystemModel::rowCount( const QModelIndex & parent ) const {

	if ( parent.column() > 0 )
		return 0;

	if ( not parent.isValid() )
		return rootNode->childCount();

	else
		return static_cast<NBFileSystemNode*>( parent.internalPointer() )->childCount();
};

int NBFileSystemModel::rowCount( QString mCategory ) const {

	if ( not rootNode->categoryList().contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return 0;

	return categoryRowMap[ mCategory ].count();
};

int NBFileSystemModel::categoryCount() const {

	return rootNode->categoryCount();
};

int NBFileSystemModel::columnCount( const QModelIndex & parent ) const {

	Q_UNUSED( parent );
	return 7;
};

Qt::ItemFlags NBFileSystemModel::flags( const QModelIndex & index ) const {

	if ( not index.isValid() )
		return Qt::NoItemFlags;

	if ( __readOnly )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
};

QVariant NBFileSystemModel::data( const QModelIndex &index, int role ) const {

	if ( not index.isValid() )
		return QVariant();

	NBFileSystemNode *node = static_cast<NBFileSystemNode*>( index.internalPointer() );
	switch( role ) {

		case Qt::DisplayRole: {
			if (  ( index.column() >= 0 ) and ( index.column() <= 6  ) )
				return QVariant( node->data( index.column() ) );

			else
				return QVariant();
		}

		case Qt::DecorationRole: {
			if ( index.column() == 0 ) {
				// Unknown theme+inbuilt icon
				QIcon unknown = QIcon::fromTheme( "unknown", QIcon( ":/icons/unknown.png" ) );

				// Icon String
				QString icoStr( node->data( 2, true ).toString() );

				// The inbuilt icon or from the file system
				QIcon savedIcon = QIcon( icoStr );

				// Icon we got from the theme
				QIcon themeIcon = QIcon::fromTheme( icoStr );

				if ( not themeIcon.isNull() )
					return themeIcon;

				else if ( not savedIcon.isNull() )
					return savedIcon;

				else
					return unknown;
			}

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
			return node->data( index.column() );
		}

		case Qt::ToolTipRole: {
			switch( index.column() ) {
				case 0:	return QString( "<center>%1<br>%2</center>" ).arg( node->data( 0 ).toString() ).arg( node->data( 1 ).toString() );
				default: return QString( "<b>%1</b>" ).arg( node->data( index.column() ).toString() );
			}
		}

		case Qt::UserRole + 1: {
			return node->data( 1 );
		}

		case Qt::UserRole + 2: {
			return node->data( 2 );
		}

		case Qt::UserRole + 3: {
			return node->data( 3 );
		}

		case Qt::UserRole + 4: {
			return node->data( 4 );
		}

		case Qt::UserRole + 5: {
			return node->data( 5 );
		}

		case Qt::UserRole + 6: {
			return node->data( 6 );
		}

		default: {
			return QVariant();
		}
	}
};

QVariant NBFileSystemModel::headerData( int section, Qt::Orientation orientation, int role ) const {

	QVariantList headerList = QVariantList() << "Name" << "Size" << "Type" << "MimeType" << "Modified" << "Permissions" << "Owner";

	if ( ( orientation == Qt::Horizontal ) and ( role == Qt::DisplayRole ) )
		return headerList.at( section );

	else if ( ( orientation == Qt::Horizontal ) and ( role == Qt::TextAlignmentRole ) )
		if ( section == 0 )
			return ( 0x0001 | 0x0080 );

		else if ( section == 1 )
			return ( 0x0002 | 0x0080 );

		else
			return Qt::AlignCenter;

	else
		return QVariant();
};

bool NBFileSystemModel::setData( const QModelIndex &index, QVariant value, int role ) {

	bool ok = rootNode->child( index.row() )->setData( role, value );
	if ( ok )
		emit dataChanged( index, index );

	return ok;
};

bool NBFileSystemModel::insertNode( QString nodeName ) {

	/*
		*
		* Quick Fix: Append the newly added file at the end of the model.
		* Then sort the model.
		*
	*/

	if ( not exists( __rootPath + nodeName ) )
		return false;

	beginResetModel();
	beginInsertRows( parent(), __childNames.count(), __childNames.count() );

	QVariantList data = quickDataGatherer->getQuickFileInfo( __rootPath + nodeName );

	if ( __showHidden ) {
		if ( __nameFilters.count() ) {
			if ( matchesFilter( __nameFilters, nodeName ) ) {
				rootNode->addChild( new NBFileSystemNode( data, getCategory( data ), rootNode ) );
				__childNames << nodeName;
			}
		}
		else {
			rootNode->addChild( new NBFileSystemNode( data, getCategory( data ), rootNode ) );
			__childNames << nodeName;
		}
	}
	else {
		if ( not nodeName.startsWith( "." ) ) {
			if ( __nameFilters.count() ) {
				if ( matchesFilter( __nameFilters, nodeName ) ) {
					rootNode->addChild( new NBFileSystemNode( data, getCategory( data ), rootNode ) );
					__childNames << nodeName;
				}
			}
			else {
				rootNode->addChild( new NBFileSystemNode( data, getCategory( data ), rootNode ) );
				__childNames << nodeName;
			}
		}
	}
	endInsertRows();

	sort( prevSort.column, prevSort.cs, prevSort.categorized );
	endResetModel();

	NBFileInfoGatherer *ig = new NBFileInfoGatherer();
	connect(
		ig, SIGNAL( done( QString, QString, QStringList ) ),
		this, SLOT( saveInfo( QString, QString, QStringList ) )
	);

	ig->gatherInfo( QStringList() << nodeName, __rootPath );

	return true;
};

void NBFileSystemModel::updateNode( QString nodeName ) {

	if ( not exists( __rootPath + nodeName ) )
		return;

	if ( not __childNames.contains( nodeName ) )
		return;

	NBFileSystemNode *node = rootNode->child( nodeName );
	if ( isDir( __rootPath + nodeName ) ) {
		node->setData( 1, nChildren( __rootPath + nodeName ), true );
		node->setData( 1, QString( "%1 items" ).arg( node->data( 1, true ).toLongLong() ), false );
	}

	else {
		node->setData( 1, getSize( __rootPath + nodeName ), true );
		node->setData( 1, formatSize( node->data( 1, true ).toLongLong() ), false );
	}

	NBFileInfoGatherer *ig = new NBFileInfoGatherer();
	connect(
		ig, SIGNAL( done( QString, QString, QStringList ) ),
		this, SLOT( saveInfo( QString, QString, QStringList ) )
	);

	ig->gatherInfo( QStringList() << nodeName, __rootPath );
	sort( prevSort.column, prevSort.cs, prevSort.categorized );

	return;
};

bool NBFileSystemModel::removeNode( QString nodeName ) {

	if ( not __childNames.contains( nodeName ) )
		return false;

	__childNames.removeAll( nodeName );
	NBFileSystemNode *removedNode = rootNode->child( nodeName );

	beginResetModel();
	beginRemoveRows( parent(), removedNode->row(), removedNode->row() );

	rootNode->removeChild( removedNode );

	endRemoveRows();
	endResetModel();

	if ( mCategorizationEnabled )
		recategorize();

	sort( prevSort.column, prevSort.cs, prevSort.categorized );

	return false;
};

QModelIndex NBFileSystemModel::index( int row, int column, const QModelIndex &parent ) const {

	if ( not hasIndex( row, column, parent ) )
		return QModelIndex();

	NBFileSystemNode *parentNode;
	if ( not parent.isValid() )
		parentNode = rootNode;

	else
		parentNode = (NBFileSystemNode *)parent.internalPointer();

	NBFileSystemNode *childNode = parentNode->child( row );
	if ( childNode )
		return createIndex( row, column, childNode );

	else
		return QModelIndex();
};

QModelIndex NBFileSystemModel::index( QString name, const QModelIndex &parent ) const {

	if ( not __childNames.contains( name ) )
		return QModelIndex();

	NBFileSystemNode *parentNode;
	if ( not parent.isValid() )
		parentNode = rootNode;

	else
		parentNode = (NBFileSystemNode *)parent.internalPointer();

	int row = parentNode->child( name )->row();
	NBFileSystemNode *childNode = parentNode->child( name );

	if ( childNode )
		return createIndex( row, 0, childNode );

	return QModelIndex();
};

QModelIndex NBFileSystemModel::parent( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QModelIndex();

	NBFileSystemNode *childNode = ( NBFileSystemNode * )index.internalPointer();
	NBFileSystemNode *parentNode = childNode->parent();

	if ( parentNode == rootNode )
		return QModelIndex();

	return createIndex( parentNode->row(), 0, parentNode );
};

QString NBFileSystemModel::category( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QString();

	NBFileSystemNode *childItem = ( NBFileSystemNode * )index.internalPointer();
	return childItem->category();
};

int NBFileSystemModel::categoryIndex( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return -1;

	NBFileSystemNode *childItem = ( NBFileSystemNode * )index.internalPointer();
	return childItem->categoryIndex();
};

QStringList NBFileSystemModel::categories() const {

	return rootNode->categoryList();
};

void NBFileSystemModel::hideCategory( QString mCategory ) {

	if ( rootNode->categoryList().contains( mCategory ) )
		categoryVisibilityMap[ mCategory ] = false;
};

void NBFileSystemModel::showCategory( QString mCategory ) {

	if ( rootNode->categoryList().contains( mCategory ) )
		categoryVisibilityMap[ mCategory ] = true;
};

bool NBFileSystemModel::isCategoryVisible( QString mCategory ) const {

	if ( rootNode->categoryList().contains( mCategory ) )
		return categoryVisibilityMap[ mCategory ];

	return false;
};

int NBFileSystemModel::indexListCountForCategory( QString mCategory ) const {

	if ( not rootNode->categoryList().contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return 0;

	return categoryRowMap[ mCategory ].count();
};

QModelIndexList NBFileSystemModel::indexListForCategory( QString mCategory ) const {

	QModelIndexList mList;

	if ( not rootNode->categoryList().contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return mList;

	foreach( int row, categoryRowMap[ mCategory ] ) {
		mList << index( row, 0, QModelIndex() );
	}

	return mList;
};

bool NBFileSystemModel::showHidden() const {

	return __showHidden;
};

void NBFileSystemModel::setShowHidden( bool shown ) {

	__showHidden = shown;

	if ( not __rootPath.isNull() or not __rootPath.isEmpty() )
		setupModelData();
};

bool NBFileSystemModel::readOnly() const {

	return __readOnly;
};

void NBFileSystemModel::setReadOnly( bool ro ) {

	__readOnly = ro;
};

Qt::DropActions NBFileSystemModel::supportedDragActions() const {

	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
};

Qt::DropActions NBFileSystemModel::supportedDropActions() const {

	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
};

Qt::ItemFlags NBFileSystemModel::flags( const QModelIndex index ) const {

	if ( isReadable( nodePath( index ) ) )
		return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled;

	else
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
};

QStringList NBFileSystemModel::mimeTypes() const {

	QStringList types;
	types << "text/uri-list";
	return types;
};

bool NBFileSystemModel::filter( Filters filter ) {

	Q_UNUSED( filter );

	return false;
};

void NBFileSystemModel::setFilter( Filters filter, bool on ) {

	Q_UNUSED( filter );
	Q_UNUSED( on );

	return;
};

QStringList NBFileSystemModel::nameFilters() {

	return __nameFilters;
};

void NBFileSystemModel::setNameFilters( QStringList filters ) {

	__nameFilters.clear();
	__nameFilters << filters;

	// setupModelData();
};

void NBFileSystemModel::clearNameFilters() {

	__nameFilters.clear();
	setupModelData();
};

void NBFileSystemModel::sort( int column, bool cs, bool categorized ) {

	prevSort.column = column;
	prevSort.cs = cs;
	prevSort.categorized = categorized;

	recategorize();
	rootNode->sort( column, cs, categorized );

	// Create a map of categoryIdndex versus rows
	categoryRowMap.clear();
	foreach( NBFileSystemNode *item, rootNode->children() )
		categoryRowMap[ item->category() ] << item->row();

	foreach( QString mCategory, rootNode->categoryList() )
		categoryVisibilityMap[ mCategory ] = true;

	emit directoryLoaded( __rootPath );
};

int NBFileSystemModel::sortColumn() const {

	return prevSort.column;
};

bool NBFileSystemModel::sortCaseSensitivity() const {

	return prevSort.cs;
};

bool NBFileSystemModel::sortCategorized() const {

	return prevSort.categorized;
};

void NBFileSystemModel::reload() {

	setupModelData();
};

bool NBFileSystemModel::mkdir( QString folderTree ) {

	if ( __readOnly )
		return false;

	return QDir( __rootPath ).mkpath( folderTree );
};

bool NBFileSystemModel::remove( QString path ) {

	Q_UNUSED( path );

	if ( __readOnly )
		return false;

	return true;
};

bool NBFileSystemModel::copy( QString, QString ) {

	if ( __readOnly )
		return false;

	return true;
};

bool NBFileSystemModel::move( QString, QString ) {

	if ( __readOnly )
		return false;

	return true;
};

bool NBFileSystemModel::rename( QString, QString ) {

	if ( __readOnly )
		return false;

	return true;
};

bool NBFileSystemModel::chmod( QString, int ) {

	if ( __readOnly )
		return false;

	return true;
};

QString NBFileSystemModel::nodeName( const QModelIndex index ) const {

	return index.data( 0 ).toString();
};

QString NBFileSystemModel::nodePath( const QModelIndex index ) const {

	return __rootPath + index.data( 0 ).toString();
};

QString NBFileSystemModel::nodePath( const QString path ) const {

	return __rootPath + path;
};

QFileInfo NBFileSystemModel::nodeInfo( const QModelIndex index ) const {

	return QFileInfo( __rootPath + index.data( 0 ).toString() );
};

QString NBFileSystemModel::rootPath() const {

	return __rootPath;
};

void NBFileSystemModel::setRootPath( QString path ) {

	if ( path == QString( "NB://Applications" ) ) {
		oldRoots << path;
		curIndex = oldRoots.count() - 1;
		emit loadApplications();
		return;
	}

	if ( path == QString( "NB://Catalogs" ) ) {
		oldRoots << path;
		curIndex = oldRoots.count() - 1;
		emit loadCatalogs();
		return;
	}

	__rootPath = ( path.endsWith( "/" ) ? path : path + "/" );

	if ( oldRoots.count() )
		oldRoots.erase( oldRoots.begin() + curIndex + 1, oldRoots.end() );

	oldRoots << __rootPath;
	curIndex = oldRoots.count() - 1;

	emit loadFolders();

	delete rootNode;

	rootNode = new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( path ), "" );
	if ( curentLoadStatus.loading )
		curentLoadStatus.stopLoading = true;

	if ( __rootPath != "/dev/" ) {
		watcher->setWatchPath( path );
		if ( not watcher->isRunning() )
			watcher->startWatch();
	}

	else {
		if ( watcher->isRunning() )
			watcher->stopWatch();
	}

	setupModelData();
};

void NBFileSystemModel::goBack() {

	if ( canGoBack() ) {
		curIndex--;

		if ( oldRoots.at( curIndex ) == QString( "NB://Applications" ) ) {
			emit loadApplications();
			return;
		}

		if ( oldRoots.at( curIndex ) == QString( "NB://Catalogs" ) ) {
			emit loadCatalogs();
			return;
		}

		emit loadFolders();

		__rootPath = oldRoots.at( curIndex );

		delete rootNode;
		rootNode = new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath ), "" );

		setupModelData();
	}
};

void NBFileSystemModel::goForward() {

	if ( canGoForward() ) {
		curIndex++;

		if ( oldRoots.at( curIndex ) == QString( "NB://Applications" ) ) {
			emit loadApplications();
			return;
		}

		if ( oldRoots.at( curIndex ) == QString( "NB://Catalogs" ) ) {
			emit loadCatalogs();
			return;
		}

		emit loadFolders();

		__rootPath = oldRoots.at( curIndex );

		delete rootNode;
		rootNode = new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath ), "" );

		setupModelData();
	}
};

void NBFileSystemModel::goUp() {

	if ( canGoUp() ) {
		QString newRoot = __rootPath.section( "/", 0, -3 );
		newRoot = ( newRoot.endsWith( "/" ) ? newRoot : newRoot + "/" );
		setRootPath( newRoot );
	}
};

void NBFileSystemModel::goHome() {

	setRootPath( QDir::homePath() );
};

bool NBFileSystemModel::canGoBack() const {

	return ( curIndex > 0 );
};

bool NBFileSystemModel::canGoForward() const {

	return ( curIndex < ( oldRoots.count() - 1 ) ? true : false);
};

bool NBFileSystemModel::canGoUp() const {

	if ( oldRoots.at( curIndex ).startsWith( "NB://" ) )
		return false;

	return ( __rootPath != "/" );
};

QString NBFileSystemModel::previousDir() const {

	if ( canGoBack() )
		return oldRoots.at( curIndex - 1 );

	else
		return QString();
};

QString NBFileSystemModel::nextDir() const {

	if ( canGoForward() )
		return oldRoots.at( curIndex + 1 );

	else
		return QString();
};

QString NBFileSystemModel::currentDir() const {

	return __rootPath;
};

QString NBFileSystemModel::parentDir() const {

	QString path = __rootPath.section( "/", 0, -3 );
	return ( path.endsWith( "/" ) ? path : path + "?" );
};

void NBFileSystemModel::setupModelData() {

	DIR *dir;
	struct dirent *ent;
	dir = opendir( qPrintable( __rootPath ) );

	__childNames.clear();
	rootNode->clearChildren();
	curentLoadStatus.loading = true;

	emit dirLoading( __rootPath );

	beginResetModel();
	if ( dir != NULL ) {
		while ( ( ent = readdir( dir ) ) != NULL) {
			if ( curentLoadStatus.stopLoading ) {
				endResetModel();
				curentLoadStatus.stopLoading = false;
				closedir( dir );
				return;
			}
			/*
				*
				* Do not show . and ..
				*
			*/

			QString nodeName = QString::fromLocal8Bit( ent->d_name );
			if ( ( nodeName.compare( "." ) == 0 ) or ( nodeName.compare( ".." ) == 0 ) )
				continue;

			QVariantList data = quickDataGatherer->getQuickFileInfo( __rootPath + nodeName );
			if ( __showHidden ) {
				if ( __nameFilters.count() ) {
					if ( isDir( __rootPath + nodeName ) ) {
						rootNode->addChild( new NBFileSystemNode( data, getCategory( data ), rootNode ) );
						__childNames << nodeName;
					}
					else if ( matchesFilter( __nameFilters, nodeName ) ) {
						rootNode->addChild( new NBFileSystemNode( data, getCategory( data ), rootNode ) );
						__childNames << nodeName;
					}
				}
				else {
					rootNode->addChild( new NBFileSystemNode( data, getCategory( data ), rootNode ) );
					__childNames << nodeName;
				}
			}
			else {
				if ( not nodeName.startsWith( "." ) ) {
					if ( __nameFilters.count() ) {
						if ( isDir( __rootPath + nodeName ) ) {
							rootNode->addChild( new NBFileSystemNode( data, getCategory( data ), rootNode ) );
							__childNames << nodeName;
						}
						else if ( matchesFilter( __nameFilters, nodeName ) ) {
							rootNode->addChild( new NBFileSystemNode( data, getCategory( data ), rootNode ) );
							__childNames << nodeName;
						}
					}
					else {
						rootNode->addChild( new NBFileSystemNode( data, getCategory( data ), rootNode ) );
						__childNames << nodeName;
					}
				}
			}
		}

		closedir( dir );
	}
	endResetModel();
;
	sort( prevSort.column, prevSort.cs, prevSort.categorized );

	curentLoadStatus.loading = false;

	emit loadFileInfo();
	emit directoryLoaded( __rootPath );
};

QString NBFileSystemModel::getCategory( QVariantList data ) {

	switch( prevSort.column ) {
		/* Name Sorting */
		case 0: {
			if ( data.at( 0 ).toString() == "dir" )
				return data.at( 3 ).toString().toUpper().at( 0 );

			else
				return data.at( 3 ).toString().toUpper().at( 0 ) + QString( " " );
		}

		/* Size Sorting */
		/* Separate directories from files */
		case 1: {
			/* Folders */
			if ( data.at( 0 ).toString() == "dir" )
				return "Folders";
			/* 0 - 100 kB */
			if ( data.at( 1 ).toLongLong() < 102400 )
				return "Tiny";
			/* 100 kB - 10 MB */
			if ( data.at( 1 ).toLongLong() < 10485760 )
				return "Small";
			/* 10 MB - 100 MB */
			if ( data.at( 1 ).toLongLong() < 104857600 )
				return "Medium";
			/* 100 MB - 1 GB */
			if ( data.at( 1 ).toLongLong() < 1073741824 )
				return "Large";
			/* 0 - 100 kB */
			if ( data.at( 1 ).toLongLong() >= 1073741824 )
				return "Massive";
		}

		/* Type Sorting */
		case 2: {
			if ( data.at( 0 ).toString() == "dir" ) {
				return "Folders";
			}

			if ( data.at( 0 ).toString() == "file" ) {
				return data.at( 5 ).toString();
			}

			if ( data.at( 0 ).toString() == "system" ) {
				return "System Files";
			}
		}

		case 4: {
			QDate date = QDate::fromString( data.at( 7 ).toString(), "ddd, dd MMM, yyyy" );
			return date.toString( "MMMM yyyy" );
		}

		default: {
			return QString( "Uncategorized" );
		}
	}

	return QString( "Uncategorized" );
};

void NBFileSystemModel::recategorize() {

	foreach( NBFileSystemNode *node, rootNode->children() )
		node->setCategory( getCategory( node->allData() ) );

	rootNode->updateCategories();
};

void NBFileSystemModel::gatherFileInfo() {

	updatedNodes = 0;

	NBFileInfoGatherer *ig = new NBFileInfoGatherer();
	connect(
		ig, SIGNAL( done( QString, QString, QStringList ) ),
		this, SLOT( saveInfo( QString, QString, QStringList ) )
	);

	ig->gatherInfo( __childNames, __rootPath );
};

void NBFileSystemModel::saveInfo( QString root, QString entry, QStringList info ) {

	/*
		 *
		 * info -> [ iconStr, Type, MimeType ]
		 * idx  -> [    s2  ,  n2 ,    n3    ]
		 *
	*/

	if ( root != __rootPath )
		return;

	if ( not exists( root + entry ) )
		return;

	QModelIndex idx = index( entry );
	NBFileSystemNode *node = rootNode->child( entry );

	node->setData( 2, info.at( 0 ), true );
	node->setData( 2, info.at( 1 ), false );
	node->setData( 3, info.at( 2 ), false );

	updatedNodes++;

	if ( updatedNodes == rowCount() )
		emit updatedAllNodes();

	emit dataChanged( idx, idx );
};

void NBFileSystemModel::handleNodeCreated( QString node ) {

	if ( baseName( node ).startsWith( "." ) )
		return;

	if ( dirName( node ) == currentDir() )
		insertNode( baseName( node ) );
};

void NBFileSystemModel::handleNodeChanged( QString node ) {

	if ( baseName( node ).startsWith( "." ) )
		return;

	if ( dirName( node ) == currentDir() )
		updateNode( baseName( node ) );
};

void NBFileSystemModel::handleNodeDeleted( QString node ) {

	if ( baseName( node ).startsWith( "." ) )
		return;

	if ( dirName( node ) == currentDir() )
		removeNode( baseName( node ) );
};

void NBFileSystemModel::loadHome() {

	emit runningHome( currentDir() );
	goHome();
};

void NBFileSystemModel::sort() {

	sort( prevSort.column, prevSort.cs, prevSort.categorized );
};
