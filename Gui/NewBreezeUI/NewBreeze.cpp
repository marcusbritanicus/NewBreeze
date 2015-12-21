/*
	*
	* NewBreeze.cpp - NewBreeze MainWindow Class
	*
*/

<<<<<<< HEAD
#include <NewBreeze.hpp>
#include <NBSystemInfo.hpp>

NewBreeze::NewBreeze( QString loc, bool tray ) : QMainWindow() {

	/* HaveUI flag */
	mHaveUI = false;

	/* If loc is non-empty and is a file, just open it and quit */
	if ( loc.count() and isFile( loc ) ) {
		qDebug() << "Init open file";
		openFile( loc );
		close();
		return;
	}

	/* loc not a file, processes it */
	else {
		if ( not loc.isEmpty() ) {
			if ( loc.endsWith( "/" ) )
				loc.chop( 1 );
		}

		mHaveUI = true;
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

			connect( trayIcon, SIGNAL( newWindow() ), this, SLOT( newWindow() ) );
		};

		createGUI();
		createAndSetupActions();
		setWindowProperties();

		if ( Settings->General.OpenWithCatalog and loc.isEmpty() )
			UI->FolderView->setCurrentIndex( 2 );
	}
};

bool NewBreeze::canOpenUI() {

	return mHaveUI;
};

void NewBreeze::createGUI() {

	UI = new NewBreezeUI( initDir );
	setCentralWidget( UI );
=======
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

>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	UI->setFocus();
};

void NewBreeze::setWindowProperties() {

	setWindowTitle( tr( "NewBreeze" ) );
	setWindowIcon( QIcon( ":/icons/newbreeze2.png" ) );

	setMinimumSize( 900, 600 );

	setGeometry( Settings->Session.Geometry );

<<<<<<< HEAD
	if ( not Settings->General.NativeTitleBar )
		setWindowFlags( Qt::FramelessWindowHint );
=======
	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	if ( not Settings->General.NativeTitleBar )
		setWindowFlags( Qt::FramelessWindowHint );

	setPalette( NBStyleManager::getPalette( Settings->General.Style ) );

	if ( Settings->General.Style == QString( "TransDark" ) )
		setStyleSheet( QString( "QWidget#mainWidget { background-image: url(:/icons/bg.png); }" ) + getStyleSheet( "NewBreeze", Settings->General.Style ) );

	else
		setStyleSheet( getStyleSheet( "NewBreeze", Settings->General.Style ) );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

void NewBreeze::setWindowTitle( QString title ) {

	QMainWindow::setWindowTitle( title );
};

void NewBreeze::setWindowIcon( QIcon icon ) {

	QMainWindow::setWindowIcon( icon );
};

<<<<<<< HEAD
bool NewBreeze::showTrayIcon() {

	return mShowTrayIcon;
};

void NewBreeze::setShowTrayIcon( bool show ) {

	mShowTrayIcon = show;
};

void NewBreeze::showTrayIconInfo( QString message ) {

	Q_UNUSED( message );
};

void NewBreeze::createAndSetupActions() {

	if ( not Settings->General.NativeTitleBar ) {
		connect( UI, SIGNAL( titleBarMousePress( QMouseEvent * ) ), this, SLOT( windowPressStart( QMouseEvent * ) ) );
		connect( UI, SIGNAL( titleBarMouseMove( QMouseEvent * ) ), this, SLOT( windowMoveStart( QMouseEvent * ) ) );
		connect( UI, SIGNAL( closeWindow() ), this, SLOT( close() ) );
		connect( UI, SIGNAL( minimizeWindow() ), this, SLOT( showMinimized() ) );
		connect( UI, SIGNAL( maximizeWindow() ), this, SLOT( toggleMaximizeRestore() ) );
		connect( UI, SIGNAL( restoreWindow() ), this, SLOT( toggleMaximizeRestore() ) );
	}

=======
void NewBreeze::createAndSetupActions() {

	connect( uBar, SIGNAL( titleBarMousePress( QMouseEvent * ) ), this, SLOT( windowPressStart( QMouseEvent * ) ) );
	connect( uBar, SIGNAL( titleBarMouseMove( QMouseEvent * ) ), this, SLOT( windowMoveStart( QMouseEvent * ) ) );
	connect( uBar, SIGNAL( closeWindow() ), this, SLOT( close() ) );
	connect( uBar, SIGNAL( minimizeWindow() ), this, SLOT( showMinimized() ) );
	connect( uBar, SIGNAL( maximizeWindow() ), this, SLOT( toggleMaximizeRestore() ) );
	connect( uBar, SIGNAL( restoreWindow() ), this, SLOT( toggleMaximizeRestore() ) );

	connect( UI, SIGNAL( addJob( QStringList, QString, NBIOMode::Mode ) ), this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
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

<<<<<<< HEAD
void NewBreeze::openFile( QString file ) {

	NBDebugMsg( DbgMsgPart::HEAD, "Opening file: %s ", qPrintable( file ) );
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

void NewBreeze::showAboutNB() {

	AboutNB *about = new AboutNB( this );
	about->exec();
=======
void NewBreeze::showAboutNB() {

	NBMessageDialog::aboutNewBreeze();
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

void NewBreeze::showAboutQt4() {

<<<<<<< HEAD
	QMessageBox::aboutQt( this, QObject::tr( "About Qt %1" ).arg( QLatin1String( QT_VERSION_STR ) ) );
=======
	NBMessageDialog::aboutQt();
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

void NewBreeze::showSettingsDialog() {

	hide();

<<<<<<< HEAD
	NBSettingsManager *settingsMgr = new NBSettingsManager( this );
	settingsMgr->exec();

	show();
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

void NewBreeze::closeEvent( QCloseEvent *cEvent ) {

	/* If we have a UI */
	if ( mHaveUI ) {

		// Close down Info Gathering
		UI->FolderView->fsModel->terminateInfoGathering();

		// Close down recursive size checker in Properties
		__terminate = true;

		// If there are background FileIO jobs, bring them to front
		if ( UI->activeJobs() )
			UI->showActiveJobs();

		// Store the previous session - geometry, and open directory.
		Settings->setValue( "Session/Geometry", geometry() );
		Settings->setValue( "Session/LastDir", UI->FolderView->fsModel->currentDir() );
		Settings->setValue( "Session/Maximized", isMaximized() );

		// Come to home dir
		chdir( NBXdg::home().toLocal8Bit().constData() );
		UI->Terminal->changeDir( NBXdg::home() );
	}
=======
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
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

	// Now hide this window, other processes may take a while longer to close down
	hide();

<<<<<<< HEAD
	cEvent->accept();

	qDebug( "Good Bye!" );
};

void NewBreeze::paintEvent( QPaintEvent *pEvent ) {

	QMainWindow::paintEvent( pEvent );

	if ( Settings->General.NativeTitleBar ) {
		QPainter *painter = new QPainter( this );
		painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 1.0 ) );

		painter->drawLine( rect().topLeft(), rect().topRight() );

		pEvent->accept();
		return;
	}

	QPainter *painter = new QPainter( this );

	painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 2.0 ) );
	painter->drawRect( rect().adjusted( +1, +1, -1, -1 ) );

	painter->end();

	pEvent->accept();
=======
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
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

void NewBreeze::showInfoDlg() {

<<<<<<< HEAD
	NBDialog *infoDlg = new NBDialog( this );
	infoDlg->setDialogTitle( "NewBreeze Info" );
	infoDlg->setDialogIcon( QIcon( ":/icons/newbreeze2.png" ) );
=======
	QDialog *shortcutDlg = new QDialog();

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		shortcutDlg->setAttribute( Qt::WA_TranslucentBackground );

	if ( not Settings->General.NativeTitleBar )
		shortcutDlg->setWindowFlags( Qt::FramelessWindowHint );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

	QHBoxLayout *dlgLyt = new QHBoxLayout();
	dlgLyt->setContentsMargins( 0, 0, 0, 0 );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	QTextBrowser *nbInfo = new QTextBrowser();
<<<<<<< HEAD

=======
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	QFile nbInfoF( ":/README" );
	nbInfoF.open( QFile::ReadOnly );
	nbInfo->setText( QString( nbInfoF.readAll() ) );
	nbInfoF.close();
	nbInfo->setFocusPolicy( Qt::NoFocus );
<<<<<<< HEAD
	nbInfo->setFont( QFont( "Courier 10 Pitch", 9 ) );

	QPushButton *okBtn = new QPushButton( "&Ok" );
	okBtn->setObjectName( "okBtn" );
	connect( okBtn, SIGNAL( clicked() ), infoDlg, SLOT( close() ) );

	btnLyt->addStretch();
	btnLyt->addWidget( okBtn );
	btnLyt->addStretch();

=======

	QPushButton *okBtn = new QPushButton( "&Ok" );
	okBtn->setObjectName( "okBtn" );
	connect( okBtn, SIGNAL( clicked() ), shortcutDlg, SLOT( close() ) );

	btnLyt->addWidget( okBtn );

	if ( not Settings->General.NativeTitleBar )
		baseLyt->addWidget( new QLabel( "<h3>NewBreeze Info</h3>" ), 0, Qt::AlignCenter );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	baseLyt->addWidget( nbInfo );
	baseLyt->addWidget( Separator::horizontal() );
	baseLyt->addLayout( btnLyt );

	QWidget *widget = new QWidget();
<<<<<<< HEAD
	widget->setLayout( baseLyt );

	dlgLyt->addWidget( widget );
	infoDlg->setLayout( dlgLyt );

	infoDlg->setFixedSize( QSize( 720, 630 ) );

	infoDlg->setWindowIcon( QIcon( ":/icons/newbreeze2.png" ) );
	infoDlg->setWindowTitle( tr( "NewBreeze Info" ) );
=======
	widget->setObjectName( "infoDlg" );
	widget->setLayout( baseLyt );

	dlgLyt->addWidget( widget );
	shortcutDlg->setLayout( dlgLyt );
	shortcutDlg->setStyleSheet( getStyleSheet( "NBDialog", Settings->General.Style ) );

	shortcutDlg->setFixedSize( QSize( 720, 630 ) );

	shortcutDlg->setWindowIcon( QIcon( ":/icons/newbreeze2.png" ) );
	shortcutDlg->setWindowTitle( tr( "NewBreeze Info" ) );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 630 ) / 2 );
<<<<<<< HEAD
	infoDlg->move( hpos, vpos );

	infoDlg->exec();
=======
	shortcutDlg->move( hpos, vpos );

	shortcutDlg->exec();
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
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

<<<<<<< HEAD
=======
void NewBreeze::initiateIO( QStringList sourceList, QString target, NBIOMode::Mode iomode ) {

	uBar->procWidget->addJob( sourceList, target, iomode );
};

>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
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
