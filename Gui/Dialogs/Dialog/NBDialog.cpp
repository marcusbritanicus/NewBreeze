/*
	*
	* NBDialog.cpp - NewBreeze Dialog Class
	*
*/

#include <NBDialog.hpp>
#include <NBStyleManager.hpp>

NBDialog::NBDialog( QWidget *parent ) : QWidget( parent ) {

	__accepted = false;

	setupGUI();
	setupDialogProperties();
};

NBDialog::NBDialog( QString btns, QWidget *parent ) : QWidget( parent ) {

	buttons = QString( btns );
	__accepted = false;

	setupGUI();
	setupDialogProperties();
};

NBDialog::NBDialog( Buttons btns, QWidget *parent ) : QWidget( parent ) {

	if ( btns == None )
		buttons = QString();

	else {
		if ( btns & Minimize )
			buttons += "n";

		if ( btns & Maximize )
			buttons += "x";

		if ( btns & Close )
			buttons += "c";
	}

	__accepted = false;

	setupGUI();
	setupDialogProperties();
};

void NBDialog::setupGUI() {

	BodyFrame = new QFrame();
	BodyFrame->setContentsMargins( 0, 0, 0, 0 );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( 5, 5, 5, 5 );

	QHBoxLayout *titleLyt = new QHBoxLayout();

	bool minBtn = false, maxBtn = false, clsBtn = false;

	if ( buttons.count( "n" ) )
		minBtn = true;

	if ( buttons.count( "x" ) )
		maxBtn = true;

	if ( buttons.count( "c" ) )
		clsBtn = true;

	QAction *closeAct = new QAction( "&Close", this );
	closeAct->setShortcut( QKeySequence( Qt::Key_Escape ) );
	connect( closeAct, SIGNAL( triggered() ), this, SLOT( reject() ) );
	addAction( closeAct );

	actBtns = new NBActionButtons( minBtn, maxBtn, clsBtn );
	connect( actBtns, SIGNAL( minimizeWindow() ), this, SLOT( showMinimized() ) );
	connect( actBtns, SIGNAL( maximizeWindow() ), this, SLOT( maxBtnClicked() ) );
	connect( actBtns, SIGNAL( closeWindow() ), this, SLOT( close() ) );

	titleLbl = new QLabel( "<b><h3>NBDialog</h3><b>" );

	titleLyt->addStretch( 0 );
	titleLyt->addWidget( titleLbl );
	titleLyt->addStretch( 0 );
	titleLyt->addWidget( actBtns );

	if ( not Settings->General.NativeTitleBar ) {
		baseLyt->addLayout( titleLyt );
		baseLyt->addWidget( Separator::horizontal() );
	}
	baseLyt->addWidget( BodyFrame );

	QWidget::setLayout( baseLyt );
};

void NBDialog::setupDialogProperties() {

	if ( not Settings->General.NativeTitleBar )
		setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );

	else
		setWindowFlags( Qt::Dialog );

	/* NewBreeze speciality */
	setDialogIcon( QIcon( ":/icons/newbreeze2.png" ) );

	setWindowModality( Qt::ApplicationModal);

	/* Size and Size policy */
	setMinimumSize( 500, 75 );
	setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
};

void NBDialog::setLayout( QLayout *lyt ) {

	BodyFrame->setLayout( lyt );
};

void NBDialog::setDialogTitle( QString title ) {

	titleLbl->setText( QString( "<b><h3>%1</h3></b>" ).arg( title ) );
	setWindowTitle( title );
};

void NBDialog::setDialogIcon( QIcon icon ) {

	setWindowIcon( icon );
};

bool NBDialog::result() {

	return __accepted;
};

void NBDialog::maxBtnClicked() {

	if ( isMaximized() )
		showNormal();

	else
		showMaximized();
};

void NBDialog::mouseDoubleClickEvent( QMouseEvent * ) {

	if ( isMaximized() )
		showNormal();

	else
		showMaximized();
};

void NBDialog::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( mpEvent->button() == Qt::LeftButton ) {
		dragPosition = mpEvent->globalPos() - frameGeometry().topLeft();
		mpEvent->accept();
	}
};

void NBDialog::mouseMoveEvent( QMouseEvent *mmEvent ) {

	if ( mmEvent->buttons() & Qt::LeftButton ) {
		move( mmEvent->globalPos() - dragPosition );
		mmEvent->accept();
	}
};

void NBDialog::paintEvent( QPaintEvent *pEvent ) {

	QWidget::paintEvent( pEvent );

	QPainter *painter = new QPainter( this );
	painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 1.0 ) );
	painter->drawRect( rect().adjusted( +0, +0, -1, -1 ) );

	painter->end();
	pEvent->accept();
};

void NBDialog::done( int result ) {

	if ( result )
		__accepted = false;

	else
		__accepted = true;

	close();
};

void NBDialog::close() {

	QWidget::close();
	emit closed();
};

void NBDialog::show() {

	qDebug() << "Showing dialog";
	QWidget::show();
};

void NBDialog::exec() {

	QEventLoop loop;
	connect( this, SIGNAL( closed() ), &loop, SLOT( quit() ) );

	QWidget::show();
	loop.exec();
};

void NBDialog::accept() {

	__accepted = true;
	close();
};

void NBDialog::reject() {

	__accepted = false;
	close();
};
