/*
	*
	* NBConfirmDeleteDialog.cpp - NBConfirmDeleteDialog class
	*
*/

#include <NBConfirmDeleteDialog.hpp>
#include <NBGuiWidgets.hpp>
#include <NBTools.hpp>
#include <NBIconProvider.hpp>

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
			"<p>Do you really want to delete the selected item(s) permanently? You cannot retieve them once deleted.</p>" \
			"<p>To see the list of files you are about to delete, press 'More..'</p>"
		);
	else
		textLbl = new QLabel(
			"<p>Do you really want to send the selected item(s) to trash?</p>" \
			"<p>To see the list of files you are about to trash, press 'More..'</p>"
		);

	textLbl->setWordWrap( true );

	table = new QTableWidget( 0, 2 );
	table->setFocusPolicy( Qt::NoFocus );
	table->hide();

	setTitle( "Delete Files?" );

	msgLyt->addWidget( iconLbl );
	msgLyt->addWidget( textLbl );

	deleteBtn = new QPushButton( QIcon( ":/icons/delete.png" ), "&Delete" );
	deleteBtn->setObjectName( "abortBtn" );
	connect( deleteBtn, SIGNAL( clicked() ), this, SLOT( deleteOk() ) );

	cancelBtn = new QPushButton( QIcon( ":/icons/cancel.png" ), "&Cancel" );
	cancelBtn->setObjectName( "cancelBtn" );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( deleteCancel() ) );

	detailsBtn = new QPushButton( QIcon( ":/icons/info.png" ), "&More..." );
	connect( detailsBtn, SIGNAL( clicked() ), this, SLOT( handleDetailsBtnClicked() ) );

	btnLyt->addWidget( detailsBtn );
	btnLyt->addStretch( 0 );
	btnLyt->addWidget( deleteBtn );
	btnLyt->addWidget( cancelBtn );

	dlgLyt->addLayout( msgLyt );
	dlgLyt->addWidget( table );
	dlgLyt->addWidget( Separator::horizontal() );
	dlgLyt->addLayout( btnLyt );

	NBDialog::setLayout( dlgLyt );

	setFixedSize( 480, 200 );

	table->setStyleSheet( getStyleSheet( "NBConfirmDeleteDialog", settings.value( "Style" ).toString() ) );
	cancelBtn->setFocus();
};

void NBConfirmDeleteDialog::setupTable() {

	table->verticalHeader()->hide();
	table->setHorizontalHeaderLabels( QStringList() << "File Name" << "Size" );

	table->setShowGrid( false );
	table->setSelectionBehavior( QAbstractItemView::SelectRows );

	QHeaderView *headerView = new QHeaderView( Qt::Horizontal, table );
	table->setHorizontalHeader( headerView );
	headerView->setResizeMode( 0, QHeaderView::Stretch );
	headerView->setResizeMode( 1, QHeaderView::Fixed );

	table->setColumnWidth( 1, 100 );
	table->verticalHeader()->setResizeMode( QHeaderView::Fixed );

	foreach( QString path, deletePaths ) {
		addEntry( path );
	}
};

void NBConfirmDeleteDialog::setTitle( QString title ) {

	NBDialog::setDialogTitle( title );
};

void NBConfirmDeleteDialog::addEntry( QString path ) {

	QString iconName = NBIconProvider::icon( path );

	QTableWidgetItem *itm1 = new QTableWidgetItem( QIcon::fromTheme( iconName, QIcon( iconName ) ), path );
	QTableWidgetItem *itm2 = new QTableWidgetItem( formatSize( getSize( path ) ) );

	itm1->setFlags( itm1->flags() & ~Qt::ItemIsEditable );
	itm2->setFlags( itm2->flags() & ~Qt::ItemIsEditable );

	table->insertRow( table->rowCount() );

	table->setItem( table->rowCount() - 1, 0, itm1 );
	table->setItem( table->rowCount() - 1, 1, itm2 );
}

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

void NBConfirmDeleteDialog::handleDetailsBtnClicked() {

	if ( table->isVisible() ) {
		detailsBtn->setText( "&More..." );
		table->hide();

		QDesktopWidget dw;
		int hpos = ( int )( ( dw.width() - 480 ) / 2 );
		int vpos = ( int )( ( dw.height() - 200 ) / 2 );
		move( hpos, vpos );

		setFixedSize( 480, 200 );
	}

	else {
		detailsBtn->setText( "&Less..." );
		table->show();

		QDesktopWidget dw;
		int hpos = ( int )( ( dw.width() - 480 ) / 2 );
		int vpos = ( int )( ( dw.height() - 480 ) / 2 );
		move( hpos, vpos );

		setFixedSize( 480, 480 );
	}
};
