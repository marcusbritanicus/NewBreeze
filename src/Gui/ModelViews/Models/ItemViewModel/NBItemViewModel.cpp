/*
	*
	* NBItemViewModel.cpp - NewBreeze NBItemViewModel Class
	*
*/

#include <NBItemViewModel.hpp>
#include <NBAppEngine.hpp>

static QMutex mutex;

inline bool matchesFilter( QStringList filters, QString text ) {

	Q_FOREACH( QString filter, filters )
		if ( text.contains( QRegExp( filter, Qt::CaseInsensitive, QRegExp::Wildcard ) ) )
			return true;

	return false;
};

NBIconUpdater::NBIconUpdater( QString root, QStringList entries, bool *term ) : QThread() {

	__terminate = term;

	entryList.clear();
	entryList << entries;

	rootPath.clear();
	rootPath = root;
	if ( not root.endsWith( "/" ) )
		rootPath += "/";
};

NBIconUpdater::~NBIconUpdater() {

	terminate();
	wait();
};

void NBIconUpdater::run() {

	if ( rootPath != "/dev/" ) {
		foreach( QString entry, entryList ) {
			if ( *__terminate )
				break;

			QMimeType mimeType = mimeDb.mimeTypeForFile( rootPath + entry );
			emit updated( rootPath, entry, QStringList() << NBIconProvider::icon( rootPath + entry, mimeType ) );
		}
	}
};

NBItemViewModel::NBItemViewModel() : QAbstractItemModel() {

	/* By default we don't show hidden files */
	__showHidden = false;

	/* Filter both files and folders? */
	__filterFolders = Settings->General.FilterFolders;

	/* Switch for temination of data gathering */
	__terminate = false;

	/* Do we show the special directories */
	__mModelDataType = NBItemViewModel::FileSystem;
	__mVirtualData = false;

	/* Categrorization enabled by default */
	mCategorizationEnabled = Settings->General.Grouping;

	/* Useless swtiches */
	currentLoadStatus.loading = false;
	currentLoadStatus.stopLoading = false;

	prevSort.column = Settings->General.SortColumn;
	prevSort.cs = Settings->General.SortCase;
	prevSort.categorized = Settings->General.Grouping;

	oldRoots.clear();
	curIndex = 0;

	/* Root node */
	rootNode = new NBItemViewNode();

	quickDataGatherer = new NBQuickFileInfoGatherer();

	/* NBFileSystemWatcher */
	watcher = new NBFileSystemWatcher();
	connect( watcher, SIGNAL( nodeCreated( QString ) ), this, SLOT( handleNodeCreated( QString ) ) );
	connect( watcher, SIGNAL( nodeChanged( QString ) ), this, SLOT( handleNodeChanged( QString ) ) );
	connect( watcher, SIGNAL( nodeDeleted( QString ) ), this, SLOT( handleNodeDeleted( QString ) ) );
	connect( watcher, SIGNAL( nodeRenamed( QString, QString ) ), this, SLOT( handleNodeRenamed( QString, QString ) ) );
	connect( watcher, SIGNAL( watchPathDeleted() ), this, SLOT( loadHome() ) );

	// connect( this, SIGNAL( directoryLoaded( QString ) ), this, SLOT( updateAllNodes( QString ) ) );
};

NBItemViewModel::~NBItemViewModel() {

};

bool NBItemViewModel::isCategorizationEnabled() {

	return mCategorizationEnabled;
};

void NBItemViewModel::setCategorizationEnabled( bool enabled ) {

	mCategorizationEnabled = enabled;
	sort( prevSort.column, prevSort.cs, enabled );
};

int NBItemViewModel::rowCount( const QModelIndex & parent ) const {

	if ( parent.column() > 0 )
		return 0;

	if ( not parent.isValid() )
		return rootNode->childCount();

	else
		return static_cast<NBItemViewNode*>( parent.internalPointer() )->childCount();
};

int NBItemViewModel::rowCount( QString mCategory ) const {

	if ( not rootNode->categoryList().contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return 0;

	return categoryRowMap[ mCategory ].count();
};

int NBItemViewModel::categoryCount() const {

	return rootNode->categoryCount();
};

int NBItemViewModel::columnCount( const QModelIndex & parent ) const {

	Q_UNUSED( parent );
	return 7;
};

Qt::ItemFlags NBItemViewModel::flags( const QModelIndex & index ) const {

	if ( not index.isValid() )
		return Qt::NoItemFlags;

	if ( isDir( nodePath( index ) ) )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
};

QVariant NBItemViewModel::data( const QModelIndex &index, int role ) const {

	if ( not index.isValid() )
		return QVariant();

	NBItemViewNode *node = static_cast<NBItemViewNode*>( index.internalPointer() );
	switch( role ) {

		case Qt::DisplayRole: {
			if (  ( index.column() >= 0 ) and ( index.column() <= 9  ) )
				return node->data( index.column() );

			else
				return QVariant();
		}

		case Qt::DecorationRole: {
			if ( index.column() == 0 ) {
				switch( __mModelDataType ) {
					case NBItemViewModel::Applications: {
						QString icoStr( node->data( 2, true ).toString() );
						QIcon icon = QIcon::fromTheme( icoStr );

						if ( icon.isNull() )
							icon = QIcon( NBIconProvider::pixmapIcon( icoStr ) );

						if ( icon.isNull() )
							icon = QIcon::fromTheme( "application-x-desktop", QIcon::fromTheme( "application-x-executable", QIcon( ":/icons/exec.png" ) ) );

						return icon;
					}

					case NBItemViewModel::SuperStart:
					case NBItemViewModel::Catalogs:
					case NBItemViewModel::FileSystem: {
						// Icon String
						QString icoStr( node->data( 2, true ).toString() );

						// Icon we got from the theme
						QIcon themeIcon = QIcon::fromTheme( icoStr, QIcon( icoStr ) );

						if ( not themeIcon.isNull() )
							return themeIcon;

						else
							return QIcon::fromTheme( "unknown", QIcon( ":/icons/unknown.png" ) );
					}
				}
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
				case 0:	{
					if ( __mModelDataType == NBItemViewModel::Applications )
						return QString( "<center>%1<br>%2</center>" ).arg( node->data( 0 ).toString() ).arg( node->data( 2 ).toString() );

					else
						return QString( "<center>%1<br>%2</center>" ).arg( node->data( 0 ).toString() ).arg( node->data( 1 ).toString() );

				}
				default: {

					return QString( "<b>%1</b>" ).arg( node->data( index.column() ).toString() );
				}
			}
		}

		case Qt::UserRole + 0: {
			return node->data( 0 );
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

		case Qt::UserRole + 7: {
			return node->data( 7 );
		}

		case Qt::UserRole + 8: {
			return node->data( 8 );
		}

		case Qt::UserRole + 9: {
			return node->data( 9 );
		}

		default: {
			return QVariant();
		}
	}
};

QVariant NBItemViewModel::headerData( int section, Qt::Orientation orientation, int role ) const {

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

bool NBItemViewModel::setData( const QModelIndex &index, QVariant value, int role ) {

	bool ok = rootNode->child( index.row() )->setData( role, value );
	if ( ok )
		emit dataChanged( index, index );

	return ok;
};

bool NBItemViewModel::isRealLocation() const {

	return not __mVirtualData;
};

quint64 NBItemViewModel::modelDataType() const {

	return __mModelDataType;
};

bool NBItemViewModel::insertNode( QString nodeName ) {

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
				rootNode->addChild( new NBItemViewNode( data, getCategory( data ), rootNode ) );
				__childNames << nodeName;
			}
		}
		else {
			rootNode->addChild( new NBItemViewNode( data, getCategory( data ), rootNode ) );
			__childNames << nodeName;
		}
	}
	else {
		if ( not nodeName.startsWith( "." ) ) {
			if ( __nameFilters.count() ) {
				if ( matchesFilter( __nameFilters, nodeName ) ) {
					rootNode->addChild( new NBItemViewNode( data, getCategory( data ), rootNode ) );
					__childNames << nodeName;
				}
			}
			else {
				rootNode->addChild( new NBItemViewNode( data, getCategory( data ), rootNode ) );
				__childNames << nodeName;
			}
		}
	}
	endInsertRows();

	sort( prevSort.column, prevSort.cs, prevSort.categorized );
	endResetModel();

	NBFileInfoGatherer *ig = new NBFileInfoGatherer( &__terminate );
	connect(
		ig, SIGNAL( done( QString, QString, QStringList ) ),
		this, SLOT( saveInfo( QString, QString, QStringList ) )
	);

	ig->gatherInfo( QStringList() << nodeName, __rootPath );

	return true;
};

void NBItemViewModel::updateNode( QString nodeName ) {

	QMutexLocker locker( &mutex );
	if ( not exists( __rootPath + nodeName ) )
		return;

	if ( not __childNames.contains( nodeName ) )
		return;

	NBItemViewNode *node = rootNode->child( nodeName );
	if ( isDir( __rootPath + nodeName ) ) {
		node->setData( 1, nChildren( __rootPath + nodeName ), true );
		node->setData( 1, QString( "%1 items" ).arg( node->data( 1, true ).toLongLong() ), false );
	}

	else {
		node->setData( 1, getSize( __rootPath + nodeName ), true );
		node->setData( 1, formatSize( node->data( 1, true ).toLongLong() ), false );
	}

	NBFileInfoGatherer *ig = new NBFileInfoGatherer( &__terminate );
	connect(
		ig, SIGNAL( done( QString, QString, QStringList ) ),
		this, SLOT( saveInfo( QString, QString, QStringList ) )
	);

	ig->gatherInfo( QStringList() << nodeName, __rootPath );
	sort( prevSort.column, prevSort.cs, prevSort.categorized );
};

void NBItemViewModel::updateDelayedNodes() {

	/* Make a local copy */
	QStringList nodeList;
	nodeList << delayedUpdateList;

	/* Clear the delayed nodes list */
	delayedUpdateList.clear();

	QMutexLocker locker( &mutex );
	Q_FOREACH( QString node, nodeList )
		updateNode( baseName( node ) );
};

bool NBItemViewModel::removeNode( QString nodeName ) {

	if ( not __childNames.contains( nodeName ) )
		return false;

	__childNames.removeAll( nodeName );
	NBItemViewNode *removedNode = rootNode->child( nodeName );

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

QModelIndex NBItemViewModel::index( int row, int column, const QModelIndex &parent ) const {

    if (row < 0 || column < 0)
        return QModelIndex();

    if ( not ( ( row < rowCount( parent ) ) and ( column < columnCount( parent ) ) ) )
		return QModelIndex();

	NBItemViewNode *parentNode;
	if ( not parent.isValid() )
		parentNode = rootNode;

	else
		parentNode = (NBItemViewNode *)parent.internalPointer();

	NBItemViewNode *childNode = parentNode->child( row );
	if ( childNode )
		return createIndex( row, column, childNode );

	else
		return QModelIndex();
};

QModelIndex NBItemViewModel::index( QString name, const QModelIndex &parent ) const {

	if ( not __childNames.contains( name ) )
		return QModelIndex();

	NBItemViewNode *parentNode;
	if ( not parent.isValid() )
		parentNode = rootNode;

	else
		parentNode = (NBItemViewNode *)parent.internalPointer();

	int row = parentNode->child( name )->row();
	NBItemViewNode *childNode = parentNode->child( name );

	if ( childNode )
		return createIndex( row, 0, childNode );

	return QModelIndex();
};

QModelIndex NBItemViewModel::parent( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QModelIndex();

	NBItemViewNode *childNode = ( NBItemViewNode * )index.internalPointer();
	NBItemViewNode *parentNode = childNode->parent();

	if ( parentNode == rootNode )
		return QModelIndex();

	return createIndex( parentNode->row(), 0, parentNode );
};

QString NBItemViewModel::category( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QString( "" );

	NBItemViewNode *childItem = ( NBItemViewNode * )index.internalPointer();
	return childItem->category();
};

int NBItemViewModel::categoryIndex( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return -1;

	NBItemViewNode *childItem = ( NBItemViewNode * )index.internalPointer();
	return childItem->categoryIndex();
};

QStringList NBItemViewModel::categories() const {

	return rootNode->categoryList();
};

QPixmap NBItemViewModel::pixmapForCategory( QString categoryName ) const {

	switch ( __mModelDataType ) {

		case NBItemViewModel::Catalogs: {

			if ( categoryName == "Documents" )
				return QIcon( ":/icons/documents.png" ).pixmap( 16, 16 );

			else if ( categoryName == "Music" )
				return QIcon::fromTheme( "folder-sound" ).pixmap( 16, 16 );

			else if ( categoryName == "Pictures" )
				return QIcon::fromTheme( "folder-image" ).pixmap( 16, 16 );

			else if ( categoryName == "Videos" )
				return QIcon::fromTheme( "folder-video" ).pixmap( 16, 16 );

			else
				return QIcon( ":/icons/catalogs-alt.png" ).pixmap( 16, 16 );
		}

		default: {
			if ( categoryVisibilityMap[ categoryName ] == false )
				return QIcon::fromTheme( "arrow-right" ).pixmap( 16, 16 );

			else
				return QIcon::fromTheme( "arrow-down" ).pixmap( 16, 16 );
		}
	}
};

void NBItemViewModel::foldCategory( QString categoryName ) {

	categoryVisibilityMap[ categoryName ] = false;
};

void NBItemViewModel::openCategory( QString categoryName ) {

	categoryVisibilityMap[ categoryName ] = true;
};

bool NBItemViewModel::isCategoryOpen( QString categoryName ) const {

	return categoryVisibilityMap[ categoryName ];
};

int NBItemViewModel::indexListCountForCategory( QString mCategory ) const {

	if ( not rootNode->categoryList().contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return 0;

	return categoryRowMap[ mCategory ].count();
};

QModelIndexList NBItemViewModel::indexListForCategory( QString mCategory ) const {

	QModelIndexList mList;

	if ( not rootNode->categoryList().contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return mList;

	foreach( int row, categoryRowMap[ mCategory ] ) {
		mList << index( row, 0, QModelIndex() );
	}

	return mList;
};

QModelIndexList NBItemViewModel::categorySiblings( QModelIndex idx ) const {

	if ( not idx.isValid() )
		return QModelIndexList();

	QModelIndexList mList = indexListForCategory( category( idx ) );
	mList.removeAll( idx );

	return mList;
};

bool NBItemViewModel::showHidden() const {

	return __showHidden;
};

void NBItemViewModel::setShowHidden( bool shown ) {

	__showHidden = shown;

	if ( not __rootPath.isNull() or not __rootPath.isEmpty() )
		setupModelData();
};

Qt::DropActions NBItemViewModel::supportedDragActions() const {

	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
};

Qt::DropActions NBItemViewModel::supportedDropActions() const {

	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
};

Qt::ItemFlags NBItemViewModel::flags( const QModelIndex index ) const {

	if ( isReadable( nodePath( index ) ) )
		return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled;

	else
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
};

QStringList NBItemViewModel::mimeTypes() const {

	QStringList types;
	types << "text/uri-list";
	return types;
};

bool NBItemViewModel::filter( Filters filter ) {

	Q_UNUSED( filter );

	return false;
};

void NBItemViewModel::setFilter( Filters filter, bool on ) {

	Q_UNUSED( filter );
	Q_UNUSED( on );

	return;
};

QStringList NBItemViewModel::nameFilters() {

	return __nameFilters;
};

void NBItemViewModel::setNameFilters( QStringList filters ) {

	__nameFilters.clear();
	__nameFilters << filters;

	setupModelData();
};

void NBItemViewModel::clearNameFilters() {

	__nameFilters.clear();
	setupModelData();
};

bool NBItemViewModel::filterFolders() const {

	return __filterFolders;
};

void NBItemViewModel::setFilterFolders( bool filter) {

	__filterFolders = filter;
	setupModelData();
};

void NBItemViewModel::sort( int column, bool cs, bool categorized ) {

	prevSort.column = column;
	prevSort.cs = cs;
	prevSort.categorized = categorized;

	recategorize();
	rootNode->sort( column, cs, categorized );

	// Create a map of categoryIndex versus rows
	categoryRowMap.clear();
	foreach( NBItemViewNode *item, rootNode->children() )
		categoryRowMap[ item->category() ] << item->row();

	emit layoutChanged();
};

void NBItemViewModel::reload() {

	setupModelData();
};

bool NBItemViewModel::rename( QString oldName, QString newName ) {

	// Do nothing: This should never happen
	if ( __mVirtualData )
		return true;

	/* If the file @oldName is not from the current directory */
	if ( dirName( oldName ) != __rootPath ) {
		insertNode( baseName( newName ) );
		return true;
	}

	/* If the file @newName is not in the current dir */
	else if ( dirName( oldName ) != __rootPath ) {
		removeNode( baseName( oldName ) );
		return true;
	}

	else {
		/* Same folder */
		NBItemViewNode *node = rootNode->child( baseName( oldName ) );
		node->setData( 0, baseName( newName ) );
		updateNode( baseName( newName ) );

		return true;
	}

	/* This can never happen! */
	return false;
};

QString NBItemViewModel::nodeName( const QModelIndex idx ) const {

	return idx.data( 0 ).toString();
};

QString NBItemViewModel::nodePath( const QModelIndex idx ) const {

	return idx.data( Qt::UserRole + 7 ).toString();
};

QString NBItemViewModel::nodePath( const QString path ) const {

	if ( __mVirtualData ) {
		QModelIndex idx = index( path );
		return idx.data( Qt::UserRole + 7 ).toString();
	}

	else
		return __rootPath + path;
};

QFileInfo NBItemViewModel::nodeInfo( const QModelIndex idx ) const {

	switch ( __mModelDataType ) {

		case NBItemViewModel::Applications: {

			return QFileInfo( idx.data( Qt::UserRole + 9 ).toString() );
		}

		case NBItemViewModel::SuperStart:
		case NBItemViewModel::Catalogs:
		case NBItemViewModel::FileSystem:
			return QFileInfo( idx.data( Qt::UserRole + 7 ).toString() );
	}
};

QString NBItemViewModel::rootPath() const {

	return __rootPath;
};

void NBItemViewModel::setRootPath( QString path ) {

	if ( path == QString( "NB://SuperStart" ) ) {
		__mVirtualData = true;
		__mModelDataType = (quint64)NBItemViewModel::SuperStart;
	}

	else if ( path == QString( "NB://Applications" ) ) {
		__mVirtualData = true;
		__mModelDataType = (quint64)NBItemViewModel::Applications;
	}

	else if ( path == QString( "NB://Catalogs" ) ) {
		__mVirtualData = true;
		__mModelDataType = (quint64)NBItemViewModel::Catalogs;
	}

	else {
		__mVirtualData = false;
		__mModelDataType = (quint64)NBItemViewModel::FileSystem;
	}


	/* Add a trailing / only if we have a real location */
	if ( not __rootPath.startsWith( "NB://" ) )
		__rootPath = ( path.endsWith( "/" ) ? path : path + "/" );

	/* Navigation: If we are in the middle, remove all 'forawrd' roots */
	if ( oldRoots.count() )
		oldRoots.erase( oldRoots.begin() + curIndex + 1, oldRoots.end() );

	/* Append this root to navigation list */
	oldRoots << __rootPath;
	curIndex = oldRoots.count() - 1;

	/* chdir( __rootPath ) to set the correct link to /pwd/<pid>/cwd */
	chdir( __rootPath.toLocal8Bit().constData() );

	/* We have set per-folder settings */
	QSettings sett( __rootPath + ".directory", QSettings::NativeFormat );

	/* Check per folder view settings */
	__showHidden = sett.value( "NewBreeze/Hidden", false ).toBool();

	prevSort.column = sett.value( "NewBreeze/SortColumn", Settings->General.SortColumn ).toInt();
	prevSort.cs = sett.value( "NewBreeze/SortCase", Settings->General.SortCase ).toBool();
	prevSort.categorized = sett.value( "NewBreeze/Grouping", Settings->General.Grouping ).toBool();
	mCategorizationEnabled = prevSort.categorized;

	/* Delete the old node */
	delete rootNode;

	/* Create a fresh root node */
	rootNode = new NBItemViewNode( quickDataGatherer->getQuickFileInfo( path ), "" );

	/* Stop loading thumbs, or even nodes */
	if ( currentLoadStatus.loading )
		currentLoadStatus.stopLoading = true;

	/* If the root path is /dev/, then stop the watcher */
	if ( __rootPath.startsWith( "/dev/" ) )
		newWatch( QString() );

	/* For all the other folders, we can happily start the watcher */
	else
		newWatch( path );

	setupModelData();
};

void NBItemViewModel::goBack() {

	if ( canGoBack() ) {
		curIndex--;

		__rootPath = oldRoots.at( curIndex );

		delete rootNode;
		rootNode = new NBItemViewNode( quickDataGatherer->getQuickFileInfo( __rootPath ), "" );

		setupModelData();
	}
};

void NBItemViewModel::goForward() {

	if ( canGoForward() ) {
		curIndex++;

		__rootPath = oldRoots.at( curIndex );

		delete rootNode;
		rootNode = new NBItemViewNode( quickDataGatherer->getQuickFileInfo( __rootPath ), "" );

		setupModelData();
	}
};

void NBItemViewModel::goUp() {

	if ( canGoUp() ) {
		QString newRoot = __rootPath.section( "/", 0, -3 );
		newRoot = ( newRoot.endsWith( "/" ) ? newRoot : newRoot + "/" );
		setRootPath( newRoot );
	}
};

void NBItemViewModel::goHome() {

	setRootPath( QDir::homePath() );
};

bool NBItemViewModel::canGoBack() const {

	return ( curIndex > 0 );
};

bool NBItemViewModel::canGoForward() const {

	return ( curIndex < ( oldRoots.count() - 1 ) ? true : false);
};

bool NBItemViewModel::canGoUp() const {

	if ( oldRoots.at( curIndex ).startsWith( "NB://" ) )
		return false;

	return ( __rootPath != "/" );
};

QString NBItemViewModel::previousDir() const {

	if ( canGoBack() )
		return oldRoots.at( curIndex - 1 );

	else
		return QString( "" );
};

QString NBItemViewModel::nextDir() const {

	if ( canGoForward() )
		return oldRoots.at( curIndex + 1 );

	else
		return QString( "" );
};

QString NBItemViewModel::currentDir() const {

	return __rootPath;
};

QString NBItemViewModel::parentDir() const {

	QString path = __rootPath.section( "/", 0, -3 );
	return ( path.endsWith( "/" ) ? path : path + "/" );
};

void NBItemViewModel::setupModelData() {

	switch( __mModelDataType ) {
		case NBItemViewModel::SuperStart: {
			goHome();
			break;
		}

		case NBItemViewModel::Applications: {
			setupApplicationsData();
			break;
		}

		case NBItemViewModel::Catalogs: {
			setupCatalogData();
			break;
		}

		case NBItemViewModel::FileSystem: {
			setupFileSystemData();
			break;
		}
	}
};

void NBItemViewModel::setupFileSystemData() {

	DIR *dir;
	struct dirent *ent;
	dir = opendir( __rootPath.toLocal8Bit().data() );

	__childNames.clear();
	rootNode->clearChildren();
	currentLoadStatus.loading = true;

	/* These are useful only when we do not refresh or change into some directory. */
	lastUpdatedNodes.clear();
	lastUpdatedTimes.clear();
	delayedUpdateList.clear();
	updateTimer.stop();

	emit dirLoading( __rootPath );

	beginResetModel();
	if ( dir != NULL ) {
		while ( ( ent = readdir( dir ) ) != NULL) {
			if ( currentLoadStatus.stopLoading ) {
				endResetModel();
				currentLoadStatus.stopLoading = false;
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
			/* Show Hidden */
			if ( __showHidden ) {
				/* We should be able to filter folders if need be */
				if ( __nameFilters.count() ) {
					/* If filter folders is false, don't bother about filtering folders */
					if ( not __filterFolders and ent->d_type == DT_DIR ) {
						rootNode->addChild( new NBItemViewNode( data, getCategory( data ), rootNode ) );
						__childNames << nodeName;
					}

					else if ( matchesFilter( __nameFilters, nodeName ) ) {
						rootNode->addChild( new NBItemViewNode( data, getCategory( data ), rootNode ) );
						__childNames << nodeName;
					}
				}
				else {
					rootNode->addChild( new NBItemViewNode( data, getCategory( data ), rootNode ) );
					__childNames << nodeName;
				}
			}

			/* Hide Hidden */
			else {
				if ( not nodeName.startsWith( "." ) ) {
					/* We want to filter folders too */
					if ( __nameFilters.count() ) {
						if ( not __filterFolders and ent->d_type == DT_DIR ) {
							rootNode->addChild( new NBItemViewNode( data, getCategory( data ), rootNode ) );
							__childNames << nodeName;
						}

						else if ( matchesFilter( __nameFilters, nodeName ) ) {
							rootNode->addChild( new NBItemViewNode( data, getCategory( data ), rootNode ) );
							__childNames << nodeName;
						}
					}

					else {
						rootNode->addChild( new NBItemViewNode( data, getCategory( data ), rootNode ) );
						__childNames << nodeName;
					}
				}
			}
		}

		closedir( dir );
	}
	endResetModel();

	foreach( QString mCategoryName, rootNode->categoryList() )
		categoryVisibilityMap[ mCategoryName ] = true;

	sort( prevSort.column, prevSort.cs, prevSort.categorized );

	currentLoadStatus.loading = false;

	emit directoryLoaded( __rootPath );

	updateAllNodes( __rootPath );
};

void NBItemViewModel::setupApplicationsData() {

	__childNames.clear();
	rootNode->clearChildren();
	currentLoadStatus.loading = true;

	emit dirLoading( __rootPath );

	beginResetModel();
	NBAppEngine *appEngine = NBAppEngine::instance();
	foreach( NBAppFile app, appEngine->allDesktops().toQList() ) {
		if ( ( app.value( NBAppFile::Type ) != "Application" ) or ( app.value( NBAppFile::NoDisplay ).toBool() ) )
			continue;

		QVariantList data;

		/* Special Data */
		data << "System" << 0 << app.value( NBAppFile::Icon );

		/* Normal Data */
		data << app.value( NBAppFile::Name );												/* Qt::UserRole + 0 */
		data << app.value( NBAppFile::Exec );												/* Qt::UserRole + 1 */
		data << app.value( NBAppFile::Comment );											/* Qt::UserRole + 2 */
		data << app.execArgs();																/* Qt::UserRole + 3 */
		data << app.value( NBAppFile::Icon );												/* Qt::UserRole + 4 */
		data << app.value( NBAppFile::WorkPath );											/* Qt::UserRole + 5 */
		data << app.value( NBAppFile::MimeTypes );											/* Qt::UserRole + 6 */
		data << app.value( NBAppFile::TerminalMode );										/* Qt::UserRole + 7 */
		data << app.value( NBAppFile::Categories );											/* Qt::UserRole + 8 */
		data << app.filePath();																/* Qt::UserRole + 9 */

		rootNode->addChild( new NBItemViewNode( data, app.category(), rootNode ) );
	}
	endResetModel();

	foreach( QString mCategoryName, rootNode->categoryList() )
		categoryVisibilityMap[ mCategoryName ] = true;

	sort( prevSort.column, prevSort.cs, prevSort.categorized );

	currentLoadStatus.loading = false;

	emit directoryLoaded( __rootPath );
};

void NBItemViewModel::setupCatalogData() {

	__childNames.clear();
	rootNode->clearChildren();
	currentLoadStatus.loading = true;

	emit dirLoading( __rootPath );

	QSettings catalogsSettings( "NewBreeze", "Catalogs" );
	/* Default Catalogs */
	beginResetModel();
	foreach( QString ctlg, catalogsSettings.childKeys() ) {
		QStringList locations = catalogsSettings.value( ctlg ).toStringList();
		foreach( QString location, locations ) {
			if ( not exists( location ) )
				continue;

			QVariantList data = quickDataGatherer->getQuickFileInfo( location );
			rootNode->addChild( new NBItemViewNode( data, ctlg, rootNode ) );
		}
	}

	/* Custom Catalogs */
	catalogsSettings.beginGroup( "Custom" );
	foreach( QString ctlg, catalogsSettings.childKeys() ) {
		QStringList locations = catalogsSettings.value( ctlg ).toStringList();
		foreach( QString location, locations ) {
			if ( not exists( location ) )
				continue;

			QVariantList data = quickDataGatherer->getQuickFileInfo( location );
			rootNode->addChild( new NBItemViewNode( data, ctlg, rootNode ) );
		}
	}
	catalogsSettings.endGroup();
	endResetModel();

	foreach( QString mCategoryName, rootNode->categoryList() )
		categoryVisibilityMap[ mCategoryName ] = true;

	sort( prevSort.column, prevSort.cs, prevSort.categorized );

	currentLoadStatus.loading = false;

	emit directoryLoaded( __rootPath );
};

void NBItemViewModel::newWatch( QString path ) {

	/* If there is a watch running: delete it and create a new watcher object */
	if ( watcher->isRunning() ) {

		/* Delete the current watch */
		watcher->stopWatch();
		delete watcher;

		/* Create a new watch */
		watcher = new NBFileSystemWatcher();
		connect( watcher, SIGNAL( nodeCreated( QString ) ), this, SLOT( handleNodeCreated( QString ) ) );
		connect( watcher, SIGNAL( nodeChanged( QString ) ), this, SLOT( handleNodeChanged( QString ) ) );
		connect( watcher, SIGNAL( nodeDeleted( QString ) ), this, SLOT( handleNodeDeleted( QString ) ) );
		connect( watcher, SIGNAL( nodeRenamed( QString, QString ) ), this, SLOT( handleNodeRenamed( QString, QString ) ) );
		connect( watcher, SIGNAL( watchPathDeleted() ), this, SLOT( loadHome() ) );

		/* Path is not empty and exists, start the watch */
		if ( path.count() and exists( path ) )
			watcher->startWatch( path );
	}

	/* At this point there will always be a valid watcher object */
	else {
		/* Path is not empty and exists, start the watch */
		if ( path.count() and exists( path ) )
			watcher->startWatch( path );
	}
};

QString NBItemViewModel::getCategory( QVariantList data ) {

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
			if ( date.isValid() ) {
				if ( date == QDate::currentDate() )
					return "Today";

				else if ( date.weekNumber() == QDate::currentDate().weekNumber() )
					return "This Week";

				else if (  date.weekNumber() == QDate::currentDate().weekNumber() - 1 )
					return "Last Week";

				else if (  date.month() == QDate::currentDate().month() )
					return "This Month";

				else if (  date.month()== QDate::currentDate().month() - 1 )
					return "Last Month";

				else
					return date.toString( "MMMM yyyy" );
			}

			return "Uncategorized";
		}

		default: {
			return QString( "Uncategorized" );
		}
	}

	return QString( "Uncategorized" );
};

void NBItemViewModel::recategorize() {

	foreach( NBItemViewNode *node, rootNode->children() )
		if ( not __mVirtualData )
			node->setCategory( getCategory( node->allData() ) );

	rootNode->updateCategories();
};

void NBItemViewModel::terminateInfoGathering() {

	__terminate = true;
};

void NBItemViewModel::updateAllNodes( QString root ) {

	NBIconUpdater *iconUpdater = new NBIconUpdater( root, __childNames, &__terminate );
	connect( iconUpdater, SIGNAL( updated( QString, QString, QStringList ) ), this, SLOT( saveInfo( QString, QString, QStringList ) ) );

	iconUpdater->start();
};

void NBItemViewModel::saveInfo( QString root, QString entry, QStringList info ) {

	/*
		 *
		 * info -> [ iconStr, Type, MimeType ]
		 * idx  -> [   s2   ,  n2 ,    n3    ]
		 *
	*/

	if ( root != __rootPath )
		return;

	if ( not exists( root + entry ) )
		return;

	QModelIndex idx = index( entry );
	NBItemViewNode *node = rootNode->child( entry );

	/* Updating the icon */
	if (  info.count() == 1 ) {

		node->setData( 2, info.at( 0 ), true );
	}

	/* Updating the icons, mime and category */
	else if (  info.count() == 3 ) {
		node->setData( 2, info.at( 0 ), true );
		node->setData( 2, info.at( 1 ), false );
		node->setData( 3, info.at( 2 ), false );
	}

	emit dataChanged( idx, idx );
};

void NBItemViewModel::handleNodeCreated( QString node ) {

	if ( baseName( node ).startsWith( "." ) and not __showHidden )
		return;

	if ( dirName( node ) == currentDir() )
		insertNode( baseName( node ) );
};

/*
	*
	* We have updated the three function to ignore rapid inotify events triggered
	* due to file copy. When a file is copied, it is rapidly changed. If an event is
	* triggered for the same node in succession, then we quarantine it for a second.
	*
*/
void NBItemViewModel::handleNodeChanged( QString node ) {

	if ( baseName( node ).startsWith( "." ) )
		return;

	if ( dirName( node ) == currentDir() ) {
		qint64 idx = lastUpdatedNodes.indexOf( node );
		if ( idx == -1 ) {
			updateNode( baseName( node ) );
			lastUpdatedNodes << node;
			lastUpdatedTimes << QTime::currentTime();
		}

		else {
			/* Past update was more than a second ago */
			if ( lastUpdatedTimes.at( idx ).secsTo( QTime::currentTime() ) > 10000 ) {
				lastUpdatedTimes.replace( idx, QTime::currentTime() );
				updateNode( baseName( node ) );
			}

			else {
				/* Add the node to the delayed update list */
				if ( not delayedUpdateList.contains( node ) )
					delayedUpdateList << node;

				/* Start timer to perform the update two seconds later */
				if ( not updateTimer.isActive() )
					updateTimer.start( 10000, this );
			}
		}
	}
};

void NBItemViewModel::handleNodeDeleted( QString node ) {

	if ( baseName( node ).startsWith( "." ) and not __showHidden )
		return;

	if ( dirName( node ) == currentDir() )
		removeNode( baseName( node ) );
};

void NBItemViewModel::handleNodeRenamed( QString oldNode, QString newNode ) {

	rename( oldNode, newNode );
};

void NBItemViewModel::loadHome() {

	emit runningHome( currentDir() );
	goHome();
};

void NBItemViewModel::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == updateTimer.timerId() ) {
		// QTimer::singleShot( 100, this, SLOT( updateDelayedNodes() ) );
		// updateTimer.stop();

		return;
	}

	QAbstractItemModel::timerEvent( tEvent );
};
