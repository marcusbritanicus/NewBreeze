/*
	*
	* NewBreeze.cpp - NewBreeze MainWindow Class
	*
*/

#include "NewBreeze.hpp"

NewBreeze::NewBreeze( QString loc ) : QMainWindow() {

	/* If we have root privileges */
	if ( not getuid() ) {
		NBMessageDialog::critical(
			this,
			"NewBreeze - Attention!!",
			"You are running NewBreeze as superuser privileges. This is generally a bad idea and it is advised to perform actions as root, on a teminal or a tty. "
			"To minimize the potential damage that can be caused, NewBreeze will be started in safe mode with all the plugins disabled."
		);
	}

	/* No terminate signal until close */
	mTerminate = false;

	/* Closed flag */
	mClosed = false;

	/* Term update flag */
	mUpdateTerminal = true;

	/* Create the UI */
	createGUI();

	/* Make sure we always have absolute paths */
	if ( loc.count() and not loc.startsWith( "NB://" ) )
		loc = QFileInfo( loc ).absoluteFilePath();

	/* Open with SuperStart */
	if ( Settings->value( "SuperStart" ) and loc.isEmpty() )
		FolderView->doOpen( "NB://SuperStart" );

	/* Load the a folder */
	else if ( not loc.isEmpty() ) {
		if ( loc.startsWith( "NB://SuperStart" ) )
			FolderView->doOpen( "NB://SuperStart" );

		else if ( loc.startsWith( "NB://Catalogs" ) )
			FolderView->doOpen( loc );

		else if ( isFile( loc ) ) {

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
		if ( exists( Settings->value( "Session/LastDir", NBSettings::GlobalScope ) ) ) {
			FolderView->doOpen( QString( Settings->value( "Session/LastDir", NBSettings::GlobalScope ) ) );
		}

		else {
			FolderView->doOpen( QString( QDir::homePath() ) );
		}
	}

	/* Focus the CurrentWidget in FolderView */
	FolderView->setFocus();

	/* Window Properties */
	setWindowProperties();

	/* Signal-Slot Connections */
	createAndSetupActions();
};

bool NewBreeze::isClosed() {

	return mClosed;
};

void NewBreeze::createGUI() {

	QWidget *BaseWidget = new QWidget( this );
	BaseWidget->setObjectName( "base" );

	AddressBar = new NBAddressBar( this );
	setupSidePanel();
	FolderView = new NBFolderView( this );
	Terminal = new NBTerminalWidget( QString(), this );
	InfoBar = new NBInfoBar( this );
	setupInfoPanel();
	FilterWidget = new NBFilterWidget( FolderView );

	QHBoxLayout *ViewLayout = new QHBoxLayout();
	ViewLayout->setContentsMargins( QMargins() );
	ViewLayout->setSpacing( 0 );

	ViewLayout->addWidget( SidePanel );
	ViewLayout->addWidget( SideBar );

	ViewLayout->addWidget( FolderView );

	QVBoxLayout *BaseLayout = new QVBoxLayout();
	BaseLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
	BaseLayout->setSpacing( 0 );

	BaseLayout->addWidget( AddressBar );
	BaseLayout->addLayout( ViewLayout );
	BaseLayout->addWidget( Terminal );
	BaseLayout->addWidget( InfoBar );

	BaseWidget->setLayout( BaseLayout );
	setCentralWidget( BaseWidget );

	// Widget Properties
	AddressBar->setFixedHeight( 32 );
	Terminal->setFixedHeight( 270 );
	Terminal->hide();
	FilterWidget->hide();

	// Transparency
	setStyleSheet( "QAbstractItemView{ background-color: transparent; }" );

	AddressBar->setFocusPolicy( Qt::NoFocus );
	SidePanel->setFocusPolicy( Qt::NoFocus );
	FolderView->setFocusPolicy( Qt::StrongFocus );

	setFocusPolicy( Qt::NoFocus );
};

void NewBreeze::setupSidePanel() {

	SidePanel = new NBSidePanel( this );
	SidePanel->hide();
	connect( SidePanel, SIGNAL( driveClicked( QString ) ), this, SLOT( handleDriveUrl( QString ) ) );
	connect( SidePanel, SIGNAL( showTrash() ), this, SLOT( showTrash() ) );

	SideBar = new NBSideBar( this );
	SideBar->hide();
	connect( SideBar, SIGNAL( driveClicked( QString ) ), this, SLOT( handleDriveUrl( QString ) ) );
	connect( SideBar, SIGNAL( showTrash() ), this, SLOT( showTrash() ) );

	/* Showing Classic SidePanel */
	if ( Settings->value( "SidePanel", NBSettings::GlobalScope ) ) {
		if ( Settings->value( "SidePanelType", NBSettings::GlobalScope ) )
			SidePanel->show();

		/* Showing Modern SidePanel */
		else
			SideBar->show();
	}
};

void NewBreeze::setupInfoPanel() {

	/* Create the InfoPanel */
	InfoPanel = new NBInfoPanel( this );

	InfoDock = new QDockWidget( "", this );
	InfoDock->setTitleBarWidget( new QWidget() );
	InfoDock->setFeatures( QDockWidget::NoDockWidgetFeatures );
	InfoDock->setWidget( InfoPanel );

	addDockWidget( Qt::RightDockWidgetArea, InfoDock );

	/* Show infopanel and hide infobar */
	if ( Settings->value( "InfoPanel", NBSettings::GlobalScope ) ) {
		InfoDock->show();
		InfoBar->hide();
	}

	/* Show infobar and hide infopanel */
	else {
		InfoDock->hide();
		InfoBar->show();
	}

	updateInfoBar();
};

void NewBreeze::setWindowProperties() {

	setWindowTitle( QString( "%1 - NewBreeze" ).arg( baseName( FolderView->fsModel->currentDir() ) ) );
	setWindowIcon( QIcon( ":/icons/newbreeze.png" ) );

	setMinimumHeight( 600 );

	if ( not Settings->value( "Session/Maximized", NBSettings::GlobalScope ) )
		setGeometry( Settings->value( "Session/Geometry", NBSettings::GlobalScope ) );
};

void NewBreeze::createAndSetupActions() {

	connect( AddressBar, SIGNAL( openLocation( QString ) ), FolderView, SLOT( doOpen( QString ) ) );
	connect( AddressBar, SIGNAL( openSearch() ), FilterWidget, SLOT( showHide() ) );

	connect( AddressBar, SIGNAL( goHome() ), FolderView, SLOT( loadHomeDir() ) );
	connect( AddressBar, SIGNAL( openSuperStart() ), FolderView, SLOT( doOpenSS() ) );

	connect( AddressBar, SIGNAL( newWindow() ), this, SLOT( newWindow() ) );

	connect( AddressBar, SIGNAL( zoomIn() ), FolderView->IconView, SLOT( zoomIn() ) );
	connect( AddressBar, SIGNAL( zoomOut() ), FolderView->IconView, SLOT( zoomOut() ) );

	connect( AddressBar, SIGNAL( cut() ), FolderView, SLOT( prepareMove() ) );
	connect( AddressBar, SIGNAL( copy() ), FolderView, SLOT( prepareCopy() ) );
	connect( AddressBar, SIGNAL( paste() ), FolderView, SLOT( prepareIO() ) );

	connect( AddressBar, SIGNAL( openVTE() ), FolderView, SLOT( openTerminal() ) );

	connect( AddressBar, SIGNAL( changeViewMode( int ) ), this, SLOT( changeViewMode( int ) ) );

	connect( AddressBar, SIGNAL( sortByName() ), FolderView, SLOT( sortByName() ) );
	connect( AddressBar, SIGNAL( sortByType() ), FolderView, SLOT( sortByType() ) );
	connect( AddressBar, SIGNAL( sortBySize() ), FolderView, SLOT( sortBySize() ) );
	connect( AddressBar, SIGNAL( sortByDate() ), FolderView, SLOT( sortByDate() ) );
	connect( AddressBar, SIGNAL( toggleHidden() ), FolderView, SLOT( doToggleHidden() ) );
	connect( AddressBar, SIGNAL( toggleGrouping() ), this, SLOT( toggleGrouping() ) );

	connect( AddressBar, SIGNAL( showSettings() ), this, SLOT( showSettingsDialog() ) );

	connect( AddressBar, SIGNAL( closeWindow() ), this, SLOT( close() ) );
	connect( AddressBar, SIGNAL( quit() ), this, SLOT( quit() ) );

	connect( FilterWidget, SIGNAL( search( QString ) ), this, SLOT( filterFiles( QString ) ) );
	connect( FilterWidget, SIGNAL( hidden() ), this, SLOT( clearFilters() ) );

	connect( FolderView, SIGNAL( showProperties() ), this, SLOT( showProperties() ) );
	connect( FolderView, SIGNAL( showPermissions() ), this, SLOT( showPermissions() ) );

	connect( FolderView, SIGNAL( newWindow( QString ) ), this, SLOT( newWindow( QString ) ) );
	connect( FolderView, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ), this, SLOT( updateInfoBar() ) );
	connect( FolderView, SIGNAL( toggleGroups() ), this, SLOT( toggleGrouping() ) );

	connect( FolderView, SIGNAL( hideStatusBar() ), InfoBar, SLOT( hide() ) );
	connect( FolderView, SIGNAL( showStatusBar() ), InfoBar, SLOT( show() ) );

	connect( FolderView, SIGNAL( reloadSuperStart() ), SidePanel, SLOT( populateSidePanel() ) );
	connect( FolderView, SIGNAL( reloadBookmarks() ), SidePanel, SLOT( populateSidePanel() ) );
	connect( FolderView, SIGNAL( reloadCatalogs() ), SidePanel, SLOT( populateSidePanel() ) );

	connect( FolderView, SIGNAL( reloadSuperStart() ), SideBar, SLOT( updateSideBar() ) );
	connect( FolderView, SIGNAL( reloadBookmarks() ), SideBar, SLOT( updateSideBar() ) );
	connect( FolderView, SIGNAL( reloadCatalogs() ), SideBar, SLOT( updateSideBar() ) );

	connect( FolderView, SIGNAL( reloadCatalogs() ), SideBar, SLOT( updateSideBar() ) );

	connect( FolderView->fsModel, SIGNAL( directoryLoading( QString ) ), this, SLOT( updateVarious( QString ) ) );
	connect( FolderView->fsModel, SIGNAL( directoryLoading( QString ) ), this, SLOT( updateInfoBar() ) );

	connect( FolderView->fsModel, SIGNAL( directoryLoading( QString ) ), SidePanel, SLOT( highlight( QString ) ) );

	connect( FolderView->fsModel, SIGNAL( directoryLoading( QString ) ), this, SLOT( setBusyCursor() ) );
	connect( FolderView->fsModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( setNormalCursor() ) );
	connect( FolderView->fsModel, SIGNAL( directoryLoaded( QString ) ), FolderView, SLOT( updateActions() ) );

	/* Update GUI if terminal cwd changes */
	connect( Terminal, SIGNAL( chdir( QString ) ), this, SLOT( chdirUI( QString ) ) );

	// About NB
	QAction *aboutNBAct = new QAction( this );
	aboutNBAct->setShortcuts( Settings->shortcuts( "AboutNB" ) );
	connect( aboutNBAct, SIGNAL( triggered() ), this, SLOT( showAboutNB() ) );

	// About Qt
	QAction *aboutQtAct = new QAction( this );
	aboutQtAct->setShortcuts( Settings->shortcuts( "AboutQt" ) );
	connect( aboutQtAct, SIGNAL( triggered() ), this, SLOT( showAboutQt() ) );

	// About Qt
	QAction *licenseAct = new QAction( this );
	licenseAct->setShortcut( tr( "Ctrl+Shift+L" ) );
	connect( licenseAct, SIGNAL( triggered() ), this, SLOT( showLicense() ) );

	// NB Info
	QAction *showInfoAct = new QAction( this );
	showInfoAct->setShortcuts( Settings->shortcuts( "NBInfo" ) );
	connect( showInfoAct, SIGNAL( triggered() ), this, SLOT( showInfoDlg() ) );

	// Show settings
	QAction *showSettingsAct = new QAction( this );
	showSettingsAct->setShortcuts( Settings->shortcuts( "Settings" ) );
	connect( showSettingsAct, SIGNAL( triggered() ), this, SLOT( showSettingsDialog() ) );

	// Show settings
	QAction *fileBugReportAct = new QAction( this );
	fileBugReportAct->setShortcuts( QList<QKeySequence>() << tr( "Ctrl+Shift+B" ) );
	connect( fileBugReportAct, SIGNAL( triggered() ), this, SLOT( fileBugReport() ) );

	// Focus AddressBar
	QAction *focusAddressBarAct = new QAction( this );
	focusAddressBarAct->setShortcuts( Settings->shortcuts( "FocusAddressBar" ) );
	connect( focusAddressBarAct, SIGNAL( triggered() ), AddressBar, SLOT( focusAddressEdit() ) );

	// Open new window
	QAction *newWindowAct = new QAction( this );
	newWindowAct->setShortcuts( Settings->shortcuts( "NewWindow" ) );
	connect( newWindowAct, SIGNAL( triggered() ), this, SLOT( newWindow() ) );

	// Display terminal widget
	QAction *termWidgetAct = new QAction( this );
	termWidgetAct->setShortcuts( Settings->shortcuts( "InlineTerminal" ) );
	connect( termWidgetAct, SIGNAL( triggered() ), this, SLOT( showHideTermWidget() ) );

	// Show/Hide side bar
	QAction *toggleSidePanel = new QAction( "Toggle SidePanel", this );
	toggleSidePanel->setShortcuts( Settings->shortcuts( "ToggleSideBar" ) );
	connect( toggleSidePanel, SIGNAL( triggered() ), this, SLOT( toggleSidePanelVisible() ) );

	// Show/Hide info panel
	QAction *toggleInfoPanelAct = new QAction( "Toggle InfoPanel", this );
	toggleInfoPanelAct->setShortcut( QKeySequence( tr( "F7" ) ) );
	connect( toggleInfoPanelAct, SIGNAL( triggered() ), this, SLOT( toggleInfoPanel() ) );

	// Change View Mode
	QAction *viewModeAct = new QAction( "Change View Mode", this );
	viewModeAct->setShortcuts( Settings->shortcuts( "ViewMode" ) );
	connect( viewModeAct, SIGNAL( triggered() ), this, SLOT( switchToNextView() ) );

	// Show Catalog View
	QAction *showCatalogsAct = new QAction( "Show Catalogs", this );
	showCatalogsAct->setShortcuts( QList<QKeySequence>() << QKeySequence( "Alt+Shift+C" ) );
	connect( showCatalogsAct, SIGNAL( triggered() ), this, SLOT( showCatalogs() ) );

	// Focus the search bar
	QAction *focusSearchAct = new QAction( "Focus SearchBar", this );
	focusSearchAct->setShortcuts( Settings->shortcuts( "FocusSearchBar" ) );
	connect( focusSearchAct, SIGNAL( triggered() ), FilterWidget, SLOT( show() ) );

	// Clear the search bar
	QAction *clearSearchAct = new QAction( "Clear SearchBar", this );
	clearSearchAct->setShortcuts( Settings->shortcuts( "ClearSearchBar" ) );
	connect( clearSearchAct, SIGNAL( triggered() ), this, SLOT( clearFilters() ) );

	// Quit NewBreeze
	QAction *quitNBAct = new QAction( "Quit NewBreeze", this );
	quitNBAct->setShortcuts( Settings->shortcuts( "QuitNewBreeze" ) );
	connect( quitNBAct, SIGNAL( triggered() ), this, SLOT( quit() ) );

	addAction( focusAddressBarAct );
	addAction( newWindowAct );
	addAction( termWidgetAct );
	addAction( toggleSidePanel );
	addAction( fileBugReportAct );
	addAction( toggleInfoPanelAct );
	addAction( viewModeAct );
	addAction( showCatalogsAct );
	addAction( focusSearchAct );
	addAction( clearSearchAct );
	addAction( aboutNBAct );
	addAction( aboutQtAct );
	addAction( licenseAct );
	addAction( showInfoAct );
	addAction( showSettingsAct );
	addAction( quitNBAct );

	/* Update Widgets */
	updateVarious();
	updateInfoBar();
};

void NewBreeze::openFile( QString file ) {

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

			qDebug( "Opening file: %s [%s]", file.toLocal8Bit().data(), ( QProcess::startDetached( exec.takeFirst(), exec ) ? "DONE" : " FAILED" ) );
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

		qDebug( "Opening file: %s [%s]", file.toLocal8Bit().data(), ( QProcess::startDetached( exec.takeFirst(), exec ) ? "DONE" : " FAILED" ) );
	}
};

void NewBreeze::show() {

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

	NBSettingsManager *settingsMgr = new NBSettingsManager( this );
	settingsMgr->exec();

	SidePanel->hide();
	SideBar->hide();

	if ( Settings->value( "SidePanel" ) ) {
		if ( Settings->value( "SidePanelType" ) )
			SidePanel->show();

		else
			SideBar->show();
	}

	NBPluginManager::instance()->reloadPlugins();

	qApp->processEvents();
};

void NewBreeze::showCustomActionsDialog() {

	NBCustomActions *customActions = new NBCustomActions();
	customActions->exec();
};

void NewBreeze::fileBugReport() {

	qDebug() << "Showing NewBreeze BugReporter";
	NBBugReporter *bugreport = new NBBugReporter();
	bugreport->exec();
};

void NewBreeze::newWindow( QString location ) {

	if ( location.isEmpty() and qobject_cast<QAction*>( sender() ) ) {
		location = FolderView->fsModel->currentDir();
	}

	NewBreeze *newbreeze = new NewBreeze( location );
	if ( Settings->value( "Session/Maximized", NBSettings::GlobalScope ) )
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

	if ( url.isEmpty() or url.isNull() )
		url = FolderView->fsModel->currentDir();

	setWindowTitle( QString( "%1 - NewBreeze" ).arg( baseName( url ) ) );
	AddressBar->setAddress( url );

	if ( mUpdateTerminal ) {
		if ( url.startsWith( "NB://" ) )
			Terminal->changeDir( NBXdg::home() );

		else
			Terminal->changeDir( url );
	}
};

void NewBreeze::updateInfoBar() {

	InfoBar->clear();

	if ( qobject_cast<NBItemViewModel*>( sender() ) == FolderView->fsModel ) {

		InfoBar->updateInfoBarCF( FolderView->fsModel->currentDir() );
		InfoPanel->updatePanel( FolderView->fsModel->currentDir(), QModelIndexList() );
	}

	else {
		QModelIndexList selectedIndexes = FolderView->getSelection();
		if ( selectedIndexes.count() == 0 )
			InfoBar->updateInfoBarCF( FolderView->fsModel->currentDir() );

		else if ( selectedIndexes.count() == 1 )
			InfoBar->updateInfoBarSingle( FolderView->fsModel->nodePath( selectedIndexes.at( 0 ) ) );

		else
			InfoBar->updateInfoBarSelection( FolderView->fsModel->currentDir(), selectedIndexes );

		InfoPanel->updatePanel( FolderView->fsModel->currentDir(), selectedIndexes );
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

	if ( FolderView->fsModel->modelDataType() == NBItemViewModel::Applications ) {
		NBDesktopFileEditor *desktopEditor = new NBDesktopFileEditor( this, paths.at( 0 ) );
		desktopEditor->exec();
	}

	else {
		NBPropertiesDialog *propsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Properties, this );
		propsDlg->show();
	}
};

void NewBreeze::showPermissions() {

	QList<QModelIndex> selectedList = FolderView->getSelection();
	QStringList paths;

	if ( !selectedList.count() )
		paths << FolderView->fsModel->currentDir();

	else
		foreach( QModelIndex idx, selectedList )
			paths << FolderView->fsModel->nodePath( idx );

	NBPropertiesDialog *permsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Permissions, this );
	permsDlg->show();
};

void NewBreeze::handleDriveUrl( QString url ){

	if ( url.startsWith( "NB://SuperStart" ) )
		FolderView->fsModel->setRootPath( url );

	if ( url.startsWith( "NB://Applications" ) )
		showApplications();

	else if ( url.startsWith( "NB://Catalogs" ) )
		showCatalogs( url );

	else
		FolderView->doOpen( url );
};

void NewBreeze::chdirUI( QString url ){

	mUpdateTerminal = false;

	/* Update the UI to show the cwd of the shell */
	FolderView->doOpen( url );

	/* Keep the focus on Terminal widget */
	Terminal->setFocus();
	mUpdateTerminal = true;
};

void NewBreeze::showApplications() {

	FolderView->doOpen( "NB://Applications" );
	AddressBar->setAddress( "NB://Applications" );
};

void NewBreeze::showCatalogs( QString url ) {

	FolderView->doOpen( url );
	AddressBar->setAddress( url );
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

	QAbstractItemView *itemView = qobject_cast<QAbstractItemView*>( FolderView->IconView );

	QRect rect = itemView->visualRect( FolderView->getSelection().at( 0 ) );
	QPoint pos( rect.x() + rect.width() / 2, rect.y() + rect.height() / 2 );

	openWithMenu->exec( itemView->mapToGlobal( pos ) );
};

void NewBreeze::changeViewMode( int mode ) {

	QStringList viewModes = QStringList() << "Tiles" << "Icons" << "Details";

	/* For anything starting with NB:// */
	if ( not FolderView->fsModel->isRealLocation() ) {
		QString location = FolderView->fsModel->currentDir().replace( "NB://", "" );
		if ( location == "SuperStart" )
			Settings->setValue( "View/ViewMode", viewModes.at( mode ), NBSettings::SuperStart );

		else
			Settings->setValue( "View/ViewMode", viewModes.at( mode ), NBSettings::Catalogs );
	}

	else
		Settings->setValue( "View/ViewMode", viewModes.at( mode ) );

	FolderView->updateViewMode();
};

void NewBreeze::switchToNextView() {

	QStringList viewModes = QStringList() << "Tiles" << "Icons" << "Details";

	QString newMode;
	if ( FolderView->IconView->viewMode() == QString( "Tiles" ) )
		newMode = QString( "Icons" );

	else if ( FolderView->IconView->viewMode() == QString( "Icons" ) )
		newMode = QString( "Details" );

	else
		newMode = QString( "Tiles" );

	if ( not FolderView->fsModel->isRealLocation() ) {
		QString location = FolderView->fsModel->currentDir().replace( "NB://", "" );
		if ( location == "SuperStart" )
			Settings->setValue( "View/ViewMode", newMode, NBSettings::SuperStart );

		else
			Settings->setValue( "View/ViewMode", newMode, NBSettings::Catalogs );
	}

	else {
		Settings->setValue( "View/ViewMode", newMode );
	}

	FolderView->updateViewMode();
};

void NewBreeze::toggleGrouping() {

	FolderView->fsModel->setCategorizationEnabled( Settings->value( "Grouping" ) );
	FolderView->groupsAct->setChecked( Settings->value( "Grouping" ) );

	if ( not FolderView->fsModel->isRealLocation() ) {
		QString location = FolderView->fsModel->currentDir().replace( "NB://", "" );
		if ( location == "SuperStart" )
			Settings->setValue( "View/Grouping", not Settings->value( "Grouping" ), NBSettings::SuperStart );

		else
			Settings->setValue( "View/Grouping", not Settings->value( "Grouping" ), NBSettings::Catalogs );
	}

	else {
		Settings->setValue( "View/Grouping", not Settings->value( "Grouping" ) );
	}

	FolderView->fsModel->reload();
};

void NewBreeze::toggleSidePanelVisible() {

	if ( SidePanel->isVisible() )
		SidePanel->hide();

	else
		SidePanel->show();

	Settings->setValue( "SidePanel", SidePanel->isVisible() );
};

void NewBreeze::toggleInfoPanel() {

	if ( InfoPanel->isVisible() ) {
		InfoDock->hide();
		InfoBar->show();

		Settings->setValue( "InfoPanel", false );
	}

	else {
		InfoDock->show();
		InfoBar->hide();

		Settings->setValue( "InfoPanel", true );
	}
};

void NewBreeze::showHideTermWidget() {

	if ( Terminal->isVisible() ) {
		Terminal->hide();
		FolderView->setFocus();
	}

	else {
		Terminal->show();
		Terminal->setFocus();
		return;
	}
};

void NewBreeze::setBusyCursor() {

	setCursor( Qt::WaitCursor );
};

void NewBreeze::setNormalCursor() {

	setCursor( Qt::ArrowCursor );
};

void NewBreeze::quit() {

	/* Close all top level widgets */
	Q_FOREACH( QWidget *w, qApp->topLevelWidgets() )
		w->close();

	// fclose( nblog );

	// If there are background IO processes, bring them to front
	if ( NBProcessManager::instance()->activeProcessCount() ) {
		NBProcessManagerUI::instance()->show();
		qApp->setQuitOnLastWindowClosed( true );

		Settings->ClosingDown = true;
	}

	else
		qApp->quit();
};

void NewBreeze::closeEvent( QCloseEvent *cEvent ) {

	// We hide this window immediately, other processes may take a while longer to close down
	QMainWindow::close();

	// Close down recursive size checker in Properties
	mTerminate = true;

	// If there are background processes, bring them to front
	if ( NBProcessManager::instance()->activeProcessCount() )
		NBProcessManagerUI::instance()->show();

	// Store the previous session - geometry, and open directory.
	Settings->setValue( "Session/Geometry", geometry(), NBSettings::GlobalScope );
	Settings->setValue( "Session/Maximized", isMaximized(), NBSettings::GlobalScope );

	Settings->setValue( "Session/LastDir", FolderView->fsModel->lastOpenedFolder(), NBSettings::GlobalScope );

	// Come to home dir
	chdir( NBXdg::home().toLocal8Bit().constData() );
	Terminal->changeDir( NBXdg::home() );

	cEvent->accept();

	mClosed = true;
	qDebug() << "Good Bye!";
};
