/*
	*
	* NBErrorsDialog.cpp - NBErrorsDialog class
	*
*/

#include <NBErrorsDialog.hpp>

NBErrorsDialog::NBErrorsDialog( QString title, QString text, QStringList nodes, QWidget *parent ) : NBDialog( parent ) {

	mTitle = QString( title );
	mText = QString ( text );

	nodeList << nodes;

	setupGUI();
	setupTable();
}

void NBErrorsDialog::setupGUI() {

	QVBoxLayout *dlgLyt = new QVBoxLayout();
	QHBoxLayout *msgLyt = new QHBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	iconLbl = new QLabel();
	iconLbl->setMaximumSize( QSize( 64, 64 ) );
	iconLbl->setPixmap( QPixmap( ":/icons/error.png" ).scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	textLbl = new QLabel( mText );

	textLbl->setWordWrap( true );

	table = new QTableWidget( 0, 2 );
	table->setFocusPolicy( Qt::NoFocus );

	msgLyt->addWidget( iconLbl );
	msgLyt->addWidget( textLbl );

	okayBtn = new NBButton( QIcon( ":/icons/ok.png" ), "&Ok", this );
	okayBtn->setObjectName( "okBtn" );
	okayBtn->installEventFilter( this );
	connect( okayBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	segBtns = new NBSegmentButton( this );
	segBtns->addSegment( okayBtn );
	segBtns->setSegmentWidth( 100 );

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( segBtns );

	dlgLyt->addLayout( msgLyt );
	dlgLyt->addWidget( table );
	dlgLyt->addWidget( Separator::horizontal() );
	dlgLyt->addLayout( btnLyt );

	NBDialog::setLayout( dlgLyt );

	setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );
	setDialogTitle( mTitle );

	setFixedSize( 480, 480 );
};

void NBErrorsDialog::setupTable() {

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

	Q_FOREACH( QString path, nodeList )
		addEntry( path );
};

void NBErrorsDialog::addEntry( QString path ) {

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

void NBErrorsDialog::keyPressEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == Qt::Key_Escape )
		close();

	else
		QWidget::keyPressEvent( keyEvent );
};

void NBErrorsDialog::close() {

	NBDialog::close();
	emit closed();
};

int NBErrorsDialog::exec() {

	QEventLoop loop;
	connect( this, SIGNAL( closed() ), &loop, SLOT( quit() ) );

	QWidget::show();
	loop.exec();

	return 0;
};
