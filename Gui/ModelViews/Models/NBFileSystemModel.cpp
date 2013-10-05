/*
	*
	* NBFileSystemModel.cpp - NewBreeze NBFileSystemModel Class
	*
*/

#include <NBFileSystemModel.hpp>
#include <NBTools.cpp>

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

	rootNode = new NBFileSystemNode( QVariantList() );
	quickDataGatherer = new NBQuickFileInfoGatherer();

	watcher = new NBFileSystemWatcher();

	connect( watcher, SIGNAL( dirCreated( QString ) ), this, SLOT( handleDirChanged( QString ) ) );
	connect( watcher, SIGNAL( dirChanged( QString ) ), this, SLOT( handleDirChanged( QString ) ) );
	connect( watcher, SIGNAL( dirDeleted( QString ) ), this, SLOT( handleDirChanged( QString ) ) );
	connect( watcher, SIGNAL( fileCreated( QString ) ), this, SLOT( handleFileChanged( QString ) ) );
	connect( watcher, SIGNAL( fileChanged( QString ) ), this, SLOT( handleFileChanged( QString ) ) );
	connect( watcher, SIGNAL( fileDeleted( QString ) ), this, SLOT( handleFileChanged( QString ) ) );
	connect( watcher, SIGNAL( watchPathDeleted() ), this, SLOT( loadHome() ) );

	connect( this, SIGNAL( loadFileInfo() ), this, SLOT( gatherFileInfo() ) );
};

NBFileSystemModel::~NBFileSystemModel() {

};

int NBFileSystemModel::rowCount( const QModelIndex & parent ) const {

	if ( parent.column() > 0 )
		return 0;

	if ( not parent.isValid() )
		return rootNode->childCount();

	else
		return static_cast<NBFileSystemNode*>( parent.internalPointer() )->childCount();
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
				// The inbuilt icon or from the file system
				QIcon savedIcon = QIcon( node->data( 2, true ).toString() );
				// Icon we got from the theme
				QIcon themeIcon = QIcon::fromTheme( node->data( 2, true ).toString() );

				if ( themeIcon.isNull() and savedIcon.isNull() )
					return unknown;

				else if ( themeIcon.isNull() and not savedIcon.isNull() )
					return savedIcon;

				else
					return themeIcon;
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

	if ( __showHidden ) {
		if ( __nameFilters.count() ) {
			if ( matchesFilter( __nameFilters, nodeName ) ) {
				rootNode->appendChild( new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath + nodeName ), rootNode ) );
				__childNames << nodeName;
			}
		}
		else {
			rootNode->appendChild( new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath + nodeName ), rootNode ) );
			__childNames << nodeName;
		}
	}
	else {
		if ( not nodeName.startsWith( "." ) ) {
			if ( __nameFilters.count() ) {
				if ( matchesFilter( __nameFilters, nodeName ) ) {
					rootNode->appendChild( new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath + nodeName ), rootNode ) );
					__childNames << nodeName;
				}
			}
			else {
				rootNode->appendChild( new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath + nodeName ), rootNode ) );
				__childNames << nodeName;
			}
		}
	}

	sort( prevSort.column, prevSort.cs, prevSort.dirsFirst );

	NBFileInfoGatherer *ig = new NBFileInfoGatherer();
	connect(
		ig, SIGNAL( done( QString, QString, QStringList ) ),
		this, SLOT( saveInfo( QString, QString, QStringList ) )
	);

	ig->gatherInfo( QStringList() << nodeName, __rootPath );

	return true;
};

bool NBFileSystemModel::removeNode( QString name ) {

	Q_UNUSED( name );
	// I have no idea what to do here

	return false;
};

void NBFileSystemModel::updateNode( QString nodeName ) {

	NBFileInfoGatherer *ig = new NBFileInfoGatherer();
	connect(
		ig, SIGNAL( done( QString, QString, QStringList ) ),
		this, SLOT( saveInfo( QString, QString, QStringList ) )
	);

	ig->gatherInfo( QStringList() << nodeName, __rootPath );

	return;
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

bool NBFileSystemModel::showHidden() const {

	return __showHidden;
};

void NBFileSystemModel::setShowHidden( bool shown ) {

	__showHidden = shown;
	setupModelData();
};

bool NBFileSystemModel::readOnly() const {

	return __readOnly;
};

void NBFileSystemModel::setReadOnly( bool ro ) {

	__readOnly = ro;
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

void NBFileSystemModel::sort( int column, bool cs, bool dirsFirst ) {

	prevSort.column = column;
	prevSort.cs = cs;
	prevSort.dirsFirst = dirsFirst;

	rootNode->sort( column, cs, dirsFirst );
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

	__rootPath = ( path.endsWith( "/" ) ? path : path + "/" );

	if ( oldRoots.count() )
		oldRoots.erase( oldRoots.begin() + curIndex + 1, oldRoots.end() );

	oldRoots << __rootPath;
	curIndex = oldRoots.count() - 1;

	delete rootNode;

	rootNode = new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( path ) );
	if ( curentLoadStatus.loading )
		curentLoadStatus.stopLoading = true;

	watcher->setWatchPath( path );

	if ( not watcher->isRunning() )
		watcher->startWatch();

	setupModelData();
};

void NBFileSystemModel::goBack() {

	if ( canGoBack() ) {
		curIndex--;

		__rootPath = oldRoots[ curIndex ];

		delete rootNode;
		rootNode = new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath ) );

		setupModelData();
	}
};

void NBFileSystemModel::goForward() {

	if ( canGoForward() ) {
		curIndex++;

		__rootPath = oldRoots.at( curIndex );

		delete rootNode;
		rootNode = new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath ) );

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

			if ( __showHidden ) {
				if ( __nameFilters.count() ) {
					if ( matchesFilter( __nameFilters, nodeName ) ) {
						rootNode->appendChild( new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath + nodeName ), rootNode ) );
						__childNames << nodeName;
					}
				}
				else {
					rootNode->appendChild( new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath + nodeName ), rootNode ) );
					__childNames << nodeName;
				}
			}
			else {
				if ( not nodeName.startsWith( "." ) ) {
					if ( __nameFilters.count() ) {
						if ( matchesFilter( __nameFilters, nodeName ) ) {
							rootNode->appendChild( new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath + nodeName ), rootNode ) );
							__childNames << nodeName;
						}
					}
					else {
						rootNode->appendChild( new NBFileSystemNode( quickDataGatherer->getQuickFileInfo( __rootPath + nodeName ), rootNode ) );
						__childNames << nodeName;
					}
				}
			}
		}

		closedir( dir );
	}
	endResetModel();

	sort( prevSort.column, prevSort.cs, prevSort.dirsFirst );

	curentLoadStatus.loading = false;

	emit loadFileInfo();
	emit directoryLoaded( __rootPath );
};

void NBFileSystemModel::gatherFileInfo() {

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
	NBFileSystemNode *node = rootNode->child( idx.row() );

	node->setData( 2, info.at( 0 ), true );
	node->setData( 2, info.at( 1 ), false );
	node->setData( 3, info.at( 2 ), false );

	emit dataChanged( idx, idx );
};

void NBFileSystemModel::handleDirChanged( QString dir ) {

	if ( ( not showHidden() ) and baseName( dir ).startsWith( "." ) )
		return;

	reload();
};

void NBFileSystemModel::handleFileChanged( QString file ) {

	if ( ( not showHidden() ) and baseName( file ).startsWith( "." ) )
		return;

	reload();
};

void NBFileSystemModel::loadHome() {

	qDebug() << "Current dir was deleted. Going back home.";

	emit runningHome( currentDir() );
	goHome();
};
