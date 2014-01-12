/*
	*
	* NBConfirmDeleteDialog.cpp - NBConfirmDeleteDialog class
	*
*/

#include <NBConfirmDeleteDialog.hpp>

NBConfirmDeleteDialog::NBConfirmDeleteDialog( QStringList paths, bool permanent ) : NBDialog() {

	deletePaths = paths;

	setupGUI( permanent );
	setupTable();
}
void NBConfirmDeleteDialog::setupGUI( bool permanent ) {

	QVBoxLayout *dlgLyt = new QVBoxLayout();
	QHBoxLayout *msgLyt = new QHBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	iconLbl = new QLabel();
	iconLbl->setMaximumSize( QSize( 64, 64 ) );
	iconLbl->setPixmap( QPixmap( ":/icons/question.png" ).scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	if ( permanent )
		textLbl = new QLabel(
			QString(
				"<p>You are about to delete the below listed files/directories from</p>"
				"<center><b><tt>%1</tt></b></center>"
				"<p>Items once deleted cannot be retrieved. Do you wan't to continue?</p>"
			).arg( dirName( deletePaths.at( 0 ) ) )
		);
	else
		textLbl = new QLabel(
			QString(
				"<p>You are about to trash the below listed files/directories from</p>"
				"<center><b><tt>%1</tt></b></center>"
				"<p>Do you wan't to continue?</p>"
			).arg( dirName( deletePaths.at( 0 ) ) )
		);

	textLbl->setWordWrap( true );

	table = new QTableWidget( 0, 2 );
	table->setFocusPolicy( Qt::NoFocus );

	setDialogIcon( QIcon( ":/icons/newbreeze2.png" ) );
	setDialogTitle( "Delete Files?" );

	msgLyt->addWidget( iconLbl );
	msgLyt->addWidget( textLbl );

	deleteBtn = new QPushButton( QIcon( ":/icons/delete.png" ), "&Delete" );
	deleteBtn->setObjectName( "abortBtn" );
	connect( deleteBtn, SIGNAL( clicked() ), this, SLOT( deleteOk() ) );

	cancelBtn = new QPushButton( QIcon( ":/icons/cancel.png" ), "&Cancel" );
	cancelBtn->setObjectName( "cancelBtn" );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( deleteCancel() ) );

	segBtns = new NBButtons();
	segBtns->addSegment( deleteBtn );
	segBtns->addSegment( cancelBtn );
	segBtns->setSegmentHeight( 24 );

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( segBtns );

	dlgLyt->addLayout( msgLyt );
	dlgLyt->addWidget( table );
	dlgLyt->addWidget( Separator::horizontal() );
	dlgLyt->addLayout( btnLyt );

	NBDialog::setLayout( dlgLyt );

	setFixedSize( 480, 480 );

	cancelBtn->setFocus();
};

void NBConfirmDeleteDialog::setupTable() {

	table->verticalHeader()->hide();
	table->setHorizontalHeaderLabels( QStringList() << "File Name" << "Size" );

	table->setShowGrid( false );
	table->setSelectionBehavior( QAbstractItemView::SelectRows );

	QHeaderView *headerView = new QHeaderView( Qt::Horizontal, table );
	table->setHorizontalHeader( headerView );
	#if QT_VERSION >= 0x050000
		headerView->setSectionResizeMode( 0, QHeaderView::Stretch );
		headerView->setSectionResizeMode( 1, QHeaderView::Fixed );
	#else
		headerView->setResizeMode( 0, QHeaderView::Stretch );
		headerView->setResizeMode( 1, QHeaderView::Fixed );
	#endif

	table->setColumnWidth( 1, 100 );
	#if QT_VERSION >= 0x050000
		table->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
	#else
		table->verticalHeader()->setResizeMode( QHeaderView::Fixed );
	#endif

	foreach( QString path, deletePaths ) {
		addEntry( path );
	}
};

void NBConfirmDeleteDialog::addEntry( QString path ) {

	QString iconName = NBIconProvider::icon( path );

	QTableWidgetItem *itm1 = new QTableWidgetItem( QIcon::fromTheme( iconName, QIcon( iconName ) ), baseName( path ) );
	QTableWidgetItem *itm2;
	if ( isDir( path ) )
		itm2 = new QTableWidgetItem( QString( "%1 items" ).arg( nChildren( path ) ) );

	else
		itm2 = new QTableWidgetItem( QString( "%1" ).arg( formatSize( getSize( path ) ) ) );

	itm1->setFlags( itm1->flags() & ~Qt::ItemIsEditable );
	itm2->setFlags( itm2->flags() & ~Qt::ItemIsEditable );

	table->insertRow( table->rowCount() );

	table->setItem( table->rowCount() - 1, 0, itm1 );
	table->setItem( table->rowCount() - 1, 1, itm2 );
};

void NBConfirmDeleteDialog::keyPressEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == Qt::Key_Escape )
		deleteCancel();

	else
		QWidget::keyPressEvent( keyEvent );
};

void NBConfirmDeleteDialog::close() {

	NBDialog::close();
	emit closed();
};

int NBConfirmDeleteDialog::exec() {

	QEventLoop loop;
	connect( this, SIGNAL( closed() ), &loop, SLOT( quit() ) );

	QWidget::show();
	loop.exec();

	return deleteFiles;
};

void NBConfirmDeleteDialog::deleteOk() {

	deleteFiles = true;
	close();
};

void NBConfirmDeleteDialog::deleteCancel() {

	deleteFiles = false;
	close();
};
