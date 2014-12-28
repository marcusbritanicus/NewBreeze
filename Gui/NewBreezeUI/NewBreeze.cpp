/*
	*
	* NewBreeze.cpp - NewBreeze MainWindow Class
	*
*/

// Local Headers
#include <NewBreeze.hpp>

NewBreeze::NewBreeze( QString loc, bool tray ) : QMainWindow() {

	if ( not loc.isEmpty() ) {
		if ( loc.endsWith( "/" ) )
			loc.chop( 1 );
	}

	__currentIndex = 0;
	__terminate = false;

	/* If we are opening NewBreeze, open Catalogs, otherwise open the folder */
	if ( not loc.isEmpty() )
		initDir = QString( loc );

	else {
		if ( exists( Settings->Session.LastDir ) )
			initDir = QString( Settings->Session.LastDir );

		else
			initDir = QString( QDir::homePath() );
	}

	if ( tray ) {
		NBTrayIcon* trayIcon = new NBTrayIcon();
		trayIcon->show();

		connect( this, SIGNAL( showTrayIcon() ), trayIcon, SLOT( showInfo() ) );
		connect( trayIcon, SIGNAL( newWindow() ), this, SLOT( newWindow() ) );
	};

	createGUI();
	createAndSetupActions();
	setWindowProperties();

	if ( Settings->General.OpenWithCatalog and loc.isEmpty() )
		UI->FolderView->setCurrentIndex( 2 );
};

void NewBreeze::createGUI() {

	QWidget *MainWidget = new QWidget();

	MainWidget->setContentsMargins( QMargins() );
	MainWidget->setObjectName( tr( "mainWidget" ) );

	QVBoxLayout *MainLayout = new QVBoxLayout();
	MainLayout->setContentsMargins( QMargins() );

	uBar = new NBUtilityBar();
	UI = new NewBreezeUI( initDir );

	// Widgets layout
	MainLayout->addWidget( uBar );
	MainLayout->addWidget( UI );

	MainWidget->setLayout( MainLayout );
	setCentralWidget( MainWidget );

	UI->setFocus();
};

void NewBreeze::setWindowProperties() {

	setWindowTitle( tr( "NewBreeze" ) );
	setWindowIcon( QIcon( ":/icons/newbreeze2.png" ) );

	setMinimumSize( 900, 600 );

	setGeometry( Settings->Session.Geometry );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	if ( not Settings->General.NativeTitleBar )
		setWindowFlags( Qt::FramelessWindowHint );

	setPalette( NBStyleManager::getPalette( Settings->General.Style ) );

	if ( Settings->General.Style == QString( "TransDark" ) )
		setStyleSheet( QString( "QWidget#mainWidget { background-image: url(:/icons/bg.png); }" ) + getStyleSheet( "NewBreeze", Settings->General.Style ) );

	else
		setStyleSheet( getStyleSheet( "NewBreeze", Settings->General.Style ) );
};

void NewBreeze::setWindowTitle( QString title ) {

	QMainWindow::setWindowTitle( title );
};

void NewBreeze::setWindowIcon( QIcon icon ) {

	QMainWindow::setWindowIcon( icon );
};

void NewBreeze::createAndSetupActions() {

	connect( uBar, SIGNAL( titleBarMousePress( QMouseEvent * ) ), this, SLOT( windowPressStart( QMouseEvent * ) ) );
	connect( uBar, SIGNAL( titleBarMouseMove( QMouseEvent * ) ), this, SLOT( windowMoveStart( QMouseEvent * ) ) );
	connect( uBar, SIGNAL( closeWindow() ), this, SLOT( close() ) );
	connect( uBar, SIGNAL( minimizeWindow() ), this, SLOT( showMinimized() ) );
	connect( uBar, SIGNAL( maximizeWindow() ), this, SLOT( toggleMaximizeRestore() ) );
	connect( uBar, SIGNAL( restoreWindow() ), this, SLOT( toggleMaximizeRestore() ) );

	connect( UI, SIGNAL( addJob( QStringList, QString, NBIOMode::Mode ) ), this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );
	connect( UI, SIGNAL( newWindow( QString ) ), this, SLOT( newWindow( QString ) ) );

	// About NB
	QAction *aboutNBAct = new QAction( this );
	aboutNBAct->setShortcuts( Settings->Shortcuts.AboutNB );
	connect( aboutNBAct, SIGNAL( triggered() ), this, SLOT( showAboutNB() ) );

	// About Qt4
	QAction *aboutQtAct = new QAction( this );
	aboutQtAct->setShortcuts( Settings->Shortcuts.AboutQt );
	connect( aboutQtAct, SIGNAL( triggered() ), this, SLOT( showAboutQt4() ) );

	// NB Info
	QAction *showInfoAct = new QAction( this );
	showInfoAct->setShortcuts( Settings->Shortcuts.NBInfo );
	connect( showInfoAct, SIGNAL( triggered() ), this, SLOT( showInfoDlg() ) );

	// Show settings
	QAction *showSettingsAct = new QAction( this );
	showSettingsAct->setShortcuts( Settings->Shortcuts.Settings );
	connect( showSettingsAct, SIGNAL( triggered() ), this, SLOT( showSettingsDialog() ) );

	addAction( aboutNBAct );
	addAction( aboutQtAct );
	addAction( showInfoAct );
	addAction( showSettingsAct );
};

void NewBreeze::showAboutNB() {

	NBMessageDialog::aboutNewBreeze();
};

void NewBreeze::showAboutQt4() {

	NBMessageDialog::aboutQt();
};

void NewBreeze::showSettingsDialog() {

	hide();

	NBSettingsManager *settingsMgr = new NBSettingsManager();
	settingsMgr->exec();

	UI->updateGUI();
	show();
};

void NewBreeze::closeEvent( QCloseEvent *cEvent ) {

	// Close down Info Gathering
	UI->FolderView->fsModel->terminateInfoGathering();

	// Close down recursive size checker in Properties
	__terminate = true;

	// If there are background FileIO jobs, bring them to front
	if ( uBar->procWidget->activeJobs() )
		uBar->procWidget->showAllIODialogs();

	// Now hide this window, other processes may take a while longer to close down
	hide();

	// Store the previous session - geometry, and open directory.
	Settings->setValue( "FolderView", Settings->General.FolderView );
	Settings->setValue( "Session/Geometry", geometry() );
	Settings->setValue( "Session/LastDir", UI->FolderView->fsModel->currentDir() );
	Settings->setValue( "Session/ShowHidden", UI->FolderView->fsModel->showHidden() );
	Settings->setValue( "Session/SortColumn", UI->FolderView->fsModel->sortColumn() );
	Settings->setValue( "Session/SortCase", UI->FolderView->fsModel->sortCaseSensitivity() );
	Settings->setValue( "Session/SortCategory", UI->FolderView->fsModel->sortCategorized() );
	Settings->setValue( "Session/Maximized", isMaximized() );

	qDebug( "Good Bye!" );

	cEvent->accept();
};

void NewBreeze::showInfoDlg() {

	QDialog *shortcutDlg = new QDialog();

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		shortcutDlg->setAttribute( Qt::WA_TranslucentBackground );

	if ( not Settings->General.NativeTitleBar )
		shortcutDlg->setWindowFlags( Qt::FramelessWindowHint );

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

	QPushButton *okBtn = new QPushButton( "&Ok" );
	okBtn->setObjectName( "okBtn" );
	connect( okBtn, SIGNAL( clicked() ), shortcutDlg, SLOT( close() ) );

	btnLyt->addWidget( okBtn );

	if ( not Settings->General.NativeTitleBar )
		baseLyt->addWidget( new QLabel( "<h3>NewBreeze Info</h3>" ), 0, Qt::AlignCenter );
	baseLyt->addWidget( nbInfo );
	baseLyt->addWidget( Separator::horizontal() );
	baseLyt->addLayout( btnLyt );

	QWidget *widget = new QWidget();
	widget->setObjectName( "infoDlg" );
	widget->setLayout( baseLyt );

	dlgLyt->addWidget( widget );
	shortcutDlg->setLayout( dlgLyt );
	shortcutDlg->setStyleSheet( getStyleSheet( "NBDialog", Settings->General.Style ) );

	shortcutDlg->setFixedSize( QSize( 720, 630 ) );

	shortcutDlg->setWindowIcon( QIcon( ":/icons/newbreeze2.png" ) );
	shortcutDlg->setWindowTitle( tr( "NewBreeze Info" ) );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 630 ) / 2 );
	shortcutDlg->move( hpos, vpos );

	shortcutDlg->exec();
};

void NewBreeze::showCustomActionsDialog() {

	NBCustomActions *customActions = new NBCustomActions();
	customActions->exec();
};

void NewBreeze::newWindow( QString location ) {

	qDebug() << "Opening new window at" << location;

	NewBreeze *newbreeze = new NewBreeze( location );
	if ( Settings->Session.Maximized )
		newbreeze->showMaximized();

	else
		newbreeze->showNormal();
};

void NewBreeze::initiateIO( QStringList sourceList, QString target, NBIOMode::Mode iomode ) {

	uBar->procWidget->addJob( sourceList, target, iomode );
};

void NewBreeze::toggleMaximizeRestore() {

	if ( isMaximized() ) {
		showNormal();
	}

	else {
		showMaximized();
	}
};

void NewBreeze::windowPressStart( QMouseEvent *mpEvent ) {

	if ( mpEvent->button() == Qt::LeftButton ) {
		dragPosition = mpEvent->globalPos() - frameGeometry().topLeft();
		mpEvent->accept();
	}
};

void NewBreeze::windowMoveStart( QMouseEvent *mmEvent ) {

	if ( mmEvent->buttons() & Qt::LeftButton ) {
		move( mmEvent->globalPos() - dragPosition );
		mmEvent->accept();
	}
};
