/*
	*
	* NewBreezeUI.cpp - NewBreeze UI Class
	*
*/

// Local Headers
#include <NewBreezeUI.hpp>

NewBreezeUI::NewBreezeUI( QString loc ) : QWidget() {

	__terminate = false;

	createGUI();
	createAndSetupActions();

	// By default we do not show hidden files, but if the user
	// had closed while viewing the hidden files, let's show it.
<<<<<<< HEAD
	if ( Settings->General.ShowHidden )
=======
	if ( Settings->Session.ShowHidden )
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
		FolderView->fsModel->setShowHidden( true );

	FolderView->doOpen( loc );
	FolderView->currentWidget()->setFocus();
};

void NewBreezeUI::createGUI() {

	QWidget *BodyWidget = new QWidget();

	QVBoxLayout *MainLayout = new QVBoxLayout();
	QHBoxLayout *BodyLayout = new QHBoxLayout();
	QVBoxLayout *ViewLayout = new QVBoxLayout();

	MainLayout->setContentsMargins( QMargins() );
	ViewLayout->setContentsMargins( QMargins( 0, 0, 3, 0 ) );
	ViewLayout->setSpacing( 3 );
	BodyLayout->setContentsMargins( QMargins() );
	BodyLayout->setSpacing( 0 );

<<<<<<< HEAD
	uBar = new NBUtilityBar();
	AddressBar = new NBAddressBar( this );
	SidePanel = new NBSidePanel( this );
	FolderView = new NBFolderView( this );
	InfoBar = new NBInfoBar( this );
=======
	AddressBar = new NBAddressBar();
	QuickMenuBar = new NBQuickMenuBar();
	SidePanel = new NBSidePanel( this );
	FolderView = new NBFolderView();
	InfoBar = new NBInfoBar();
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	Terminal = new NBTerminal();

	// Widgets layout
	ViewLayout->addWidget( AddressBar );
<<<<<<< HEAD
	ViewLayout->addWidget( FolderView );

	BodyLayout->addWidget( SidePanel );
	// BodyLayout->addWidget( NBSpacer::horizontal( 3 ) );
=======
	ViewLayout->addWidget( Separator::horizontal() );
	ViewLayout->addWidget( QuickMenuBar );
	ViewLayout->addWidget( FolderView );

	BodyLayout->addWidget( SidePanel );
	BodyLayout->addWidget( Separator::vertical() );
	BodyLayout->addWidget( NBSpacer::horizontal( 3 ) );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	BodyLayout->addLayout( ViewLayout );

	BodyWidget->setLayout( BodyLayout );

<<<<<<< HEAD
	if ( not Settings->General.NativeTitleBar )
		MainLayout->addWidget( uBar );

	else
		MainLayout->setContentsMargins( QMargins( 0, 5, 0, 0 ) );

=======
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	MainLayout->addWidget( BodyWidget );
	MainLayout->addWidget( Terminal );
	MainLayout->addWidget( Separator::horizontal() );
	MainLayout->addWidget( InfoBar );

	setLayout( MainLayout );

	// Widget Properties
	AddressBar->setFixedHeight( 24 );
<<<<<<< HEAD
	Terminal->setFixedHeight( 270 );
	Terminal->hide();

=======
	QuickMenuBar->setFixedHeight( 24 );
	FolderView->setObjectName( "mainList" );
	Terminal->setFixedHeight( 270 );
	Terminal->hide();

	if ( not Settings->Session.SidePanel )
		SidePanel->hide();

>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	AddressBar->setFocusPolicy( Qt::NoFocus );
	SidePanel->setFocusPolicy( Qt::NoFocus );
	FolderView->setFocusPolicy( Qt::StrongFocus );
	InfoBar->setFocusPolicy( Qt::NoFocus );

	setFocusPolicy( Qt::NoFocus );
};

void NewBreezeUI::createAndSetupActions() {

<<<<<<< HEAD
	connect( uBar, SIGNAL( titleBarMousePress( QMouseEvent * ) ), this, SIGNAL( titleBarMousePress( QMouseEvent * ) ) );
	connect( uBar, SIGNAL( titleBarMouseMove( QMouseEvent * ) ), this, SIGNAL( titleBarMouseMove( QMouseEvent * ) ) );
	connect( uBar, SIGNAL( closeWindow() ), this, SIGNAL( closeWindow() ) );
	connect( uBar, SIGNAL( minimizeWindow() ), this, SIGNAL( minimizeWindow() ) );
	connect( uBar, SIGNAL( maximizeWindow() ), this, SIGNAL( maximizeWindow() ) );
	connect( uBar, SIGNAL( restoreWindow() ), this, SIGNAL( restoreWindow() ) );

	connect( AddressBar, SIGNAL( openTerminal() ), FolderView, SLOT( openTerminal() ) );
	connect( AddressBar, SIGNAL( reload() ), FolderView, SLOT( doReload() ) );
	connect( AddressBar, SIGNAL( switchToNextView() ), this, SLOT( switchToNextView() ) );
	connect( AddressBar, SIGNAL( changeViewMode() ), this, SLOT( changeViewMode() ) );
	connect( AddressBar, SIGNAL( openLocation( QString ) ), this, SLOT( openAddress( QString ) ) );
	connect( AddressBar, SIGNAL( search( QString ) ), this, SLOT( filterFiles( QString ) ) );
	connect( AddressBar, SIGNAL( clearSearch() ), this, SLOT( clearFilters() ) );

	// connect( QuickMenu, SIGNAL( openWithClicked() ), this, SLOT( openWithList() ) );
	// connect( QuickMenu, SIGNAL( deleteClicked() ), FolderView, SLOT( doDelete() ) );
	// connect( QuickMenu, SIGNAL( propsClicked() ), this, SLOT( showProperties() ) );
	// connect( QuickMenu, SIGNAL( permsClicked() ), this, SLOT( showPermissions() ) );
=======
	connect( AddressBar->openVTEBtn, SIGNAL( clicked() ), FolderView, SLOT( openTerminal() ) );
	connect( AddressBar->reloadBtn, SIGNAL( clicked() ), FolderView, SLOT( doReload() ) );
	connect( AddressBar->viewModeBtn, SIGNAL( switchToNextView() ), this, SLOT( switchToNextView() ) );
	connect( AddressBar->viewModeBtn, SIGNAL( changeViewMode() ), this, SLOT( changeViewMode() ) );
	connect( AddressBar->addressWidget->addressEdit, SIGNAL( returnPressed() ), this, SLOT( openAddressBar() ) );
	connect( AddressBar->addressWidget->crumbsBar, SIGNAL( openLocation( QString ) ), this, SLOT( openAddress( QString ) ) );
	connect( AddressBar->searchBar, SIGNAL( searchString( QString ) ), this, SLOT( filterFiles( QString ) ) );
	connect( AddressBar->searchBar, SIGNAL( searchCleared() ), this, SLOT( clearFilters() ) );

	connect( QuickMenuBar, SIGNAL( openWithClicked() ), this, SLOT( openWithList() ) );
	connect( QuickMenuBar, SIGNAL( deleteClicked() ), FolderView, SLOT( doDelete() ) );
	connect( QuickMenuBar, SIGNAL( propsClicked() ), this, SLOT( showProperties() ) );
	connect( QuickMenuBar, SIGNAL( permsClicked() ), this, SLOT( showPermissions() ) );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

	connect( SidePanel, SIGNAL( driveClicked( QString ) ), this, SLOT( handleDriveUrl( QString ) ) );
	connect( SidePanel, SIGNAL( showFolders() ), this, SLOT( showFolders() ) );
	connect( SidePanel, SIGNAL( showApplications() ), this, SLOT( showApplications() ) );
	connect( SidePanel, SIGNAL( showCatalogs() ), this, SLOT( showCatalogs() ) );
	connect( SidePanel, SIGNAL( showTrash() ), this, SLOT( showTrash() ) );

	connect( SidePanel, SIGNAL( copy( QStringList, QString, NBIOMode::Mode ) ),
		this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );

	connect( SidePanel, SIGNAL( move( QStringList, QString, NBIOMode::Mode ) ),
		this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );

	connect( FolderView, SIGNAL( copy( QStringList, QString, NBIOMode::Mode ) ),
		this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );

	connect( FolderView, SIGNAL( move( QStringList, QString, NBIOMode::Mode ) ),
		this, SLOT( initiateIO( QStringList, QString, NBIOMode::Mode ) ) );

	connect( FolderView, SIGNAL( showProperties() ), this, SLOT( showProperties() ) );
	connect( FolderView, SIGNAL( showPermissions() ), this, SLOT( showPermissions() ) );
<<<<<<< HEAD
	connect( FolderView, SIGNAL( focusSearchBar() ), AddressBar, SLOT( focusSearchBar() ) );
	connect( FolderView, SIGNAL( clearSearchBar() ), this, SLOT( clearSearch() ) );
	connect( FolderView, SIGNAL( newWindow( QString ) ), this, SIGNAL( newWindow( QString ) ) );
	connect( FolderView, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ), this, SLOT( updateInfoBar() ) );
=======
	connect( FolderView, SIGNAL( focusSearchBar() ), this, SLOT( focusSearch() ) );
	connect( FolderView, SIGNAL( clearSearchBar() ), this, SLOT( clearSearch() ) );
	connect( FolderView, SIGNAL( newWindow( QString ) ), this, SIGNAL( newWindow( QString ) ) );
	connect( FolderView, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ), this, SLOT( updateInfoBar() ) );
	connect( FolderView, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ), this, SLOT( updateQuickMenuBar() ) );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	connect( FolderView, SIGNAL( reloadCatalogs() ), FolderView->widget( 2 ), SLOT( reload() ) );
	connect( FolderView, SIGNAL( toggleGroups() ), this, SLOT( toggleGrouping() ) );
	connect( FolderView, SIGNAL( updateAddressBar( QString ) ), this, SLOT( updateVarious( QString ) ) );

<<<<<<< HEAD
	connect( FolderView, SIGNAL( hideStatusBar() ), InfoBar, SLOT( hide() ) );
	connect( FolderView, SIGNAL( showStatusBar() ), InfoBar, SLOT( show() ) );

	connect( FolderView->fsModel, SIGNAL( dirLoading( QString ) ), this, SLOT( updateVarious( QString ) ) );
	connect( FolderView->fsModel, SIGNAL( dirLoading( QString ) ), this, SLOT( updateInfoBar() ) );
=======
	connect( FolderView->fsModel, SIGNAL( dirLoading( QString ) ), this, SLOT( updateVarious( QString ) ) );
	connect( FolderView->fsModel, SIGNAL( dirLoading( QString ) ), this, SLOT( updateInfoBar() ) );
	connect( FolderView->fsModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( updateQuickMenuBar() ) );
	connect( FolderView->fsModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( updateQuickMenuBar() ) );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

	connect( FolderView->widget( 2 ), SIGNAL( openLocation( QString ) ), this, SLOT( openAddress( QString ) ) );

	// Focus AddressBar
	QAction *focusAddressBarAct = new QAction( this );
	focusAddressBarAct->setShortcuts( Settings->Shortcuts.FocusAddressBar );
<<<<<<< HEAD
	connect( focusAddressBarAct, SIGNAL( triggered() ), AddressBar, SLOT( focusAddressEdit() ) );
=======
	connect( focusAddressBarAct, SIGNAL( triggered() ), AddressBar->addressWidget->addressEdit, SLOT( setFocus() ) );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

	// Open new window
	QAction *newWindowAct = new QAction( this );
	newWindowAct->setShortcuts( Settings->Shortcuts.NewWindow );
	connect( newWindowAct, SIGNAL( triggered() ), this, SLOT( openNewWindow() ) );

	// Add bookmark
	addBookMarkAct = new QAction( this );
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

	addAction( focusAddressBarAct );
	addAction( newWindowAct );
	addAction( addBookMarkAct );
	addAction( termWidgetAct );
	addAction( toggleSideBar );
	addAction( viewModeAct );
	addAction( showApplicationsAct );
	addAction( showCatalogsAct );
	addAction( showFoldersAct );
};

<<<<<<< HEAD
void NewBreezeUI::setFocus() {

	FolderView->setFocus();
};

bool NewBreezeUI::activeJobs() {

	if ( not Settings->General.NativeTitleBar )
		return uBar->procWidget->activeJobs();

	else
		return AddressBar->procWidget()->activeJobs();
};

void NewBreezeUI::showActiveJobs() {

	if ( not Settings->General.NativeTitleBar )
		uBar->procWidget->showAllIODialogs();

	else
		AddressBar->procWidget()->showAllIODialogs();
=======
void NewBreezeUI::updateGUI() {

	AddressBar->setStyleSheet( getStyleSheet( "NBTooBar", Settings->General.Style ) );
	SidePanel->setStyleSheet( getStyleSheet( "NBSidePanel", Settings->General.Style ) );
	Terminal->setStyleSheet( getStyleSheet( "NBTerminal", Settings->General.Style ) );
	InfoBar->setStyleSheet( getStyleSheet( "NBInfoBar", Settings->General.Style ) );
};

void NewBreezeUI::setFocus() {

	FolderView->setFocus();
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

void NewBreezeUI::toggleSideBarVisible() {

	if ( SidePanel->isVisible() )
		SidePanel->hide();

	else
		SidePanel->show();

	Settings->setValue( "Session/SidePanel", SidePanel->isVisible() );
};

void NewBreezeUI::addBookMark() {

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

void NewBreezeUI::showHideTermWidget() {

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

void NewBreezeUI::openAddressBar() {

<<<<<<< HEAD
	if ( !QFileInfo( AddressBar->address() ).exists() ) {
		QString text = QString( "There is no file or directory named: "		\
			"<tt><b>%1</b></tt>. Please check the path entered."
		).arg(  AddressBar->address() );

		QString title = QString( "NewBreeze - Invalid Location" );

		NBMessageDialog::error( this, title, text );
=======
	if ( !QFileInfo( AddressBar->addressWidget->addressEdit->text() ).exists() ) {
		QString text = QString( "There is no file or directory named: "		\
			"<tt><b>%1</b></tt>. Please check the path entered."
		).arg(  AddressBar->addressWidget->addressEdit->text() );

		QString title = QString( "NewBreeze - Invalid Location" );

		NBMessageDialog::error( title, text );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

		return;
	}

<<<<<<< HEAD
	FolderView->doOpen( AddressBar->address() );
=======
	FolderView->doOpen( AddressBar->addressWidget->addressEdit->text() );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

void NewBreezeUI::openAddress( QString path ) {

	FolderView->setCurrentIndex( 0 );

	FolderView->doOpen( path );
};

void NewBreezeUI::openNewWindow() {

	QString location = FolderView->fsModel->currentDir();
	qDebug( "Opening new window at %s", qPrintable( location ) );

	emit newWindow( location );
};

<<<<<<< HEAD
void NewBreezeUI::clearSearch() {

	AddressBar->clearSearchBar();
=======
void NewBreezeUI::focusSearch() {

	AddressBar->searchBar->searchLE->setFocus();
};

void NewBreezeUI::clearSearch() {

	AddressBar->searchBar->searchLE->clear();
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	clearFilters();

	FolderView->setFocus();
};

void NewBreezeUI::showProperties() {

	QList<QModelIndex> selectedList = FolderView->getSelection();
	QStringList paths;

	if ( !selectedList.count() )
		paths << FolderView->fsModel->currentDir();

	else
		foreach( QModelIndex idx, selectedList )
			paths << FolderView->fsModel->nodePath( idx );

<<<<<<< HEAD
	NBPropertiesDialog *propsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Properties, &__terminate, this );
=======
	NBPropertiesDialog *propsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Properties, &__terminate );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	propsDlg->show();
};

void NewBreezeUI::showPermissions() {

	QList<QModelIndex> selectedList = FolderView->getSelection();
	QStringList paths;

	if ( !selectedList.count() )
		paths << FolderView->fsModel->currentDir();

	else
		foreach( QModelIndex idx, selectedList )
			paths << FolderView->fsModel->nodePath( idx );

<<<<<<< HEAD
	NBPropertiesDialog *permsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Permissions, &__terminate, this );
=======
	NBPropertiesDialog *permsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Permissions, &__terminate );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	permsDlg->show();
};

void NewBreezeUI::handleDriveUrl( QString url ){

	if ( url.startsWith( "NB://A" ) )
		showApplications();

	else if ( url.startsWith( "NB://C" ) )
		showCatalogs();

	else
		FolderView->doOpen( url );
};

void NewBreezeUI::showApplications() {

	if ( qobject_cast<NBSidePanel*>( sender() ) != SidePanel )
		SidePanel->flashApplications();

	FolderView->doOpen( "NB://Applications" );
<<<<<<< HEAD
	AddressBar->setAddress( "NB://Applications" );
=======
	AddressBar->addressWidget->addressEdit->setText( "NB://Applications" );
	AddressBar->addressWidget->crumbsBar->setCurrentDirectory( "NB://Applications" );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

void NewBreezeUI::showCatalogs() {

	if ( qobject_cast<NBSidePanel*>( sender() ) != SidePanel )
		SidePanel->flashCatalogs();

	FolderView->doOpen( "NB://Catalogs" );
<<<<<<< HEAD
	AddressBar->setAddress( "NB://Catalogs" );
=======
	AddressBar->addressWidget->addressEdit->setText( "NB://Catalogs" );
	AddressBar->addressWidget->crumbsBar->setCurrentDirectory( "NB://Catalogs" );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

void NewBreezeUI::showFolders() {

	if ( qobject_cast<NBSidePanel*>( sender() ) != SidePanel )
		SidePanel->flashFolders();

	FolderView->doOpen( FolderView->fsModel->currentDir() );
};

void NewBreezeUI::showTrash() {

	NBTrashManager *trashManager = new NBTrashManager();
	trashManager->show();
};

void NewBreezeUI::filterFiles( QString filter ) {

	FolderView->fsModel->setNameFilters( QStringList() << QString( "*%1*" ).arg( filter ) );
};

void NewBreezeUI::clearFilters() {

	FolderView->fsModel->setNameFilters( QStringList() );
};

void NewBreezeUI::initiateIO( QStringList sourceList, QString target, NBIOMode::Mode iomode ) {

<<<<<<< HEAD
	if ( not Settings->General.NativeTitleBar )
		uBar->procWidget->addJob( sourceList, target, iomode );

	else
		AddressBar->procWidget()->addJob( sourceList, target, iomode );
=======
	emit addJob( sourceList, target, iomode );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

void NewBreezeUI::openWithList() {

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
<<<<<<< HEAD

void NewBreezeUI::updateVarious( QString url ) {

	QString folder = ( ( url == "/" ) ? QString() : url.split( "/", QString::SkipEmptyParts ).takeLast() );
	setWindowTitle( ( folder.isEmpty() ? "/ - NewBreeze" : QString( "%1 - NewBreeze" ).arg( folder ) ) );

	AddressBar->setAddress( url );

	if ( not url.startsWith( "NB://" ) )
		Terminal->changeDir( url );
};

void NewBreezeUI::updateInfoBar() {

	InfoBar->clear();
	if ( qobject_cast<NBFolderView*>( sender() ) == FolderView ) {
		QModelIndexList selectedIndexes = FolderView->getSelection();
		if ( selectedIndexes.count() == 0 )
			InfoBar->updateInfoBarCF( FolderView->fsModel->currentDir() );

		else if ( selectedIndexes.count() == 1 )
			InfoBar->updateInfoBarSingle( FolderView->fsModel->nodePath( selectedIndexes.at( 0 ) ) );

		else
			InfoBar->updateInfoBarSelection( FolderView->fsModel->currentDir(), selectedIndexes );
	}

	else if ( qobject_cast<NBFileSystemModel*>( sender() ) == FolderView->fsModel )
		InfoBar->updateInfoBarCF( FolderView->fsModel->currentDir() );
};

void NewBreezeUI::changeViewMode() {

	QStringList viewModes = QStringList() << "TilesView" << "IconsView" << "DetailsView";
	Settings->General.ViewMode = viewModes.at( AddressBar->checkedAction() );

	FolderView->updateViewMode();

	QSettings sett( FolderView->fsModel->nodePath( ".directory" ), QSettings::NativeFormat );
	sett.setValue( "NewBreeze/ViewMode", Settings->General.ViewMode );
};

void NewBreezeUI::switchToNextView() {

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

void NewBreezeUI::toggleGrouping() {

	if ( Settings->General.Grouping )
		Settings->General.Grouping = false;

	else
		Settings->General.Grouping = true;

	FolderView->fsModel->setCategorizationEnabled( Settings->General.Grouping );
	FolderView->groupsAct->setChecked( Settings->General.Grouping );

	QSettings sett( FolderView->fsModel->nodePath( ".directory" ), QSettings::NativeFormat );
	sett.setValue( "NewBreeze/Grouping", Settings->General.Grouping );
};
=======
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
