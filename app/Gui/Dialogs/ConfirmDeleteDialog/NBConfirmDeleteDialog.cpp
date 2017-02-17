/*
	*
	* NBConfirmDeleteDialog.cpp - NBConfirmDeleteDialog class
	*
*/

#include "NBConfirmDeleteDialog.hpp"

NBConfirmDeleteDialog::NBConfirmDeleteDialog( QString src, QStringList paths, bool permanent, QWidget *parent ) : NBDialog( parent ) {

	deletePaths = paths;
	source = src;

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

	if ( permanent ) {
		textLbl = new QLabel(
			QString(
				"<p>You are about to delete the below listed files/directories from</p>"
				"<center><b><tt>%1</tt></b></center>"
				"<p>Items once deleted cannot be retrieved. Do you wan't to continue?</p>"
			).arg( source )
		);
	}

	else {
		textLbl = new QLabel(
			QString(
				"<p>You are about to trash the below listed files/directories from</p>"
				"<center><b><tt>%1</tt></b></center>"
				"<p>Do you wan't to continue?</p>"
			).arg( source )
		);
	}

	textLbl->setWordWrap( true );

	table = new QTableWidget( 0, 2 );
	table->setFocusPolicy( Qt::NoFocus );

	setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );
	setDialogTitle( "Delete Files?" );

	msgLyt->addWidget( iconLbl );
	msgLyt->addWidget( textLbl );

	segBtns = new NBSegmentButton( this );
	segBtns->setCount( 2 );

	segBtns->setSegmentIcon( 0, QIcon( ":/icons/delete.png" ) );
	segBtns->setSegmentText( 0, tr( "&Delete" ) );
	segBtns->segment( 0 )->setObjectName( "abortBtn" );

	segBtns->setSegmentIcon( 1, QIcon( ":/icons/cancel.png" ) );
	segBtns->setSegmentText( 1, tr( "&Cancel" ) );
	segBtns->segment( 1 )->setObjectName( "cancelBtn" );
	segBtns->segment( 1 )->setFocus();

	connect( segBtns, SIGNAL( clicked( int ) ), this, SLOT( handleSegmentClick( int ) ) );

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( segBtns );

	dlgLyt->addLayout( msgLyt );
	dlgLyt->addWidget( table );
	dlgLyt->addWidget( Separator::horizontal() );
	dlgLyt->addLayout( btnLyt );

	NBDialog::setLayout( dlgLyt );

	setFixedSize( 480, 480 );
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

void NBConfirmDeleteDialog::handleSegmentClick( int seg ) {

	switch( seg ) {
		case 0:
			return deleteOk();

		case 1:
			return deleteCancel();
	}
};

void NBConfirmDeleteDialog::deleteOk() {

	deleteFiles = true;
	close();
};

void NBConfirmDeleteDialog::deleteCancel() {

	deleteFiles = false;
	close();
};

NBDeleteErrorsDialog::NBDeleteErrorsDialog( QStringList files, QStringList dirs, QWidget *parent ) : NBDialog( parent ) {

	fileList << files;
	dirList << dirs;

	setupGUI();
	setupTable();
}

void NBDeleteErrorsDialog::setupGUI() {

	QVBoxLayout *dlgLyt = new QVBoxLayout();
	QHBoxLayout *msgLyt = new QHBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	iconLbl = new QLabel();
	iconLbl->setMaximumSize( QSize( 64, 64 ) );
	iconLbl->setPixmap( QPixmap( ":/icons/question.png" ).scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	textLbl = new QLabel(
		"Some errors were encountered while deleting the files and folders you requested. "
		"As a result, some of the files and folders may not have been deleted. For the "
		"list for files and folder not deleted click <u>M</u>ore"
	);

	textLbl->setWordWrap( true );

	table = new QTableWidget( 0, 2 );
	table->setFocusPolicy( Qt::NoFocus );

	msgLyt->addWidget( iconLbl );
	msgLyt->addWidget( textLbl );

	segBtns = new NBSegmentButton( this );
	segBtns->setCount( 1 );

	segBtns->setSegmentIcon( 0, QIcon( ":/icons/ok.png" ) );
	segBtns->setSegmentText( 0, tr( "&Okay" ) );
	segBtns->setSegmentDisabled( 0 );

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( segBtns );

	dlgLyt->addLayout( msgLyt );
	dlgLyt->addWidget( table );
	dlgLyt->addWidget( Separator::horizontal() );
	dlgLyt->addLayout( btnLyt );

	NBDialog::setLayout( dlgLyt );

	setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );
	setDialogTitle( "NewBreeze - Error while deleting" );

	setFixedSize( 480, 480 );
};

void NBDeleteErrorsDialog::setupTable() {

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

	foreach( QString path, dirList ) {
		addEntry( path );
	}

	foreach( QString path, fileList ) {
		addEntry( path );
	}
};

void NBDeleteErrorsDialog::addEntry( QString path ) {

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

void NBDeleteErrorsDialog::keyPressEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == Qt::Key_Escape )
		close();

	else
		QWidget::keyPressEvent( keyEvent );
};

void NBDeleteErrorsDialog::close() {

	NBDialog::close();
	emit closed();
};

int NBDeleteErrorsDialog::exec() {

	QEventLoop loop;
	connect( this, SIGNAL( closed() ), &loop, SLOT( quit() ) );

	QWidget::show();
	loop.exec();

	return 0;
};
