/*
	*
	* NBDialog.cpp - NBDialog Class
	*
*/

#include <QtGui>
#include <QtCore>

#include <NBDialog.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>

using namespace std;

NBDialog::NBDialog( QString btns ) : QWidget() {

	buttons = QString( btns );
	__newClosed = false;
	__accepted = false;

	setupGUI();
	setupDialogProperties();
};

void NBDialog::setupGUI() {

	QWidget *BaseWidget = new QWidget();
	BaseWidget->setObjectName( tr( "guiBase" ) );

	BodyFrame = new QFrame();
	BodyFrame->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	QHBoxLayout *titleLyt = new QHBoxLayout();

	if ( buttons.count( "c" ) ) {
		closeBtn = new QToolButton();
		closeBtn->setObjectName( tr( "closeBtn" ) );
		closeBtn->setText( trUtf8( "\u2718" ) );
		closeBtn->setFocusPolicy( Qt::NoFocus );
		connect( closeBtn, SIGNAL( clicked() ), this, SLOT( close() ) );
	}

	if ( buttons.count( "n" ) ) {
		minBtn = new QToolButton();
		minBtn->setObjectName( tr( "minBtn" ) );
		minBtn->setText( trUtf8( "\u25AC" ) );
		minBtn->setFocusPolicy( Qt::NoFocus );
		connect( minBtn, SIGNAL( clicked() ), this, SLOT( showMinimized() ) );
	}

	if ( buttons.count( "x" ) ) {
		maxBtn = new QToolButton();
		maxBtn->setObjectName( tr( "maxBtn" ) );
		maxBtn->setText( trUtf8( "\u25A1" ) );
		maxBtn->setFocusPolicy( Qt::NoFocus );
		connect( maxBtn, SIGNAL( clicked() ), this, SLOT( maxBtnClicked() ) );
	}

	titleLbl = new QLabel( "<b><h3>NBDialog</h3><b>" );

	titleLyt->addStretch( 0 );
	titleLyt->addWidget( titleLbl );
	titleLyt->addStretch( 0 );

	if ( buttons.count( "n" ) )
		titleLyt->addWidget( minBtn );

	if ( buttons.count( "x" ) )
		titleLyt->addWidget( maxBtn );

	if ( buttons.count( "c" ) )
		titleLyt->addWidget( closeBtn );

	if ( not Settings->General.NativeTitleBar ) {
		baseLyt->addLayout( titleLyt );
		// baseLyt->addWidget( Separator::horizontal() );
	}
	baseLyt->addWidget( BodyFrame );

	BaseWidget->setLayout( baseLyt );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( 0, 0, 0, 0 );
	lyt->addWidget( BaseWidget );

	QWidget::setLayout( lyt );
};

void NBDialog::setupDialogProperties() {

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	if ( not Settings->General.NativeTitleBar )
		setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );

	else
		setWindowFlags( Qt::Dialog );

	setWindowModality( Qt::ApplicationModal);

	setMinimumSize( 600, 200 );

	setStyleSheet( getStyleSheet( "NBDialog", Settings->General.Style ) );
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

void NBDialog::keyPressEvent( QKeyEvent *keyEvent ) {

	 if ( keyEvent->key() == Qt::Key_Escape )
		close();

	else
		QWidget::keyPressEvent( keyEvent );
};

void NBDialog::closeEvent( QCloseEvent *cEvent ) {

	if ( __newClosed )
		cEvent->accept();

	else
		cEvent->ignore();
};

void NBDialog::done( int result ) {

	Q_UNUSED( result );
	close();
};

void NBDialog::close() {

	__newClosed = true;

	QWidget::close();
	emit closed();
};

void NBDialog::show() {

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
