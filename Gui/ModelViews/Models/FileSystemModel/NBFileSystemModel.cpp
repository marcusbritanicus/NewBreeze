/*
	*
	* NBFileSystemModel.cpp - NewBreeze NBFileSystemModel Class
	*
*/

#include <NBFileSystemModel.hpp>

inline bool matchesFilter( QStringList filters, QString text ) {

	Q_FOREACH( QString filter, filters )
		if ( text.contains( QRegExp( filter, Qt::CaseInsensitive, QRegExp::Wildcard ) ) )
			return true;

	return false;
};

NBFileSystemModel::NBFileSystemModel() : QAbstractItemModel() {

	__showHidden = false;
	__filterFolders = Settings->General.FilterFolders;
	updatedNodes = 0;

	__terminate = false;

	__mCombiShown = false;

	mCategorizationEnabled = false;

	currentLoadStatus.loading = false;
	currentLoadStatus.stopLoading = false;

	prevSort.column = Settings->General.SortColumn;
	prevSort.cs = Settings->General.SortCase;
	prevSort.categorized = Settings->General.Grouping;

	oldRoots.clear();
	curIndex = 0;

	rootNode = new NBFileSystemNode();

	quickDataGatherer = new NBQuickFileInfoGatherer();

	watcher = new NBFileSystemWatcher();

	connect( watcher, SIGNAL( nodeCreated( QString ) ), this, SLOT( handleNodeCreated( QString ) ) );
	connect( watcher, SIGNAL( nodeChanged( QString ) ), this, SLOT( handleNodeChanged( QString ) ) );
	connect( watcher, SIGNAL( nodeDeleted( QString ) ), this, SLOT( handleNodeDeleted( QString ) ) );
	connect( watcher, SIGNAL( nodeRenamed( QString, QString ) ), this, SLOT( handleNodeRenamed( QString, QString ) ) );

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

	if ( isDir( nodePath( index ) ) )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
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
				// Icon String
				QString icoStr( node->data( 2, true ).toString() );

				// Icon we got from the theme
				QIcon themeIcon = QIcon::fromTheme( icoStr, QIcon( icoStr ) );

				if ( not themeIcon.isNull() )
					return themeIcon;

				else
					return QIcon::fromTheme( "unknown", QIcon( ":/icons/unknown.png" ) );
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

	NBFileInfoGatherer *ig = new NBFileInfoGatherer( &__terminate );
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

	NBFileInfoGatherer *ig = new NBFileInfoGatherer( &__terminate );
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

    if (row < 0 || column < 0)
        return QModelIndex();

    if ( not ( ( row < rowCount( parent ) ) and ( column < columnCount( parent ) ) ) )
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
		return QString( "" );

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

QModelIndexList NBFileSystemModel::categorySiblings( QModelIndex idx ) const {

	if ( not idx.isValid() )
		return QModelIndexList();

	QModelIndexList mList = indexListForCategory( category( idx ) );
	mList.removeAll( idx );

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

	setupModelData();
};

void NBFileSystemModel::clearNameFilters() {

	__nameFilters.clear();
	setupModelData();
};

bool NBFileSystemModel::filterFolders() const {

	return __filterFolders;
};

void NBFileSystemModel::setFilterFolders( bool filter) {

	__filterFolders = filter;
	setupModelData();
};

void NBFileSystemModel::sort( int column, bool cs, bool categorized ) {

	prevSort.column = column;
	prevSort.cs = cs;
	prevSort.categorized = categorized;

	recategorize();
	rootNode->sort( column, cs, categorized );

	// Create a map of categoryIndex versus rows
	categoryRowMap.clear();
	foreach( NBFileSystemNode *item, rootNode->children() )
		categoryRowMap[ item->category() ] << item->row();

	emit layoutChanged();
};

void NBFileSystemModel::reload() {

	setupModelData();
};

bool NBFileSystemModel::rename( QString oldName, QString newName ) {

	// Do nothing: This should never happen
	if ( __mCombiShown )
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
		NBFileSystemNode *node = rootNode->child( baseName( oldName ) );
		node->setData( 0, baseName( newName ) );
		updateNode( baseName( newName ) );

		return true;
	}

	/* This can never happen! */
	return false;
};

QString NBFileSystemModel::nodeName( const QModelIndex index ) const {

	return index.data( 0 ).toString();
};

QString NBFileSystemModel::nodePath( const QModelIndex index ) const {

	if ( __mCombiShown ) {
		NBFileSystemNode *node = static_cast<NBFileSystemNode*>( index.internalPointer() );
		return node->data( 7 ).toString();
	}

	else
		return __rootPath + index.data( 0 ).toString();
};

QString NBFileSystemModel::nodePath( const QString path ) const {

	if ( __mCombiShown ) {
		QModelIndex idx = index( path );
		return idx.data( 7 ).toString();
	}

	else
		return __rootPath + path;
};

QFileInfo NBFileSystemModel::nodeInfo( const QModelIndex index ) const {

	if ( __mCombiShown ) {
		return QFileInfo( nodePath( index ) );
	}

	else
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
	__mCombiShown = false;

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

	/* If we are in NB://Applications or NB://Catalogs mode, come to FolderView mode */
	emit loadFolders();

	/* Delete the old node */
	delete rootNode;

	/* Create a fresh root node */
	rootNode = new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( path ), "" );

	/* Stop loading thumbs, or even nodes */
	if ( currentLoadStatus.loading )
		currentLoadStatus.stopLoading = true;

	/* If the root path is /dev/, then stop the watcher */
	if ( __rootPath == "/dev/" ) {
		if ( watcher->isRunning() )
			watcher->stopWatch();
	}

	/* For all the other folders, we can happily start the watcher */
	else {
		watcher->setWatchPath( path );
		if ( not watcher->isRunning() )
			watcher->startWatch();
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

void NBFileSystemModel::loadCombiView() {

	goHome();
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
		return QString( "" );
};

QString NBFileSystemModel::nextDir() const {

	if ( canGoForward() )
		return oldRoots.at( curIndex + 1 );

	else
		return QString( "" );
};

QString NBFileSystemModel::currentDir() const {

	return __rootPath;
};

QString NBFileSystemModel::parentDir() const {

	QString path = __rootPath.section( "/", 0, -3 );
	return ( path.endsWith( "/" ) ? path : path + "/" );
};

void NBFileSystemModel::setupModelData() {

	DIR *dir;
	struct dirent *ent;
	dir = opendir( qPrintable( __rootPath ) );

	__childNames.clear();
	rootNode->clearChildren();
	currentLoadStatus.loading = true;

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

			/* Hide Hidden */
			else {
				if ( not nodeName.startsWith( "." ) ) {
					/* We want to filter folders too */
					if ( __nameFilters.count() ) {
						if ( not __filterFolders and ent->d_type == DT_DIR ) {
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

	sort( prevSort.column, prevSort.cs, prevSort.categorized );

	currentLoadStatus.loading = false;

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

void NBFileSystemModel::recategorize() {

	foreach( NBFileSystemNode *node, rootNode->children() )
		if ( __mCombiShown )
			node->setCategory( "My Computer" );

		else
			node->setCategory( getCategory( node->allData() ) );

	rootNode->updateCategories();
};

void NBFileSystemModel::gatherFileInfo() {

	updatedNodes = 0;

	NBFileInfoGatherer *ig = new NBFileInfoGatherer( &__terminate );
	connect(
		ig, SIGNAL( done( QString, QString, QStringList ) ),
		this, SLOT( saveInfo( QString, QString, QStringList ) )
	);

	ig->gatherInfo( __childNames, __rootPath );
};

void NBFileSystemModel::terminateInfoGathering() {

	__terminate = true;
};

void NBFileSystemModel::saveInfo( QString root, QString entry, QStringList info ) {

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

	if ( baseName( node ).startsWith( "." ) and not __showHidden )
		return;

	if ( dirName( node ) == currentDir() )
		insertNode( baseName( node ) );

	else
		qDebug() << dirName( node ) << currentDir();
};

void NBFileSystemModel::handleNodeChanged( QString node ) {

	if ( baseName( node ).startsWith( "." ) )
		return;

	if ( dirName( node ) == currentDir() )
		updateNode( baseName( node ) );
};

void NBFileSystemModel::handleNodeDeleted( QString node ) {

	if ( baseName( node ).startsWith( "." ) and not __showHidden )
		return;

	if ( dirName( node ) == currentDir() )
		removeNode( baseName( node ) );
};

void NBFileSystemModel::handleNodeRenamed( QString oldNode, QString newNode ) {

	rename( oldNode, newNode );
};

void NBFileSystemModel::loadHome() {

	emit runningHome( currentDir() );
	goHome();
};

void NBFileSystemModel::sort() {

	sort( prevSort.column, prevSort.cs, prevSort.categorized );
};
