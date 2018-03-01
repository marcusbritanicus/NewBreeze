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

	/* Create the UI */
	createGUI();

	/* Make sure we always have absolute paths */
	if ( loc.count() )
		loc = QFileInfo( loc ).absoluteFilePath();

	/* Open with SuperStart */
	if ( Settings->General.SpecialOpen and Settings->General.SuperStart and loc.isEmpty() )
		FolderView->doOpen( "NB://SuperStart" );

	/* Open with Catalogs */
	else if ( Settings->General.SpecialOpen and Settings->General.OpenWithCatalog and loc.isEmpty() )
		FolderView->doOpen( "NB://Catalogs" );

	/* Load the a folder */
	else if ( not loc.isEmpty() ) {
		if ( loc.startsWith( "NB://SuperStart" ) )
			FolderView->doOpen( "NB://SuperStart" );

		else if ( loc.startsWith( "NB://Applications" ) )
			FolderView->doOpen( "NB://Applications" );

		else if ( loc.startsWith( "NB://Catalogs" ) )
			FolderView->doOpen( "NB://Catalogs" );

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
		if ( exists( Settings->Session.LastDir ) ) {
			FolderView->doOpen( QString( Settings->Session.LastDir ) );
		}

		else {
			FolderView->doOpen( QString( QDir::homePath() ) );
		}
	}

	/* Focus the CurrentWidget in FolderView */
	FolderView->currentWidget()->setFocus();

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
	Terminal = new NBTerminal( QString(), this );
	InfoBar = new NBInfoBar( this );
	setupInfoPanel();
	FilterWidget = new NBFilterWidget( FolderView );

	QWidget *Spacer = new QWidget();
	Spacer->setFixedHeight( 3 );
	Spacer->setStyleSheet( "border-bottom: 1px solid darkgray;" );

	QHBoxLayout *ViewLayout = new QHBoxLayout();
	ViewLayout->setContentsMargins( QMargins() );
	ViewLayout->setSpacing( 0 );

	/* One of the SidePanel's will be hidden automatically */
	ViewLayout->addWidget( SidePanel );
	ViewLayout->addWidget( SideBar );

	ViewLayout->addWidget( FolderView );
	ViewLayout->addWidget( InfoPanel );

	QVBoxLayout *BaseLayout = new QVBoxLayout();
	BaseLayout->setContentsMargins( QMargins( 0, 3, 0, 0 ) );
	BaseLayout->setSpacing( 0 );

	BaseLayout->addWidget( AddressBar );
	BaseLayout->addWidget( Spacer );
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

	AddressBar->setFocusPolicy( Qt::NoFocus );
	SidePanel->setFocusPolicy( Qt::NoFocus );
	SideBar->setFocusPolicy( Qt::NoFocus );
	FolderView->setFocusPolicy( Qt::StrongFocus );

	setFocusPolicy( Qt::NoFocus );
};

void NewBreeze::setupSidePanel() {

	SidePanel = new NBSidePanel( this );
	SideBar = new NBSideBar( this );

	connect( SidePanel, SIGNAL( driveClicked( QString ) ), this, SLOT( handleDriveUrl( QString ) ) );
	connect( SidePanel, SIGNAL( showFolders() ), this, SLOT( showFolders() ) );
	connect( SidePanel, SIGNAL( showApplications() ), this, SLOT( showApplications() ) );
	connect( SidePanel, SIGNAL( showCatalogs() ), this, SLOT( showCatalogs() ) );
	connect( SidePanel, SIGNAL( showTrash() ), this, SLOT( showTrash() ) );

	connect( SideBar, SIGNAL( driveClicked( QString ) ), this, SLOT( handleDriveUrl( QString ) ) );
	connect( SideBar, SIGNAL( showTrash() ), this, SLOT( showTrash() ) );

	/* If we are showing sidepanel */
	if ( Settings->General.SidePanel ) {
		/* Modern SideBar */
		if ( Settings->General.SidePanelType == 0 ) {
			SidePanel->show();
			SideBar->hide();
		}
		/* Classic SidePanel */
		else {
			SidePanel->hide();
			SideBar->show();
		}
	}

	/* We are not showing the sidepanel at all */
	else {
		SideBar->hide();
		SidePanel->hide();
	}
};

void NewBreeze::setupInfoPanel() {

	/* Create the InfoPanel */
	InfoPanel = new NBInfoPanel( this );

	/* Show infopanel and hide infobar */
	if ( Settings->General.InfoPanel ) {
		InfoPanel->show();
		InfoBar->hide();
	}

	/* Show infobar and hide infopanel */
	else {
		InfoPanel->hide();
		InfoBar->show();
	}

	updateInfoBar();
};

void NewBreeze::setWindowProperties() {

	setWindowTitle( QString( "%1 - NewBreeze" ).arg( baseName( FolderView->fsModel->currentDir() ) ) );
	setWindowIcon( QIcon( ":/icons/newbreeze.png" ) );

	setMinimumSize( 900, 600 );

	if ( not Settings->Session.Maximized )
		setGeometry( Settings->Session.Geometry );
};

void NewBreeze::createAndSetupActions() {

	connect( AddressBar, SIGNAL( changeViewMode( int ) ), this, SLOT( changeViewMode( int ) ) );
	connect( AddressBar, SIGNAL( openLocation( QString ) ), FolderView, SLOT( doOpen( QString ) ) );
	connect( AddressBar, SIGNAL( openSearch() ), FilterWidget, SLOT( show() ) );

	connect( AddressBar, SIGNAL( goBack() ), FolderView, SLOT( goBack() ) );
	connect( AddressBar, SIGNAL( goHome() ), FolderView, SLOT( loadHomeDir() ) );
	connect( AddressBar, SIGNAL( openSuperStart() ), FolderView, SLOT( doOpenSS() ) );
	connect( AddressBar, SIGNAL( goForward() ), FolderView, SLOT( goForward() ) );

	connect( FilterWidget, SIGNAL( search( QString ) ), this, SLOT( filterFiles( QString ) ) );
	connect( FilterWidget, SIGNAL( shown() ), AddressBar, SLOT( hideSearchButton() ) );
	connect( FilterWidget, SIGNAL( hidden() ), AddressBar, SLOT( showSearchButton() ) );
	connect( FilterWidget, SIGNAL( hidden() ), this, SLOT( clearFilters() ) );

	connect( FolderView, SIGNAL( showProperties() ), this, SLOT( showProperties() ) );
	connect( FolderView, SIGNAL( showPermissions() ), this, SLOT( showPermissions() ) );

	connect( FolderView, SIGNAL( newWindow( QString ) ), this, SLOT( newWindow( QString ) ) );
	connect( FolderView, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ), this, SLOT( updateInfoBar() ) );
	connect( FolderView, SIGNAL( toggleGroups() ), this, SLOT( toggleGrouping() ) );

	connect( FolderView, SIGNAL( hideStatusBar() ), InfoBar, SLOT( hide() ) );
	connect( FolderView, SIGNAL( showStatusBar() ), InfoBar, SLOT( show() ) );

	connect( FolderView, SIGNAL( reloadSuperStart() ), SideBar, SLOT( reloadQuickFiles() ) );
	connect( FolderView, SIGNAL( reloadCatalogs() ), SidePanel, SLOT( flashCatalogs() ) );
	connect( FolderView, SIGNAL( reloadBookmarks() ), SidePanel, SLOT( flashBookmarks() ) );

	connect( FolderView->fsModel, SIGNAL( directoryLoading( QString ) ), this, SLOT( updateVarious( QString ) ) );
	connect( FolderView->fsModel, SIGNAL( directoryLoading( QString ) ), this, SLOT( updateInfoBar() ) );
	connect( FolderView->fsModel, SIGNAL( directoryLoading( QString ) ), FolderView, SLOT( updateActions() ) );

	connect( FolderView->fsModel, SIGNAL( directoryLoading( QString ) ), SideBar, SLOT( highlight( QString ) ) );

	connect( FolderView->fsModel, SIGNAL( directoryLoading( QString ) ), this, SLOT( setBusyCursor() ) );
	connect( FolderView->fsModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( setNormalCursor() ) );

	/* Update View Modes on the UI */
	connect( FolderView->IconView, SIGNAL( updateViewMode( QString ) ), AddressBar, SLOT( updateViewMode( QString ) ) );

	/* Update GUI if terminal cwd changes */
	connect( Terminal, SIGNAL( chdir( QString ) ), this, SLOT( chdirUI( QString ) ) );

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

	// Display terminal widget
	QAction *termWidgetAct = new QAction( this );
	termWidgetAct->setShortcuts( Settings->Shortcuts.InlineTerminal );
	connect( termWidgetAct, SIGNAL( triggered() ), this, SLOT( showHideTermWidget() ) );

	// Show/Hide side bar
	QAction *toggleSideBar = new QAction( "Toggle Sidebar", this );
	toggleSideBar->setShortcuts( Settings->Shortcuts.ToggleSideBar );
	connect( toggleSideBar, SIGNAL( triggered() ), this, SLOT( toggleSideBarVisible() ) );

	// Show/Hide info panel
	QAction *toggleInfoPanelAct = new QAction( "Toggle InfoPanel", this );
	toggleInfoPanelAct->setShortcut( QKeySequence( tr( "F7" ) ) );
	connect( toggleInfoPanelAct, SIGNAL( triggered() ), this, SLOT( toggleInfoPanel() ) );

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

	// Quit NewBreeze
	QAction *quitNBAct = new QAction( "Quit NewBreeze", this );
	quitNBAct->setShortcuts( Settings->Shortcuts.QuitNewBreeze );
	connect( quitNBAct, SIGNAL( triggered() ), this, SLOT( quit() ) );

	addAction( focusAddressBarAct );
	addAction( newWindowAct );
	addAction( termWidgetAct );
	addAction( toggleSideBar );
	addAction( toggleInfoPanelAct );
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

	/* Hack */
	if ( Settings->General.SidePanelType == 0 ) {
		SideBar->hide();			// Classic SideBar
		SidePanel->show();			// Modern SideBar
	}

	else {
		SideBar->show();			// Classic SideBar
		SidePanel->hide();			// Modern SideBar
	}

	qApp->processEvents();
};

void NewBreeze::showCustomActionsDialog() {

	NBCustomActions *customActions = new NBCustomActions();
	customActions->exec();
};

void NewBreeze::newWindow( QString location ) {

	if ( location.isEmpty() and qobject_cast<QAction*>( sender() ) ) {
		location = FolderView->fsModel->currentDir();
	}

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
		NBPropertiesDialog *propsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Properties, &mTerminate, this );
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

	NBPropertiesDialog *permsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Permissions, &mTerminate, this );
	permsDlg->show();
};

void NewBreeze::handleDriveUrl( QString url ){

	if ( url.startsWith( "NB://SuperStart" ) )
		FolderView->fsModel->goHome();

	if ( url.startsWith( "NB://Applications" ) )
		showApplications();

	else if ( url.startsWith( "NB://Catalogs" ) )
		showCatalogs();

	else if ( url.startsWith( "NB://Folders" ) )
		showFolders();

	else
		FolderView->doOpen( url );
};

void NewBreeze::chdirUI( QString url ){

	/* Update the UI to show the cwd of the shell */
	FolderView->doOpen( url );

	/* Keep the focus on Terminal widget */
	Terminal->setFocus();
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

	FolderView->showFolders();
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

	QAbstractItemView *itemView = qobject_cast<QAbstractItemView*>( FolderView->currentWidget() );

	QRect rect = itemView->visualRect( FolderView->getSelection().at( 0 ) );
	QPoint pos( rect.x() + rect.width() / 2, rect.y() + rect.height() / 2 );

	openWithMenu->exec( itemView->mapToGlobal( pos ) );
};

void NewBreeze::changeViewMode( int mode ) {

	QStringList viewModes = QStringList() << "Tiles" << "Icons" << "Details";
	Settings->General.ViewMode = viewModes.at( mode );

	QSettings sett( FolderView->fsModel->nodePath( ".directory" ), QSettings::NativeFormat );
	sett.setValue( "NewBreeze/ViewMode", Settings->General.ViewMode );

	FolderView->updateViewMode();
};

void NewBreeze::switchToNextView() {

	// if ( Settings->General.ViewMode == QString( "Tiles" ) )
		Settings->General.ViewMode = QString( "Icons" );

	// else if ( Settings->General.ViewMode == QString( "Icons" ) )
		// Settings->General.ViewMode = QString( "Details" );

	// else
		// Settings->General.ViewMode = QString( "Tiles" );

	FolderView->updateViewMode();

	QSettings sett( FolderView->fsModel->nodePath( ".directory" ), QSettings::NativeFormat );
	sett.setValue( "NewBreeze/ViewMode", Settings->General.ViewMode );
	sett.sync();
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

	/* Modern SideBar */
	if ( Settings->General.SidePanelType == 0 ) {
		if ( SidePanel->isVisible() )
			SidePanel->hide();

		else
			SidePanel->show();

		Settings->setValue( "SidePanel", SidePanel->isVisible() );
	}

	/* Classic SidePanel */
	else {
		if ( SideBar->isVisible() )
			SideBar->hide();

		else
			SideBar->show();

		Settings->setValue( "SidePanel", SideBar->isVisible() );
	}
};

void NewBreeze::toggleInfoPanel() {

	if ( InfoPanel->isVisible() ) {
		InfoPanel->hide();
		InfoBar->show();

		Settings->setValue( "InfoPanel", false );
	}

	else {
		InfoPanel->show();
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

	fclose( nblog );

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
	Settings->setValue( "Session/Geometry", geometry() );
	Settings->setValue( "Session/Maximized", isMaximized() );

	Settings->setValue( "Session/LastDir", FolderView->fsModel->lastOpenedFolder() );

	// Come to home dir
	chdir( NBXdg::home().toLocal8Bit().constData() );
	Terminal->changeDir( NBXdg::home() );

	cEvent->accept();

	mClosed = true;
	qDebug( "Good Bye!" );
};
