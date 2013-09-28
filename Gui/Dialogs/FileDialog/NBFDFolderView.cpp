/*
	*
	* NBFDFolderView.cpp - NewBreeze Restricted FolderView Class
	*
*/

#include <NBFDFolderView.hpp>
#include <NBMessageDialog.hpp>
#include <NBGuiWidgets.hpp>
#include <NBFVDialogs.hpp>
#include <NBTools.hpp>

NBFDFolderView::NBFDFolderView( QString location ) : QListView() {

	contentsWidth = 640;

	// Setup Data Model
	fsModel = new NBFileSystemModel();
	fsModel->setRootPath( location );
	setModel( fsModel );

	// Item Delegate
	iDelegate = new NBFDItemDelegate();
	setItemDelegate( iDelegate );

	// SHow Hidden Files
	showHidden = false;

	// View, Sizes and Resize Modes
	updateViewMode();
	setUniformItemSizes( true );

	// Minimum Width - 640px
	setMinimumWidth( 640 );

	setResizeMode( QListView::Adjust );
	setMovement( QListView::Static );
	setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );

	// DragAndDrop
	setDragDropMode( QAbstractItemView::DragDrop );

	// Selection
	setSelectionMode( QAbstractItemView::ExtendedSelection );

	// Wrapping
	setWordWrap( true );
	setWrapping( true );

	// Context Menu
	setContextMenuPolicy( Qt::CustomContextMenu );

	// Setup the watchdog;
	watchDog = new QFileSystemWatcher();

	// Create and setup actions like rename, etc, etc
	createAndSetupActions();

	// Cut
	toBeCut = false;
};

QSize NBFDFolderView::getGridSize( QSize mSize ) {

	int items = ( int )( contentsWidth / mSize.width() );
	int gWidth = ( int )( contentsWidth / items );

	return QSize( gWidth, mSize.height() );
};

void NBFDFolderView::updateViewMode() {

	// View and Resize Modes
	setViewMode( QListView::IconMode );
	setFlow( QListView::LeftToRight );

	// Sizes
	setGridSize( getGridSize( QSize( 120, 80 ) ) );
	setIconSize( QSize( 48, 48 ) );

	// Wrapping
	setWrapping( true );
};

void NBFDFolderView::setContentsWidth( int cWidth ) {

	contentsWidth = cWidth;
};

void NBFDFolderView::mouseDoubleClickEvent( QMouseEvent *mouseEvent ) {

	QModelIndex idx = indexAt( mouseEvent->pos() );
	if ( idx.isValid() )
		emit doubleClicked( idx );

	mouseEvent->accept();
};

void NBFDFolderView::createAndSetupActions() {

	connect(
		this, SIGNAL( activated( QModelIndex ) ),
		this, SLOT( doOpen( QModelIndex ) )
	);

	connect(
		this, SIGNAL( doubleClicked( QModelIndex ) ),
		this, SLOT( doOpen( QModelIndex ) )
	);

	connect(
		this, SIGNAL( customContextMenuRequested( QPoint ) ),
		this, SLOT( contextMenuRequested( QPoint ) )
	);

	connect(
		watchDog, SIGNAL( directoryChanged( const QString& ) ),
		this, SLOT( handleWatchDogBark( QString ) )
	);

	// New Folder
	actNewDir = new QAction( QIcon::fromTheme( "folder-new" ), "New folder", this );
	actNewDir->setShortcuts( Settings.Shortcuts.NewFolder );

	connect( actNewDir, SIGNAL( triggered() ), this, SLOT( newFolder() ) );
	addAction( actNewDir );

	// New file
	actNewFile = new QAction( QIcon::fromTheme( "document-new" ), "New File", this );
	actNewFile->setShortcuts( Settings.Shortcuts.NewFile );

	connect( actNewFile, SIGNAL( triggered() ), this, SLOT( newFile() ) );
	addAction( actNewFile );

	// Reload
	reload = new QAction( QIcon( ":/icons/reload.png" ), "Re&fresh", this );
	reload->setShortcuts( Settings.Shortcuts.Reload );

	connect( reload, SIGNAL( triggered() ), this, SLOT( doReload() ) );
	addAction( reload );

	// showDotFiles
	showHideDotFiles = new QAction( QIcon( ":/icons/showDotFiles.png" ), "Show &Hidden", this );
	showHideDotFiles->setShortcuts( Settings.Shortcuts.ToggleHidden );

	connect( showHideDotFiles, SIGNAL( triggered() ), this, SLOT( doShowHideDotFiles() ) );
	addAction( showHideDotFiles );

	// Sorting
	sortByNameAct = new QAction( "&Name", this );
	connect( sortByNameAct, SIGNAL( triggered() ), this, SLOT( sortByName() ) );

	sortByTypeAct = new QAction( "&Type", this );
	connect( sortByTypeAct, SIGNAL( triggered() ), this, SLOT( sortByType() ) );

	sortBySizeAct = new QAction( "&Size", this );
	connect( sortBySizeAct, SIGNAL( triggered() ), this, SLOT( sortBySize() ) );

	sortByDateAct = new QAction( "&Date", this );
	connect( sortByDateAct, SIGNAL( triggered() ), this, SLOT( sortByDate() ) );

	// Show/Hide side bar
	QAction *toggleSideBar = new QAction( "Toggle Sidebar", this );
	toggleSideBar->setShortcuts( Settings.Shortcuts.ToggleSideBar );

	connect( toggleSideBar, SIGNAL( triggered() ), this, SIGNAL( showHideSideBar() ) );
	addAction( toggleSideBar );
};

QList<QModelIndex> NBFDFolderView::getSelection() {

	QList<QModelIndex> fileList = selectedIndexes();
	return fileList;
};

void NBFDFolderView::goUp() {

	fsModel->goUp();
};

void NBFDFolderView::goBack() {

	fsModel->goBack();
};

void NBFDFolderView::goForward() {

	fsModel->goForward();
};

void NBFDFolderView::goHome() {

	fsModel->goHome();
};

void NBFDFolderView::newFile() {

	NBNewFileFolderDialog *newFile = new NBNewFileFolderDialog( "file", QDir( fsModel->currentDir() ) );
	newFile->exec();

	doReload();
};

void NBFDFolderView::newFolder() {

	NBNewFileFolderDialog *newFolder = new NBNewFileFolderDialog( "dir", QDir( fsModel->currentDir() ) );
	newFolder->exec();

	doReload();
};

void NBFDFolderView::doOpen( QModelIndex idx ) {

	Q_UNUSED( idx );
	QList<QModelIndex> selectedList = selectedIndexes();

	foreach( QModelIndex index, selectedList ) {
		QFileInfo info = fsModel->nodeInfo( index );
		QString fileToBeOpened = info.absoluteFilePath();

		bool hasPermission = true;
		hasPermission &= info.isReadable();
		if ( info.isDir() )
			hasPermission &= info.isExecutable();

		if ( !hasPermission ) {
			QString title = tr( "NewBreeze - Access Error" );
			QString text = tr( "You do not have enough permissions to open the <b>%1</b>. " ).arg( info.fileName() );
			if ( info.isDir() )
				text += tr( "Please change the permissions of the directory to enter it." );

			else
				text += tr( "Please change the permissions of the file to edit/view it." );

			NBMessageDialog::error( title, text );
			return;
		}

		if ( info.isDir() ) {
			NBDebugMsg( DbgMsgPart::ONESHOT, "Opening dir: %s", qPrintable( fileToBeOpened ) );
			fsModel->setRootPath( fileToBeOpened );
		}

		else if  ( info.isFile() ) {
			if ( info.isExecutable() and isExec( fileToBeOpened ) ) {
				/*
					*
					* We make sure that @v fileToBeOpened is really an executable file,
					* i.e it is one of shellscript, install file, or x-exec or x-sharedlib
					* or something of the sort and not a jpg file with exec perms
					*
				*/
				NBDebugMsg( DbgMsgPart::HEAD, "Executing: %s... ", qPrintable( fileToBeOpened ) );
				NBDebugMsg( DbgMsgPart::TAIL, ( QProcess::startDetached( info.absoluteFilePath() ) ? "[DONE]" : "[FAILED]" ) );
			}

			else {
				NBDebugMsg( DbgMsgPart::HEAD, "Opening file: %s... ", qPrintable( fileToBeOpened ) );
				NBDebugMsg( DbgMsgPart::TAIL, ( QProcess::startDetached( "xdg-open", QStringList() << fileToBeOpened ) ? "[DONE]" : "[FAILED]" ) );
			}
		}

		else {
			if ( selectedIndexes().count() ) {
				NBMessageDialog::critical( "NewBreeze - Error", "I really do not have any idea how to open this file." );
				return;
			}
		}
	}

	setFocus();
	emit updateToolBar();
};

void NBFDFolderView::doOpen( QString loc ) {

	if ( !QFileInfo( loc ).isAbsolute() )
		loc = QFileInfo( loc ).absoluteFilePath();

	QFileInfo info( loc );

	bool hasPermission = true;
	hasPermission &= info.isReadable();

	if ( !hasPermission ) {
		QString title = tr( "NewBreeze - Access Error" );
		QString text = tr( "You do not have enough permissions to open the <b>%1</b>. " ).arg( info.fileName() );
		if ( info.isDir() )
			text += tr( "Please change the permissions of the directory to enter it." );

		else
			text += tr( "Please change the permissions of the file to edit/view it." );

		NBMessageDialog::error( title, text );
		return;
	}

	if ( info.isDir() )
		hasPermission &= info.isExecutable();

	if ( info.isDir() ) {
		NBDebugMsg( DbgMsgPart::ONESHOT, "Opening dir: %s", qPrintable( loc ) );
		fsModel->setRootPath( loc );
	}

	else if  ( info.isFile() ) {
		NBDebugMsg( DbgMsgPart::HEAD, "Opening file: %s", qPrintable( loc ) );
		NBDebugMsg( DbgMsgPart::TAIL, ( QProcess::startDetached( "xdg-open", QStringList() << loc ) ? "[DONE]" : " [FAILED]" ) );
	}

	else {
		if ( selectedIndexes().count() ) {
			NBMessageDialog::critical( "NewBreeze - Error", "I really do not have any idea how to open this file." );
			return;
		}
	}
};

void NBFDFolderView::doReload() {

	fsModel->reload();
};

void NBFDFolderView::doShowHideDotFiles() {

	if ( showHidden ) {
		showHidden = false;
		fsModel->setShowHidden( false );
	}

	else {
		showHidden = true;
		fsModel->setShowHidden( true );
	}

	settings.setValue( "Session/ShowHidden", showHidden );
	settings.sync();
};

void NBFDFolderView::sortByName() {

	bool descending = qobject_cast<QAction*>( sender() )->data().toBool();

	if ( descending )
		fsModel->sort( 0, Qt::DescendingOrder );

	else
		fsModel->sort( 0, Qt::AscendingOrder );
};

void NBFDFolderView::sortBySize() {

	bool descending = qobject_cast<QAction*>( sender() )->data().toBool();

	if ( descending )
		fsModel->sort( 1, Qt::DescendingOrder );

	else
		fsModel->sort( 1, Qt::AscendingOrder );
};

void NBFDFolderView::sortByType() {

	bool descending = qobject_cast<QAction*>( sender() )->data().toBool();

	if ( descending )
		fsModel->sort( 2, Qt::DescendingOrder );

	else
		fsModel->sort( 2, Qt::AscendingOrder );
};

void NBFDFolderView::sortByDate() {

	bool descending = qobject_cast<QAction*>( sender() )->data().toBool();

	if ( descending )
		fsModel->sort( 3, Qt::DescendingOrder );

	else
		fsModel->sort( 3, Qt::AscendingOrder );
};

void NBFDFolderView::toBeImplemented() {

	qDebug() << "something was called";
};

void NBFDFolderView::handleWatchDogBark( QString path ) {

	QFileInfo info( path );
	if ( !info.exists() ) {
		NBMessageDialog::critical( "NewBreeze - Error",
			"The current directory has been moved or deleted by an external process. Loading home dir." );

		goHome();
	}
	else {
		doReload();
	}
};

void NBFDFolderView::contextMenuRequested( const QPoint &position ) {

    NBMenu *menu = new NBMenu();

	// Create a new file/directory
	NBMenu *createNewMenu = new NBMenu( "Create &New" );
	createNewMenu->setIcon( QIcon::fromTheme( "archive-insert" ) );

	createNewMenu->addAction( actNewDir );
	createNewMenu->addAction( actNewFile );

	// File/directory sorting
	NBMenu *sortMenu = new NBMenu( "&Sort by" );
	sortMenu->setIcon( QIcon::fromTheme( "view-sort-ascending" ) );

	sortMenu->addAction( sortByNameAct );
	sortMenu->addAction( sortBySizeAct );
	sortMenu->addAction( sortByTypeAct );
	sortMenu->addAction( sortByDateAct );

	menu->addMenu( createNewMenu );
	menu->addSeparator();

	menu->addAction( reload );
	menu->addSeparator();

	menu->addSeparator();
	menu->addMenu( sortMenu );

    menu->exec( mapToGlobal( position ) );
};
