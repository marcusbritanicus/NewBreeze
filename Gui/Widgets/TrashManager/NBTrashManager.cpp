/*
	*
	* NBTrashManager.cpp - NewBreeze FolderView Class
	*
*/

#include <NBTrashManager.hpp>

NBTrashManager::NBTrashManager() : NBDialog( "nxc" ) {

	ClipBoard = qApp->clipboard();

	setupGui();
	setDialogProperties();
};

void NBTrashManager::setupGui() {

	// Set Data Model
	tModel = new NBTrashModel();

	// Setup the view
	TrashView = new NBTrashView( tModel );

	// Buttons
	NBButtons *trashButtons = new NBButtons( QStringList() << "&Restore" << "Restore &All" << "&Delete" );
	NBButtons *emptyButtons = new NBButtons( QStringList() << "&Empty Trash" );

	connect( trashButtons->segment( 0 ), SIGNAL( clicked() ), this, SLOT( restoreSelected() ) );
	connect( trashButtons->segment( 1 ), SIGNAL( clicked() ), this, SLOT( restoreAll() ) );
	connect( trashButtons->segment( 2 ), SIGNAL( clicked() ), this, SLOT( deleteSelected() ) );
	connect( emptyButtons->segment( 0 ), SIGNAL( clicked() ), this, SLOT( emptyTrash() ) );

	// Layouts
	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->setContentsMargins( QMargins() );
	btnLyt->addWidget( trashButtons );
	btnLyt->addStretch();
	btnLyt->addWidget( emptyButtons );

	QVBoxLayout *widgetLyt = new QVBoxLayout();
	widgetLyt->setContentsMargins( QMargins() );
	widgetLyt->addLayout( btnLyt );
	widgetLyt->addWidget( TrashView );

	QWidget *widget = new QWidget();
	widget->setLayout( widgetLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->addWidget( widget );
	setLayout( baseLyt );

	// Create and setup actions like copy, move, rename, etc, etc
	createAndSetupActions();

	// Do not accept focus
	setFocusPolicy( Qt::NoFocus );
};

void NBTrashManager::setDialogProperties() {

	setDialogTitle( "NewBreeze Trash Manager" );
	setDialogIcon( QIcon( ":/icons/newbreeze2.png" ) );

	setMinimumSize( QSize( 900, 600 ) );
};

void NBTrashManager::createAndSetupActions() {

	connect( tModel, SIGNAL( restored( QModelIndexList ) ), this, SLOT( handleFailedRestore( QModelIndexList ) ) );

	reloadAct = new QAction( "Re&load", this );
	reloadAct->setShortcuts( NBSettings::instance()->Shortcuts.Reload );
	connect( reloadAct, SIGNAL( triggered() ), this, SLOT( doReload() ) );

	restoreAct = new QAction( "&Restore Selected", this );
	connect( restoreAct, SIGNAL( triggered() ), this, SLOT( restoreSelected() ) );

	restoreAllAct = new QAction( "Res&tore All", this );
	connect( restoreAllAct, SIGNAL( triggered() ), this, SLOT( restoreAll() ) );

	deleteAct = new QAction( "&Delete Selected", this );
	connect( deleteAct, SIGNAL( triggered() ), this, SLOT( deleteSelected() ) );

	emptyAct = new QAction( "&Empty Trash", this );
	connect( emptyAct, SIGNAL( triggered() ), this, SLOT( emptyTrash() ) );

	addAction( reloadAct );
	addAction( restoreAct );
	addAction( restoreAllAct );
	addAction( deleteAct );
	addAction( emptyAct );
};

bool NBTrashManager::hasSelection() {

	return TrashView->selectionModel()->hasSelection();
};

QModelIndexList NBTrashManager::getSelection() {

	QModelIndexList selectedList;
	selectedList << TrashView->selectionModel()->selectedIndexes();
	Q_FOREACH( QModelIndex idx, selectedList )
		if ( idx.column() )
			selectedList.removeAt( selectedList.indexOf( idx ) );

	return selectedList;
};

void NBTrashManager::doReload() {

	tModel->reload();
};

void NBTrashManager::handleFailedRestore( QModelIndexList failedIndexes ) {

	if ( not failedIndexes.count() )
		return;

	QTableWidget *table = new QTableWidget( 0, 2 );
	table->setFocusPolicy( Qt::NoFocus );

	table->verticalHeader()->hide();
	table->setHorizontalHeaderLabels( QStringList() << "File Name" << "Deleteion Date" );

	table->setShowGrid( false );
	table->setSelectionBehavior( QAbstractItemView::SelectRows );

	QHeaderView *headerView = new QHeaderView( Qt::Horizontal, table );
	table->setHorizontalHeader( headerView );
	#if QT_VERSION >= 0x050000
		headerView->setSectionResizeMode( 0, QHeaderView::Stretch );
		headerView->setSectionResizeMode( 1, QHeaderView::Fixed );
		table->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
	#else
		headerView->setResizeMode( 0, QHeaderView::Stretch );
		headerView->setResizeMode( 1, QHeaderView::Fixed );
		table->verticalHeader()->setResizeMode( QHeaderView::Fixed );
	#endif

	table->setColumnWidth( 1, 200 );

	QStringList dirs, files;
	Q_FOREACH( QModelIndex idx, failedIndexes ) {
		NBTrashNode *node = static_cast<NBTrashNode*>( idx.internalPointer() );
		QTableWidgetItem *itm1 = new QTableWidgetItem( node->icon(), node->name() );
		QTableWidgetItem *itm2 = new QTableWidgetItem( node->deletionDate().toString( "MMM dd, yyyy hh:mm:ss" ) );

		itm1->setFlags( itm1->flags() & ~Qt::ItemIsEditable );
		itm2->setFlags( itm2->flags() & ~Qt::ItemIsEditable );

		table->insertRow( table->rowCount() );

		table->setItem( table->rowCount() - 1, 0, itm1 );
		table->setItem( table->rowCount() - 1, 1, itm2 );
	}

	NBMessageDialog::error(
		"NewBreeze - Error while restoring",
		"Some errors were encountered while restoring the files and folders from the trash. "			\
		"As a result, some of the files and folders have not have been restored. For the "				\
		"list for files and folders not restored click <tt><u>M</u>ore</tt>.",
		QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok, table
	);
};

void NBTrashManager::restoreSelected() {

	if ( hasSelection() )
		tModel->restore( getSelection() );
};

void NBTrashManager::restoreAll() {

	QModelIndexList selection;
	for( int row = 0; row < tModel->rowCount(); row++ )
		selection << tModel->index( row, 0 , TrashView->rootIndex() );

	tModel->restore( selection );
};

void NBTrashManager::deleteSelected() {

	if ( hasSelection() )
		tModel->removeFromDisk( getSelection() );
};

void NBTrashManager::emptyTrash() {

	QModelIndexList selection;
	for( int row = 0; row < tModel->rowCount(); row++ )
		selection << tModel->index( row, 0 , TrashView->rootIndex() );

	tModel->removeFromDisk( selection );
};
