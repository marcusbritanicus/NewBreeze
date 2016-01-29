/*
	*
	* NewBreeze.cpp - NewBreeze MainWindow Class
	*
*/

#include <NewBreeze.hpp>
#include <NBSystemInfo.hpp>

NewBreeze::NewBreeze( QString loc, bool tray ) : QMainWindow() {

	/* HaveUI flag */
	mHaveUI = false;

	/* If loc is non-empty and is a file, just open it and quit */
	if ( loc.count() and isFile( loc ) ) {
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

	// qDebug() << "NewBreeze v3. Ready for Action...!";
};

bool NewBreeze::canOpenUI() {

	return mHaveUI;
};

void NewBreeze::createGUI() {

	UI = new NewBreezeUI( initDir );
	setCentralWidget( UI );
	UI->setFocus();
};

void NewBreeze::setWindowProperties() {

	setWindowTitle( tr( "NewBreeze" ) );
	setWindowIcon( QIcon( ":/icons/newbreeze2.png" ) );

	setMinimumSize( 900, 600 );

	setGeometry( Settings->Session.Geometry );

	if ( not Settings->General.NativeTitleBar )
		setWindowFlags( Qt::FramelessWindowHint );
};

void NewBreeze::setWindowTitle( QString title ) {

	QMainWindow::setWindowTitle( title );
};

void NewBreeze::setWindowIcon( QIcon icon ) {

	QMainWindow::setWindowIcon( icon );
};

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
};

void NewBreeze::showAboutQt4() {

	QMessageBox::aboutQt( this, QObject::tr( "About Qt %1" ).arg( QLatin1String( QT_VERSION_STR ) ) );
};

void NewBreeze::showSettingsDialog() {

	hide();

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

	// Now hide this window, other processes may take a while longer to close down
	hide();

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
};

void NewBreeze::showInfoDlg() {

	NBDialog *infoDlg = new NBDialog( this );
	infoDlg->setDialogTitle( "NewBreeze Info" );
	infoDlg->setDialogIcon( QIcon( ":/icons/newbreeze2.png" ) );

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

	QWidget *widget = new QWidget();
	widget->setLayout( baseLyt );

	dlgLyt->addWidget( widget );
	infoDlg->setLayout( dlgLyt );

	infoDlg->setFixedSize( QSize( 720, 630 ) );

	infoDlg->setWindowIcon( QIcon( ":/icons/newbreeze2.png" ) );
	infoDlg->setWindowTitle( tr( "NewBreeze Info" ) );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 630 ) / 2 );
	infoDlg->move( hpos, vpos );

	infoDlg->exec();
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
