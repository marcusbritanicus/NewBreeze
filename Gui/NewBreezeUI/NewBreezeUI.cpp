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
	if ( Settings->Session.ShowHidden )
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

	AddressBar = new NBAddressBar();
	QuickMenuBar = new NBQuickMenuBar();
	SidePanel = new NBSidePanel( this );
	FolderView = new NBFolderView();
	InfoBar = new NBInfoBar();
	Terminal = new NBTerminal();

	// Widgets layout
	ViewLayout->addWidget( AddressBar );
	ViewLayout->addWidget( Separator::horizontal() );
	ViewLayout->addWidget( QuickMenuBar );
	ViewLayout->addWidget( FolderView );

	BodyLayout->addWidget( SidePanel );
	BodyLayout->addWidget( Separator::vertical() );
	BodyLayout->addWidget( NBSpacer::horizontal( 3 ) );
	BodyLayout->addLayout( ViewLayout );

	BodyWidget->setLayout( BodyLayout );

	MainLayout->addWidget( BodyWidget );
	MainLayout->addWidget( Terminal );
	MainLayout->addWidget( Separator::horizontal() );
	MainLayout->addWidget( InfoBar );

	setLayout( MainLayout );

	// Widget Properties
	AddressBar->setFixedHeight( 24 );
	QuickMenuBar->setFixedHeight( 24 );
	FolderView->setObjectName( "mainList" );
	Terminal->setFixedHeight( 270 );
	Terminal->hide();

	if ( not Settings->Session.SidePanel )
		SidePanel->hide();

	AddressBar->setFocusPolicy( Qt::NoFocus );
	SidePanel->setFocusPolicy( Qt::NoFocus );
	FolderView->setFocusPolicy( Qt::StrongFocus );
	InfoBar->setFocusPolicy( Qt::NoFocus );

	setFocusPolicy( Qt::NoFocus );
};

void NewBreezeUI::createAndSetupActions() {

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
	connect( FolderView, SIGNAL( focusSearchBar() ), this, SLOT( focusSearch() ) );
	connect( FolderView, SIGNAL( clearSearchBar() ), this, SLOT( clearSearch() ) );
	connect( FolderView, SIGNAL( newWindow( QString ) ), this, SIGNAL( newWindow( QString ) ) );
	connect( FolderView, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ), this, SLOT( updateInfoBar() ) );
	connect( FolderView, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ), this, SLOT( updateQuickMenuBar() ) );
	connect( FolderView, SIGNAL( reloadCatalogs() ), FolderView->widget( 2 ), SLOT( reload() ) );
	connect( FolderView, SIGNAL( toggleGroups() ), this, SLOT( toggleGrouping() ) );
	connect( FolderView, SIGNAL( updateAddressBar( QString ) ), this, SLOT( updateVarious( QString ) ) );

	connect( FolderView->fsModel, SIGNAL( dirLoading( QString ) ), this, SLOT( updateVarious( QString ) ) );
	connect( FolderView->fsModel, SIGNAL( dirLoading( QString ) ), this, SLOT( updateInfoBar() ) );
	connect( FolderView->fsModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( updateQuickMenuBar() ) );
	connect( FolderView->fsModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( updateQuickMenuBar() ) );

	connect( FolderView->widget( 2 ), SIGNAL( openLocation( QString ) ), this, SLOT( openAddress( QString ) ) );

	// Focus AddressBar
	QAction *focusAddressBarAct = new QAction( this );
	focusAddressBarAct->setShortcuts( Settings->Shortcuts.FocusAddressBar );
	connect( focusAddressBarAct, SIGNAL( triggered() ), AddressBar->addressWidget->addressEdit, SLOT( setFocus() ) );

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

void NewBreezeUI::updateGUI() {

	AddressBar->setStyleSheet( getStyleSheet( "NBTooBar", Settings->General.Style ) );
	SidePanel->setStyleSheet( getStyleSheet( "NBSidePanel", Settings->General.Style ) );
	Terminal->setStyleSheet( getStyleSheet( "NBTerminal", Settings->General.Style ) );
	InfoBar->setStyleSheet( getStyleSheet( "NBInfoBar", Settings->General.Style ) );
};

void NewBreezeUI::setFocus() {

	FolderView->setFocus();
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

	if ( !QFileInfo( AddressBar->addressWidget->addressEdit->text() ).exists() ) {
		QString text = QString( "There is no file or directory named: "		\
			"<tt><b>%1</b></tt>. Please check the path entered."
		).arg(  AddressBar->addressWidget->addressEdit->text() );

		QString title = QString( "NewBreeze - Invalid Location" );

		NBMessageDialog::error( title, text );

		return;
	}

	FolderView->doOpen( AddressBar->addressWidget->addressEdit->text() );
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

void NewBreezeUI::focusSearch() {

	AddressBar->searchBar->searchLE->setFocus();
};

void NewBreezeUI::clearSearch() {

	AddressBar->searchBar->searchLE->clear();
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

	NBPropertiesDialog *propsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Properties, &__terminate );
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

	NBPropertiesDialog *permsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Permissions, &__terminate );
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
	AddressBar->addressWidget->addressEdit->setText( "NB://Applications" );
	AddressBar->addressWidget->crumbsBar->setCurrentDirectory( "NB://Applications" );
};

void NewBreezeUI::showCatalogs() {

	if ( qobject_cast<NBSidePanel*>( sender() ) != SidePanel )
		SidePanel->flashCatalogs();

	FolderView->doOpen( "NB://Catalogs" );
	AddressBar->addressWidget->addressEdit->setText( "NB://Catalogs" );
	AddressBar->addressWidget->crumbsBar->setCurrentDirectory( "NB://Catalogs" );
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

	emit addJob( sourceList, target, iomode );
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
