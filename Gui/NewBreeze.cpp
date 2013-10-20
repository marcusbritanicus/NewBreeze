/*
	*
	* NewBreeze.cpp - NewBreeze MainWindow Class
	*
*/

// Local Headers
#include <NewBreeze.hpp>
#include <NBTools.hpp>
#include <NBCustomActions.hpp>
#include <NBSettingsManager.hpp>

NewBreeze::NewBreeze( QString loc ) : QMainWindow() {

	if ( not loc.isEmpty() ) {
		if ( loc.endsWith( "/" ) )
			loc.chop( 1 );
	}

	createGUI();
	createAndSetupActions();
	setWindowProperties();

	if ( not loc.isEmpty() ) {
		FolderView->doOpen( loc );
	}

	else {
		if ( exists( Settings->Session.LastDir ) )
			FolderView->doOpen( Settings->Session.LastDir );

		else
			FolderView->doOpen( QDir::homePath() );
	}

	// By default we do not show hidden files, but if the user
	// had closed while viewing the hidden files, let's show it.
	if ( Settings->Session.ShowHidden )
		FolderView->fsModel->setShowHidden( true );

	FolderView->currentWidget()->setFocus();
};

void NewBreeze::createGUI() {

	QWidget *MainWidget = new QWidget();
	QWidget *BodyWidget = new QWidget();

	MainWidget->setContentsMargins( QMargins() );
	MainWidget->setObjectName( tr( "mainWidget" ) );

	QVBoxLayout *MainLayout = new QVBoxLayout();
	QHBoxLayout *BodyLayout = new QHBoxLayout();
	QVBoxLayout *ViewLayout = new QVBoxLayout();

	MainLayout->setContentsMargins( QMargins() );

	TitleBar = new NBTitleBar();
	AddressBar = new NBAddressBar();
	QuickMenuBar = new NBQuickMenuBar();
	SidePanel = new NBSidePanel();
	FolderView = new NBFolderView();
	InfoBar = new NBInfoBar();
	Terminal = new NBTerminal();

	// Widgets layout
	ViewLayout->addWidget( AddressBar );
	ViewLayout->addWidget( Separator::horizontal() );
	ViewLayout->addWidget( QuickMenuBar );
	ViewLayout->addWidget( Separator::horizontal() );
	ViewLayout->addWidget( FolderView );

	BodyLayout->setContentsMargins( QMargins() );
	BodyLayout->addWidget( SidePanel );
	BodyLayout->addWidget( Separator::vertical() );
	BodyLayout->addLayout( ViewLayout );

	BodyWidget->setLayout( BodyLayout );

	if ( not Settings->General.NativeTitleBar ) {
		MainLayout->addWidget( TitleBar );
		MainLayout->addWidget( Separator::horizontal() );
	}
	MainLayout->addWidget( BodyWidget );
	MainLayout->addWidget( Terminal );
	MainLayout->addWidget( Separator::horizontal() );
	MainLayout->addWidget( InfoBar );

	MainWidget->setLayout( MainLayout );
	setCentralWidget( MainWidget );

	// Widget Properties
	AddressBar->setFixedHeight( 24 );
	QuickMenuBar->setFixedHeight( 24 );
	FolderView->setObjectName( "mainList" );
	Terminal->setFixedHeight( 270 );
	Terminal->hide();

	if ( not Settings->Session.SidePanel )
		SidePanel->hide();

	TitleBar->setFocusPolicy( Qt::NoFocus );
	AddressBar->setFocusPolicy( Qt::NoFocus );
	SidePanel->setFocusPolicy( Qt::NoFocus );
	FolderView->setFocusPolicy( Qt::StrongFocus );
	InfoBar->setFocusPolicy( Qt::NoFocus );
};

void NewBreeze::setWindowProperties() {

	setWindowTitle( tr( "NewBreeze" ) );
	setWindowIcon( QIcon( ":/icons/newbreeze.png" ) );

	setMinimumSize( 900, 600 );

	setGeometry( Settings->Session.Geometry);

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	if ( not Settings->General.NativeTitleBar )
		setWindowFlags( Qt::FramelessWindowHint );

	setStyleSheet( getStyleSheet( "NewBreeze", Settings->General.Style ) );
};

void NewBreeze::setWindowTitle( QString title ) {

	TitleBar->setTitle( title );
	QMainWindow::setWindowTitle( title );
};

void NewBreeze::setWindowIcon( QIcon icon ) {

	TitleBar->setIcon( icon );
	QMainWindow::setWindowIcon( icon );
};

void NewBreeze::createAndSetupActions() {

	connect( TitleBar, SIGNAL( aboutNB() ),
		this, SLOT( showAboutNB() ) );

	connect( TitleBar, SIGNAL( aboutQt4() ),
		this, SLOT( showAboutQt4() ) );

	connect( TitleBar, SIGNAL( titlebarMousePress( QMouseEvent * ) ),
		this, SLOT( windowPressStart( QMouseEvent * ) ) );

	connect( TitleBar, SIGNAL( titlebarMousePress( QMouseEvent * ) ),
		this, SLOT( windowPressStart( QMouseEvent * ) ) );

	connect( TitleBar, SIGNAL( titlebarMouseMove( QMouseEvent * ) ),
		this, SLOT( windowMoveStart( QMouseEvent * ) ) );

	connect( TitleBar, SIGNAL( closeWindow() ),
		this, SLOT( close() ) );

	connect( TitleBar, SIGNAL( minimizeWindow() ),
		this, SLOT( showMinimized() ) );

	connect( TitleBar, SIGNAL( maximizeWindow() ),
		this, SLOT( toggleMaximizeRestore() ) );

	connect( TitleBar, SIGNAL( restoreWindow() ),
		this, SLOT( toggleMaximizeRestore() ) );

	connect( AddressBar->openVTEBtn, SIGNAL( clicked() ),
		FolderView, SLOT( openTerminal() ) );

	connect( AddressBar->reloadBtn, SIGNAL( clicked() ),
		FolderView, SLOT( doReload() ) );

	connect( AddressBar->viewModeBtn, SIGNAL( switchToNextView() ),
		this, SLOT( switchToNextView() ) );

	connect( AddressBar->viewModeBtn, SIGNAL( changeViewMode() ),
		this, SLOT( changeViewMode() ) );

	connect( AddressBar->addressWidget->addressEdit, SIGNAL( returnPressed() ),
		this, SLOT( openAddressBar() ) );

	connect( AddressBar->addressWidget->crumbsBar, SIGNAL( openLocation( QString ) ),
		this, SLOT( openAddressBar( QString ) ) );

	// connect( AddressBar->searchBar, SIGNAL( searchString( QString ) ),
		// this, SLOT( filterFiles( QString ) ) );

	// connect( AddressBar->searchBar, SIGNAL( searchCleared() ),
		// this, SLOT( clearFilters() ) );

	connect( QuickMenuBar->openWithBtn, SIGNAL( clicked() ),
		this, SLOT( openWithList() ) );

	connect( QuickMenuBar->deleteBtn, SIGNAL( clicked() ),
		FolderView, SLOT( doDelete() ) );

	connect( QuickMenuBar->propsBtn, SIGNAL( clicked() ),
		FolderView, SLOT( showProperties() ) );

	connect( SidePanel, SIGNAL( driveClicked( QString ) ),
		FolderView, SLOT( doOpen( QString ) ) );

	connect( SidePanel, SIGNAL( copy( QStringList, QString, NBIOMode::Mode ) ),
		this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );

	connect( SidePanel, SIGNAL( move( QStringList, QString, NBIOMode::Mode ) ),
		this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );

	connect( FolderView, SIGNAL( copy( QStringList, QString, NBIOMode::Mode ) ),
		this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );

	connect( FolderView, SIGNAL( move( QStringList, QString, NBIOMode::Mode ) ),
		this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );

	connect( FolderView, SIGNAL( focusSearchBar() ),
		this, SLOT( focusSearch() ) );

	connect( FolderView, SIGNAL( clearSearchBar() ),
		this, SLOT( clearSearch() ) );

	connect( FolderView, SIGNAL( newWindow( QString ) ),
		this, SLOT( openInNewWindow( QString ) ) );

	connect( FolderView, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
		this, SLOT( updateInfoBar() ) );

	connect( FolderView, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
		this, SLOT( updateQuickMenuBar() ) );

	connect( FolderView->fsModel, SIGNAL( dirLoading( QString ) ),
		this, SLOT( updateVarious( QString ) ) );

	connect( FolderView->fsModel, SIGNAL( dirLoading( QString ) ),
		this, SLOT( updateInfoBar() ) );

	connect( FolderView->fsModel, SIGNAL( directoryLoaded( QString ) ),
		this, SLOT( updateQuickMenuBar() ) );

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

	// Custom Actions
	QAction *showCustomActionsAct = new QAction( this );
	showCustomActionsAct->setShortcuts( Settings->Shortcuts.CustomActions );
	connect( showCustomActionsAct, SIGNAL( triggered() ), this, SLOT( showCustomActionsDialog() ) );

	// Focus AddressBar
	QAction *focusAddressBarAct = new QAction( this );
	focusAddressBarAct->setShortcuts( Settings->Shortcuts.FocusAddressBar );
	connect( focusAddressBarAct, SIGNAL( triggered() ), AddressBar->addressWidget->addressEdit, SLOT( setFocus() ) );

	QAction *showSettingsAct = new QAction( this );
	showSettingsAct->setShortcuts( Settings->Shortcuts.Settings );
	connect( showSettingsAct, SIGNAL( triggered() ), this, SLOT( showSettingsDialog() ) );

	QAction *newWindowAct = new QAction( this );
	newWindowAct->setShortcuts( Settings->Shortcuts.NewWindow );
	connect( newWindowAct, SIGNAL( triggered() ), this, SLOT( openNewWindow() ) );

	addBookMarkAct = new QAction( this );
	addBookMarkAct->setShortcuts( Settings->Shortcuts.AddBookmark );
	connect( addBookMarkAct, SIGNAL( triggered() ), this, SLOT( addBookMark() ) );

	// Update devices list
	int mountsFD = open( "/proc/self/mounts", O_RDONLY, 0 );
	QSocketNotifier *devWatcher = new QSocketNotifier( mountsFD, QSocketNotifier::Write );
	connect( devWatcher, SIGNAL( activated( int ) ), SidePanel, SLOT( updateDevices() ) );

	QAction *termWidgetAct = new QAction( this );
	termWidgetAct->setShortcuts( Settings->Shortcuts.InlineTerminal );
	connect( termWidgetAct, SIGNAL( triggered() ), this, SLOT( showHideTermWidget() ) );
	connect (Terminal, SIGNAL( shown( bool ) ), addBookMarkAct, SLOT( setDisabled( bool ) ) );

	// Show/Hide side bar
	QAction *toggleSideBar = new QAction( "Toggle Sidebar", this );
	toggleSideBar->setShortcuts( Settings->Shortcuts.ToggleSideBar );

	connect( toggleSideBar, SIGNAL( triggered() ), this, SLOT( toggleSideBarVisible() ) );
	addAction( toggleSideBar );

	addAction( aboutNBAct );
	addAction( aboutQtAct );
	addAction( showInfoAct );
	addAction( showCustomActionsAct );
	addAction( focusAddressBarAct );
	addAction( showSettingsAct );
	addAction( newWindowAct );
	addAction( addBookMarkAct );
	addAction( termWidgetAct );
};

void NewBreeze::updateGUI() {

	TitleBar->setStyleSheet( getStyleSheet( "NBTitleBar", Settings->General.Style ) );
	AddressBar->setStyleSheet( getStyleSheet( "NBTooBar", Settings->General.Style ) );
	SidePanel->setStyleSheet( getStyleSheet( "NBSidePanel", Settings->General.Style ) );
	Terminal->setStyleSheet( getStyleSheet( "NBTerminal", Settings->General.Style ) );
	InfoBar->setStyleSheet( getStyleSheet( "NBInfoBar", Settings->General.Style ) );

	setWindowProperties();
};

void NewBreeze::toggleMaximizeRestore() {

	if ( isMaximized() ) {
		showNormal();
		TitleBar->isMaximized = false;
	}

	else {
		showMaximized();
		TitleBar->isMaximized = true;
	}
};

void NewBreeze::toggleSideBarVisible() {

	if ( SidePanel->isVisible() )
		SidePanel->hide();

	else
		SidePanel->show();

	Settings->setValue( "Session/SidePanel", SidePanel->isVisible() );
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

	updateGUI();
	show();
};

void NewBreeze::showCustomActionsDialog() {

	NBCustomActions *customActions = new NBCustomActions();
	customActions->exec();
};

void NewBreeze::closeEvent( QCloseEvent *cEvent ) {

	// If there are background FileIO jobs, bring them to front
	InfoBar->ioManager->showAllIODialogs();

	// Store the previous session - geometry, and open directory.
	Settings->setValue( "Session/Geometry", geometry() );
	Settings->setValue( "Session/LastDir", FolderView->fsModel->currentDir() );
	Settings->setValue( "Session/ShowHidden", FolderView->fsModel->showHidden() );
	Settings->setValue( "Session/Maximized", isMaximized() );
	Settings->setValue( "Session/ExpandDevices", SidePanel->isExpanded( SidePanel->spModel->index( 0, 0 ) ) );
	Settings->setValue( "Session/ExpandBookmarks", SidePanel->isExpanded( SidePanel->spModel->index( 1, 0 ) ) );

	qDebug() << "Good Bye!";

	cEvent->accept();
};


void NewBreeze::resizeEvent( QResizeEvent *rEvent ) {

	int fvcWidth = FolderView->size().width();

	if ( not FolderView->currentIndex() ) {
		fvcWidth -= FolderView->IconView->contentsMargins().left();
		fvcWidth -= FolderView->IconView->contentsMargins().right();
		fvcWidth -= FolderView->IconView->autoScrollMargin();
		fvcWidth -= FolderView->IconView->verticalScrollBar()->width();
	}

	FolderView->setContentsWidth( fvcWidth );
	FolderView->updateViewMode();

	rEvent->accept();
};

void NewBreeze::showEvent( QShowEvent *sEvent ) {

	int fvcWidth = FolderView->size().width();

	if ( not FolderView->currentIndex() ) {
		fvcWidth -= FolderView->IconView->contentsMargins().left();
		fvcWidth -= FolderView->IconView->contentsMargins().right();
		fvcWidth -= FolderView->IconView->autoScrollMargin();
		fvcWidth -= FolderView->IconView->verticalScrollBar()->width();
	}

	FolderView->setContentsWidth( fvcWidth );
	FolderView->updateViewMode();

	sEvent->accept();
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

	SidePanel->updateBookmarks();
};

void NewBreeze::showHideTermWidget() {

	if ( Terminal->isVisible() ) {
		Terminal->hide();
		FolderView->setFocus();
	}

	else {
		Terminal->show();
		return;
	}
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

	shortcutDlg->setWindowIcon( QIcon( ":/icons/newbreeze.png" ) );
	shortcutDlg->setWindowTitle( tr( "NewBreeze Info" ) );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 630 ) / 2 );
	shortcutDlg->move( hpos, vpos );

	shortcutDlg->exec();
};

void NewBreeze::openAddressBar() {

	if ( !QFileInfo( AddressBar->addressWidget->addressEdit->text() ).exists() ) {
		QString title = QString( "Invalid Location" );
		QString icon = QString( ":/icons/close.png" );
		QString text = QString( "There is no file or directory named: "		\
			"<tt><b>%1</b></tt>. Please check the path entered."
		).arg(  AddressBar->addressWidget->addressEdit->text() );

		QString btns = "O";
		QString wTitle = QString( "NewBreeze - Invalid Location" );
		QString wIcon = QString( ":/icons/newbreeze.png" );

		QDialog *errDlg = getMessageDialog( title, icon, text, wTitle, wIcon, btns );
		errDlg->setFixedWidth( 540 );

		QDesktopWidget dw;
		int hpos = (int)( ( dw.width() - 540 ) / 2 );
		int vpos = (int)( ( dw.height() - errDlg->sizeHint().height() ) / 2 );
		errDlg->setGeometry( hpos, vpos, 540, errDlg->sizeHint().height() );

		errDlg->exec();

		return;
	}

	FolderView->setFocus();
	FolderView->doOpen( AddressBar->addressWidget->addressEdit->text() );
};

void NewBreeze::openAddressBar( QString path ) {

	FolderView->setFocus();
	FolderView->doOpen( path );
};

void NewBreeze::newWindow( QString path ) {

	if ( path.isEmpty() )
		path = QDir::homePath();

	NewBreeze *newbreeze = new NewBreeze( path );
	if ( Settings->Session.Maximized )
		newbreeze->showMaximized();

	else
		newbreeze->showNormal();
};

void NewBreeze::openNewWindow() {

	QString location = FolderView->fsModel->currentDir();
	qDebug() << "Opening new window at " << location;

	NewBreeze *newbreeze = new NewBreeze( location );
	if ( Settings->Session.Maximized )
		newbreeze->showMaximized();

	else
		newbreeze->showNormal();
};

void NewBreeze::openInNewWindow( QString location ) {

	qDebug() << "Opening new window at " << location;

	NewBreeze *newbreeze = new NewBreeze( location );
	if ( Settings->Session.Maximized )
		newbreeze->showMaximized();

	else
		newbreeze->showNormal();
};

void NewBreeze::focusSearch() {

	// AddressBar->searchBar->searchLE->setFocus();
};

void NewBreeze::clearSearch() {

	// AddressBar->searchBar->searchLE->clear();
	clearFilters();

	FolderView->setFocus();
};

void NewBreeze::filterFiles( QString filter ) {

	Q_UNUSED( filter );
	// FolderView->fsModel->setNameFilters( QStringList() << QString( "*%1*" ).arg( filter ) );
};

void NewBreeze::clearFilters() {

	// FolderView->fsModel->setNameFilters( QStringList() );
};

void NewBreeze::initiateIO( QStringList sourceList, QString target, NBIOMode::Mode iomode ) {

	InfoBar->ioManager->addJob( sourceList, target, iomode );
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
