/*
	*
	* NewBreeze.cpp - NewBreeze MainWindow Class
	*
*/

#include <NewBreeze.hpp>

NewBreeze::NewBreeze( QString loc ) : QMainWindow() {

	/* No terminate signal until close */
	mTerminate = false;

	createGUI();
	if ( not loc.isEmpty() ) {
		if ( isFile( loc ) ) {

			/* This is a file, just open the file */
			openFile( loc );
		}

		else if ( isDir( loc ) ) {

			/* This is a folder open it */
			FolderView->doOpen( loc );
		}

		else if ( exists( loc ) ) {

			/* we do not what this node is */
			qDebug() << "I do not know how to open this:" << loc;
			qDebug() << "Trying xdg-open";
			QProcess proc;
			proc.startDetached( "xdg-open", QStringList() << loc );
		}

		else {

			/* What the hell is this? */
			qDebug() << "What should I do with:" << loc;
		}
	}

	else {
		if ( exists( Settings->Session.LastDir ) )
			FolderView->doOpen( QString( Settings->Session.LastDir ) );

		else
			FolderView->doOpen( QString( QDir::homePath() ) );
	}

	/* Open with catalog */
	if ( Settings->General.OpenWithCatalog and loc.isEmpty() )
		FolderView->setCurrentIndex( 2 );

	/* Show/hide hidden files */
	if ( Settings->General.ShowHidden )
		FolderView->fsModel->setShowHidden( true );

	/* Focus the CurrentWidget in FolderView */
	FolderView->currentWidget()->setFocus();

	/* Window Properties */
	setWindowProperties();

	/* Signal-Slot Connections */
	createAndSetupActions();
};

void NewBreeze::createGUI() {

	QWidget *BodyWidget = new QWidget( this );
	QWidget *MainWidget = new QWidget( this );

	QVBoxLayout *MainLayout = new QVBoxLayout();
	QHBoxLayout *BodyLayout = new QHBoxLayout();
	QVBoxLayout *ViewLayout = new QVBoxLayout();

	MainLayout->setContentsMargins( QMargins() );
	ViewLayout->setContentsMargins( QMargins( 0, 0, 3, 0 ) );
	ViewLayout->setSpacing( 3 );
	BodyLayout->setContentsMargins( QMargins() );
	BodyLayout->setSpacing( 0 );

	AddressBar = new NBAddressBar( this );
	SidePanel = new NBSidePanel( this );
	FolderView = new NBFolderView( this );
	InfoBar = new NBInfoBar( this );
	Terminal = new NBTerminal( QString(), this );
	FilterWidget = new NBFilterWidget( FolderView );

	// Widgets layout
	ViewLayout->addWidget( AddressBar );
	ViewLayout->addWidget( FolderView );

	BodyLayout->addWidget( SidePanel );
	BodyLayout->addLayout( ViewLayout );

	BodyWidget->setLayout( BodyLayout );
	MainLayout->setContentsMargins( QMargins( 0, 5, 0, 0 ) );

	MainLayout->addWidget( BodyWidget );
	MainLayout->addWidget( Terminal );
	MainLayout->addWidget( Separator::horizontal() );
	MainLayout->addWidget( InfoBar );

	MainWidget->setLayout( MainLayout );
	setCentralWidget( MainWidget );

	// Widget Properties
	AddressBar->setFixedHeight( 32 );
	Terminal->setFixedHeight( 270 );
	Terminal->hide();
	FilterWidget->hide();

	AddressBar->setFocusPolicy( Qt::NoFocus );
	SidePanel->setFocusPolicy( Qt::NoFocus );
	FolderView->setFocusPolicy( Qt::StrongFocus );

	setFocusPolicy( Qt::NoFocus );
};

void NewBreeze::setWindowProperties() {

	setWindowTitle( QString( "%1 - NewBreeze" ).arg( baseName( FolderView->fsModel->currentDir() ) ) );
	setWindowIcon( QIcon( ":/icons/newbreeze.png" ) );

	setMinimumSize( 900, 600 );

	setGeometry( Settings->Session.Geometry );
};

void NewBreeze::createAndSetupActions() {

	connect( AddressBar, SIGNAL( changeViewMode( int ) ), this, SLOT( changeViewMode( int ) ) );
	connect( AddressBar, SIGNAL( openLocation( QString ) ), FolderView, SLOT( doOpen( QString ) ) );
	connect( AddressBar, SIGNAL( openSearch() ), FilterWidget, SLOT( show() ) );

	connect( FilterWidget, SIGNAL( search( QString ) ), this, SLOT( filterFiles( QString ) ) );
	connect( FilterWidget, SIGNAL( shown() ), AddressBar, SLOT( hideSearchButton() ) );
	connect( FilterWidget, SIGNAL( hidden() ), AddressBar, SLOT( showSearchButton() ) );
	connect( FilterWidget, SIGNAL( hidden() ), this, SLOT( clearFilters() ) );

	connect( AddressBar, SIGNAL( goBack() ), FolderView, SLOT( goBack() ) );
	connect( AddressBar, SIGNAL( goForward() ), FolderView, SLOT( goForward() ) );

	connect( SidePanel, SIGNAL( driveClicked( QString ) ), this, SLOT( handleDriveUrl( QString ) ) );
	connect( SidePanel, SIGNAL( showFolders() ), this, SLOT( showFolders() ) );
	connect( SidePanel, SIGNAL( showApplications() ), this, SLOT( showApplications() ) );
	connect( SidePanel, SIGNAL( showCatalogs() ), this, SLOT( showCatalogs() ) );
	connect( SidePanel, SIGNAL( showTrash() ), this, SLOT( showTrash() ) );

	connect( SidePanel, SIGNAL( copy( QStringList, QString, NBIOMode::Mode ) ), this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );
	connect( SidePanel, SIGNAL( move( QStringList, QString, NBIOMode::Mode ) ), this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );

	connect( FolderView, SIGNAL( copy( QStringList, QString, NBIOMode::Mode ) ), this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );
	connect( FolderView, SIGNAL( move( QStringList, QString, NBIOMode::Mode ) ), this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );

	connect( FolderView, SIGNAL( showProperties() ), this, SLOT( showProperties() ) );
	connect( FolderView, SIGNAL( showPermissions() ), this, SLOT( showPermissions() ) );

	connect( FolderView, SIGNAL( newWindow( QString ) ), this, SLOT( newWindow( QString ) ) );
	connect( FolderView, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ), this, SLOT( updateInfoBar() ) );
	connect( FolderView, SIGNAL( reloadCatalogs() ), FolderView->widget( 2 ), SLOT( reload() ) );
	connect( FolderView, SIGNAL( toggleGroups() ), this, SLOT( toggleGrouping() ) );

	connect( FolderView, SIGNAL( hideStatusBar() ), InfoBar, SLOT( hide() ) );
	connect( FolderView, SIGNAL( showStatusBar() ), InfoBar, SLOT( show() ) );

	connect( FolderView->fsModel, SIGNAL( dirLoading( QString ) ), this, SLOT( updateVarious( QString ) ) );
	connect( FolderView->fsModel, SIGNAL( dirLoading( QString ) ), this, SLOT( updateInfoBar() ) );
	connect( FolderView->widget( 2 ), SIGNAL( openLocation( QString ) ), FolderView, SLOT( doOpen( QString ) ) );

	// About NB
	QAction *aboutNBAct = new QAction( this );
	aboutNBAct->setShortcuts( Settings->Shortcuts.AboutNB );
	connect( aboutNBAct, SIGNAL( triggered() ), this, SLOT( showAboutNB() ) );

	// About Qt
	QAction *aboutQtAct = new QAction( this );
	aboutQtAct->setShortcuts( Settings->Shortcuts.AboutQt );
	connect( aboutQtAct, SIGNAL( triggered() ), this, SLOT( showAboutQt() ) );

	// About Qt
	QAction *licenseAct = new QAction( this );
	licenseAct->setShortcut( tr( "Ctrl+Shift+L" ) );
	connect( licenseAct, SIGNAL( triggered() ), this, SLOT( showLicense() ) );

	// NB Info
	QAction *showInfoAct = new QAction( this );
	showInfoAct->setShortcuts( Settings->Shortcuts.NBInfo );
	connect( showInfoAct, SIGNAL( triggered() ), this, SLOT( showInfoDlg() ) );

	// Show settings
	QAction *showSettingsAct = new QAction( this );
	showSettingsAct->setShortcuts( Settings->Shortcuts.Settings );
	connect( showSettingsAct, SIGNAL( triggered() ), this, SLOT( showSettingsDialog() ) );

	// Focus AddressBar
	QAction *focusAddressBarAct = new QAction( this );
	focusAddressBarAct->setShortcuts( Settings->Shortcuts.FocusAddressBar );
	connect( focusAddressBarAct, SIGNAL( triggered() ), AddressBar, SLOT( focusAddressEdit() ) );

	// Open new window
	QAction *newWindowAct = new QAction( this );
	newWindowAct->setShortcuts( Settings->Shortcuts.NewWindow );
	connect( newWindowAct, SIGNAL( triggered() ), this, SLOT( newWindow() ) );

	// Add bookmark
	QAction *addBookMarkAct = new QAction( this );
	addBookMarkAct->setShortcuts( Settings->Shortcuts.AddBookmark );
	connect( addBookMarkAct, SIGNAL( triggered() ), this, SLOT( addBookMark() ) );

	// Update devices list
	int mountsFD = open( "/proc/self/mounts", O_RDONLY, 0 );
	QSocketNotifier *devWatcher = new QSocketNotifier( mountsFD, QSocketNotifier::Write );
	connect( devWatcher, SIGNAL( activated( int ) ), SidePanel, SLOT( flashDevices() ) );

	// Display terminal widget
	QAction *termWidgetAct = new QAction( this );
	termWidgetAct->setShortcuts( Settings->Shortcuts.InlineTerminal );
	connect( termWidgetAct, SIGNAL( triggered() ), this, SLOT( showHideTermWidget() ) );
	connect (Terminal, SIGNAL( shown( bool ) ), addBookMarkAct, SLOT( setDisabled( bool ) ) );

	// Show/Hide side bar
	QAction *toggleSideBar = new QAction( "Toggle Sidebar", this );
	toggleSideBar->setShortcuts( Settings->Shortcuts.ToggleSideBar );
	connect( toggleSideBar, SIGNAL( triggered() ), this, SLOT( toggleSideBarVisible() ) );

	// Change View Mode
	QAction *viewModeAct = new QAction( "Change View Mode", this );
	viewModeAct->setShortcuts( Settings->Shortcuts.ViewMode );
	connect( viewModeAct, SIGNAL( triggered() ), this, SLOT( switchToNextView() ) );

	// Show Application View
	QAction *showApplicationsAct = new QAction( "show Applications", this );
	showApplicationsAct->setShortcuts( QList<QKeySequence>() << QKeySequence( "Alt+Shift+A" ) );
	connect( showApplicationsAct, SIGNAL( triggered() ), this, SLOT( showApplications() ) );

	// Show Catalog View
	QAction *showCatalogsAct = new QAction( "show Catalogs", this );
	showCatalogsAct->setShortcuts( QList<QKeySequence>() << QKeySequence( "Alt+Shift+C" ) );
	connect( showCatalogsAct, SIGNAL( triggered() ), this, SLOT( showCatalogs() ) );

	// Show Folder View
	QAction *showFoldersAct = new QAction( "Toggle Catalogs", this );
	showFoldersAct->setShortcuts( QList<QKeySequence>() << QKeySequence( "Alt+Shift+F" ) );
	connect( showFoldersAct, SIGNAL( triggered() ), this, SLOT( showFolders() ) );

	// Focus the search bar
	QAction *focusSearchAct = new QAction( "Focus SearchBar", this );
	focusSearchAct->setShortcuts( Settings->Shortcuts.FocusSearchBar );
	connect( focusSearchAct, SIGNAL( triggered() ), FilterWidget, SLOT( show() ) );

	// Clear the search bar
	QAction *clearSearchAct = new QAction( "Clear SearchBar", this );
	clearSearchAct->setShortcuts( Settings->Shortcuts.ClearSearchBar );
	connect( clearSearchAct, SIGNAL( triggered() ), this, SLOT( clearFilters() ) );

	addAction( focusAddressBarAct );
	addAction( newWindowAct );
	addAction( addBookMarkAct );
	addAction( termWidgetAct );
	addAction( toggleSideBar );
	addAction( viewModeAct );
	addAction( showApplicationsAct );
	addAction( showCatalogsAct );
	addAction( showFoldersAct );
	addAction( focusSearchAct );
	addAction( clearSearchAct );
	addAction( aboutNBAct );
	addAction( aboutQtAct );
	addAction( licenseAct );
	addAction( showInfoAct );
	addAction( showSettingsAct );

	/* Update Widgets */
	updateVarious();
	updateInfoBar();
};

void NewBreeze::openFile( QString file ) {

	NBDebugMsg( DbgMsgPart::HEAD, "Opening file: %s ", file.toLocal8Bit().data() );
	NBAppFile app = NBAppEngine::instance()->xdgDefaultApp( mimeDb.mimeTypeForFile( file ) );

	/* Show Open with command  */
	if ( not app.isValid() ) {

		NBRunCmdDialog *runCmd = new NBRunCmdDialog( QFileInfo( file ).fileName() );
		runCmd->exec();

		if ( runCmd->canRun() ) {
			QStringList exec = runCmd->commandString().split( " " );

			// Prepare @v exec
			if ( exec.contains( "<#NEWBREEZE-ARG-FILES#>" ) ) {
				int idx = exec.indexOf( "<#NEWBREEZE-ARG-FILES#>" );
				exec.removeAt( idx );
				exec.insert( idx, file );
			}

			else if ( exec.contains( "<#NEWBREEZE-ARG-FILE#>" ) ) {
				int idx = exec.indexOf( "<#NEWBREEZE-ARG-FILE#>" );
				exec.removeAt( idx );
				exec.insert( idx, file );
			}

			else
				exec << file;

			NBDebugMsg( DbgMsgPart::TAIL, ( QProcess::startDetached( exec.takeFirst(), exec ) ? "[DONE]" : " [FAILED]" ) );
		}
	}

	/* We have a valid app */
	else {
		QStringList exec = app.execArgs();

		// Prepare @v exec
		if ( app.takesArgs() ) {
			if ( app.multipleArgs() ) {

				int idx = exec.indexOf( "<#NEWBREEZE-ARG-FILES#>" );
				exec.removeAt( idx );
				exec.insert( idx, file );
			}

			else {

				int idx = exec.indexOf( "<#NEWBREEZE-ARG-FILE#>" );
				exec.removeAt( idx );
				exec.insert( idx, file );
			}
		}

		else {

			exec << file;
		}

		NBDebugMsg( DbgMsgPart::TAIL, ( QProcess::startDetached( exec.takeFirst(), exec ) ? "[DONE]" : " [FAILED]" ) );
	}
};

void NewBreeze::show() {

	qDebug() << mTerminate;

	if ( mTerminate )
		return;

	else
		QMainWindow::show();
};

void NewBreeze::showAboutNB() {

	AboutNB *about = new AboutNB( this );
	about->exec();
};

void NewBreeze::showAboutQt() {

	QMessageBox::aboutQt( this, QObject::tr( "About Qt %1" ).arg( QLatin1String( QT_VERSION_STR ) ) );
};

void NewBreeze::showLicense() {

	License *license = new License( this );
	license->exec();
};

void NewBreeze::showInfoDlg() {

	NBDialog *infoDlg = new NBDialog( this );
	infoDlg->setDialogTitle( "NewBreeze Readme" );
	infoDlg->setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );

	QHBoxLayout *dlgLyt = new QHBoxLayout();
	dlgLyt->setContentsMargins( 0, 0, 0, 0 );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	QTextBrowser *nbInfo = new QTextBrowser();

	QFile nbInfoF( ":/README" );
	nbInfoF.open( QFile::ReadOnly );
	nbInfo->setText( QString( nbInfoF.readAll() ) );
	nbInfoF.close();
	nbInfo->setFocusPolicy( Qt::NoFocus );
	nbInfo->setFont( QFont( "Courier 10 Pitch", 9 ) );

	QPushButton *okBtn = new QPushButton( "&Ok" );
	okBtn->setObjectName( "okBtn" );
	connect( okBtn, SIGNAL( clicked() ), infoDlg, SLOT( close() ) );

	btnLyt->addStretch();
	btnLyt->addWidget( okBtn );
	btnLyt->addStretch();

	baseLyt->addWidget( nbInfo );
	baseLyt->addWidget( Separator::horizontal() );
	baseLyt->addLayout( btnLyt );

	QWidget *widget = new QWidget( this );
	widget->setLayout( baseLyt );

	dlgLyt->addWidget( widget );
	infoDlg->setLayout( dlgLyt );

	infoDlg->setFixedSize( QSize( 720, 630 ) );

	infoDlg->setWindowIcon( QIcon( ":/icons/newbreeze.png" ) );
	infoDlg->setWindowTitle( tr( "NewBreeze Info" ) );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 630 ) / 2 );
	infoDlg->move( hpos, vpos );

	infoDlg->exec();
};

void NewBreeze::showSettingsDialog() {

	hide();

	NBSettingsManager *settingsMgr = new NBSettingsManager( this );
	settingsMgr->exec();

	show();
};

void NewBreeze::showCustomActionsDialog() {

	NBCustomActions *customActions = new NBCustomActions();
	customActions->exec();
};

void NewBreeze::newWindow( QString location ) {

	if ( location.isEmpty() and qobject_cast<QAction*>( sender() ) )
		location = FolderView->fsModel->currentDir();

	NewBreeze *newbreeze = new NewBreeze( location );
	if ( Settings->Session.Maximized )
		newbreeze->showMaximized();

	else
		newbreeze->showNormal();
};

void NewBreeze::handleMessages( const QString message ) {

	qDebug() << "Message recieved:" << message;

	if ( message.isEmpty() ) {

		/* User did not say what to do */
		newWindow( QString() );
	}

	else {
		if ( isFile( message ) ) {

			/* This is a file, just open the file */
			openFile( message );
		}

		else if ( isDir( message ) ) {

			/* This is a folder open it */
			newWindow( message );
		}

		else if ( exists( message ) ) {

			/* we do not what this node is */
			qDebug() << "I do not know how to open this:" << message;
			qDebug() << "Trying xdg-open";
			QProcess proc;
			proc.startDetached( "xdg-open", QStringList() << message );
		}

		else {

			/* What the hell is this? */
			qDebug() << "What should I do with:" << message;
		}
	}
};

void NewBreeze::updateVarious( QString url ) {

	/* Navigation Buttons */
	bool backBtn = false, fwdBtn = false;

	/* Enable/disable goBack */
	if ( FolderView->fsModel->canGoBack() )
		backBtn = true;

	/* Enable/disable goForward */
	if ( FolderView->fsModel->canGoForward() )
		fwdBtn = true;

	AddressBar->updateNavigationButtons( backBtn, fwdBtn );

	if ( url.isEmpty() or url.isNull() )
		url = FolderView->fsModel->currentDir();

	setWindowTitle( QString( "%1 - NewBreeze" ).arg( baseName( url ) ) );
	AddressBar->setAddress( url );

	if ( not url.startsWith( "NB://" ) )
		Terminal->changeDir( url );
};

void NewBreeze::updateInfoBar() {

	InfoBar->clear();

	if ( qobject_cast<NBFileSystemModel*>( sender() ) == FolderView->fsModel ) {

		InfoBar->updateInfoBarCF( FolderView->fsModel->currentDir() );
	}

	else {
		QModelIndexList selectedIndexes = FolderView->getSelection();
		if ( selectedIndexes.count() == 0 )
			InfoBar->updateInfoBarCF( FolderView->fsModel->currentDir() );

		else if ( selectedIndexes.count() == 1 )
			InfoBar->updateInfoBarSingle( FolderView->fsModel->nodePath( selectedIndexes.at( 0 ) ) );

		else
			InfoBar->updateInfoBarSelection( FolderView->fsModel->currentDir(), selectedIndexes );
	}
};

void NewBreeze::showProperties() {

	QList<QModelIndex> selectedList = FolderView->getSelection();
	QStringList paths;

	if ( !selectedList.count() )
		paths << FolderView->fsModel->currentDir();

	else
		foreach( QModelIndex idx, selectedList )
			paths << FolderView->fsModel->nodePath( idx );

	NBPropertiesDialog *propsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Properties, &mTerminate, this );
	propsDlg->show();
};

void NewBreeze::showPermissions() {

	QList<QModelIndex> selectedList = FolderView->getSelection();
	QStringList paths;

	if ( !selectedList.count() )
		paths << FolderView->fsModel->currentDir();

	else
		foreach( QModelIndex idx, selectedList )
			paths << FolderView->fsModel->nodePath( idx );

	NBPropertiesDialog *permsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Permissions, &mTerminate, this );
	permsDlg->show();
};

void NewBreeze::handleDriveUrl( QString url ){

	if ( url.startsWith( "NB://A" ) )
		showApplications();

	else if ( url.startsWith( "NB://C" ) )
		showCatalogs();

	else
		FolderView->doOpen( url );
};

void NewBreeze::showApplications() {

	if ( qobject_cast<NBSidePanel*>( sender() ) != SidePanel )
		SidePanel->flashApplications();

	FolderView->doOpen( "NB://Applications" );
	AddressBar->setAddress( "NB://Applications" );
};

void NewBreeze::showCatalogs() {

	if ( qobject_cast<NBSidePanel*>( sender() ) != SidePanel )
		SidePanel->flashCatalogs();

	FolderView->doOpen( "NB://Catalogs" );
	AddressBar->setAddress( "NB://Catalogs" );
};

void NewBreeze::showFolders() {

	if ( qobject_cast<NBSidePanel*>( sender() ) != SidePanel )
		SidePanel->flashFolders();

	FolderView->doOpen( FolderView->fsModel->currentDir() );
};

void NewBreeze::showTrash() {

	NBTrashManager *trashManager = new NBTrashManager();
	trashManager->show();
};

void NewBreeze::filterFiles( QString filter ) {

	FolderView->fsModel->setNameFilters( QStringList() << QString( "*%1*" ).arg( filter ) );
};

void NewBreeze::clearFilters() {

	FilterWidget->clear();
	FolderView->fsModel->setNameFilters( QStringList() );
	FolderView->setFocus();
};

void NewBreeze::initiateIO( QStringList sourceList, QString target, NBIOMode::Mode iomode ) {

	NBProcess::Progress *progress = new NBProcess::Progress;
	progress->sourceDir = dirName( sourceList.at( 0 ) ) + "/";
	progress->targetDir = target;

	QStringList srcList;
	foreach( QString path, sourceList )
		srcList << path.replace( progress->sourceDir, "" );

	if ( iomode == NBIOMode::Move )
		progress->type = NBProcess::Move;

	else
		progress->type = NBProcess::Copy;

	NBIOProcess *proc = new NBIOProcess( srcList, progress );
	NBProcessManager::instance()->addProcess( progress, proc );

	progress->startTime = QTime::currentTime();

	proc->start();
};

void NewBreeze::addBookMark() {

	QStringList order = bookmarkSettings.value( "Order" ).toStringList();

	QString bmkPath = FolderView->fsModel->currentDir();
	if ( bmkPath.endsWith( "/" ) )
		bmkPath.chop( 1 );

	QString label = QFileInfo( bmkPath ).isRoot() ? "FileSystem" : baseName( bmkPath );

	order << bmkPath;
	order.removeDuplicates();

	bookmarkSettings.setValue( QUrl::toPercentEncoding( bmkPath ), label );
	bookmarkSettings.setValue( "Order", order );
	bookmarkSettings.sync();

	SidePanel->flashBookmarks();
};

void NewBreeze::openAddressBar() {

	if ( !QFileInfo( AddressBar->address() ).exists() ) {
		QString text = QString( "There is no file or directory named: "		\
			"<tt><b>%1</b></tt>. Please check the path entered."
		).arg(  AddressBar->address() );

		QString title = QString( "NewBreeze - Invalid Location" );

		NBMessageDialog::error( this, title, text );

		return;
	}

	FolderView->doOpen( AddressBar->address() );
};

void NewBreeze::openWithList() {

	if ( not FolderView->hasSelection() )
		return;

	NBOpenWithMenu *openWithMenu = new NBOpenWithMenu( ":/icons/openWith.png", "&Open With", FolderView );
	openWithMenu->setWorkingDirectory( FolderView->fsModel->currentDir() );
	openWithMenu->buildMenu( FolderView->getSelection() );

	QAbstractItemView *itemView = qobject_cast<QAbstractItemView*>( FolderView->currentWidget() );

	QRect rect = itemView->visualRect( FolderView->getSelection().at( 0 ) );
	QPoint pos( rect.x() + rect.width() / 2, rect.y() + rect.height() / 2 );

	openWithMenu->exec( itemView->mapToGlobal( pos ) );
};

void NewBreeze::changeViewMode( int mode ) {

	QStringList viewModes = QStringList() << "TilesView" << "IconsView" << "DetailsView";
	Settings->General.ViewMode = viewModes.at( mode );

	QSettings sett( FolderView->fsModel->nodePath( ".directory" ), QSettings::NativeFormat );
	sett.setValue( "NewBreeze/ViewMode", Settings->General.ViewMode );

	FolderView->updateViewMode();
};

void NewBreeze::switchToNextView() {

	if ( Settings->General.ViewMode == QString( "TilesView" ) )
		Settings->General.ViewMode = QString( "IconsView" );

	else if ( Settings->General.ViewMode == QString( "IconsView" ) )
		Settings->General.ViewMode = QString( "DetailsView" );

	else
		Settings->General.ViewMode = QString( "TilesView" );

	FolderView->updateViewMode();

	QSettings sett( FolderView->fsModel->nodePath( ".directory" ), QSettings::NativeFormat );
	sett.setValue( "NewBreeze/ViewMode", Settings->General.ViewMode );
};

void NewBreeze::toggleGrouping() {

	if ( Settings->General.Grouping )
		Settings->General.Grouping = false;

	else
		Settings->General.Grouping = true;

	FolderView->fsModel->setCategorizationEnabled( Settings->General.Grouping );
	FolderView->groupsAct->setChecked( Settings->General.Grouping );

	QSettings sett( FolderView->fsModel->nodePath( ".directory" ), QSettings::NativeFormat );
	sett.setValue( "NewBreeze/Grouping", Settings->General.Grouping );
};

void NewBreeze::toggleSideBarVisible() {

	if ( SidePanel->isVisible() )
		SidePanel->hide();

	else
		SidePanel->show();

	Settings->setValue( "Session/SidePanel", SidePanel->isVisible() );
};

void NewBreeze::showHideTermWidget() {

	if ( Terminal->isVisible() ) {
		Terminal->hide();
		FolderView->setFocus();
	}

	else {
		Terminal->show();
		// Terminal->setFocus();
		return;
	}
};

void NewBreeze::closeEvent( QCloseEvent *cEvent ) {

	/* If we have a UI */
	// Close down Info Gathering
	FolderView->fsModel->terminateInfoGathering();

	// Close down recursive size checker in Properties
	mTerminate = true;

	// If there are background processes, bring them to front
	if ( NBProcessManager::instance()->activeProcessCount() )
		NBProcessManagerUI::instance()->show();

	// Store the previous session - geometry, and open directory.
	Settings->setValue( "Session/Geometry", geometry() );
	Settings->setValue( "Session/LastDir", FolderView->fsModel->currentDir() );
	Settings->setValue( "Session/Maximized", isMaximized() );

	// Come to home dir
	chdir( NBXdg::home().toLocal8Bit().constData() );
	Terminal->changeDir( NBXdg::home() );

	// Now hide this window, other processes may take a while longer to close down
	QMainWindow::close();

	cEvent->accept();
	qDebug( "Good Bye!" );
};

void NewBreeze::paintEvent( QPaintEvent *pEvent ) {

	QMainWindow::paintEvent( pEvent );

	QPainter *painter = new QPainter( this );

	painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 1.0 ) );
	painter->drawLine( rect().topLeft(), rect().topRight() );

	painter->end();

	pEvent->accept();
};
