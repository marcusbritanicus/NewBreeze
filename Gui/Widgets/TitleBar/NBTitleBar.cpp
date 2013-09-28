/*
	*
	* NBTitleBar.cpp - Custom TitleBar for NB
	*
*/

#include <NBTools.hpp>
#include <NBTitleBar.hpp>

NBTitleBar::NBTitleBar() : QWidget() {

	setObjectName( tr( "TitleBar") );
	setFixedHeight( 24 );

	isMaximized = true;

	iconLbl = new NBTitleIcon( ":/icons/newbreeze.png" );
	iconLbl->setPixmap( QPixmap() );

	titleLbl = new QLabel();
	titleLbl->setText( QString() );
	titleLbl->setFont( QFont( "Architects Daughter", 10 ) );

	closeBtn = new QToolButton();
	closeBtn->setObjectName( tr( "closeBtn" ) );
	closeBtn->setText( trUtf8( "\u2718" ) );
	connect( closeBtn, SIGNAL( clicked() ), this, SLOT( closeBtnClicked() ) );

	minBtn = new QToolButton();
	minBtn->setObjectName( tr( "minBtn" ) );
	minBtn->setText( trUtf8( "\u25AC" ) );
	connect( minBtn, SIGNAL( clicked() ), this, SLOT( minBtnClicked() ) );

	maxBtn = new QToolButton();
	maxBtn->setObjectName( tr( "maxBtn" ) );
	maxBtn->setText( trUtf8( "\u25A1" ) );
	connect( maxBtn, SIGNAL( clicked() ), this, SLOT( maxBtnClicked() ) );

	closeBtn->setFocusPolicy( Qt::NoFocus );
	minBtn->setFocusPolicy( Qt::NoFocus );
	maxBtn->setFocusPolicy( Qt::NoFocus );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( 0, 2, 0, 0 );

	lyt->addWidget( iconLbl );
	lyt->addStretch( 0 );
	lyt->addWidget( titleLbl );
	lyt->addStretch( 0 );
	lyt->addWidget( minBtn );
	lyt->addWidget( maxBtn );
	lyt->addWidget( closeBtn );

	setLayout( lyt );

	setStyleSheet( getStyleSheet( "NBTitleBar", Settings.General.Style ) );
};

void NBTitleBar::setTitle( QString title ) {

	titleLbl->setText( title );
};

void NBTitleBar::setIcon( QIcon icon ) {

	iconLbl->setPixmap( icon.pixmap( 21, 21, QIcon::Normal, QIcon::Off ) );
};

void NBTitleBar::closeBtnClicked() {

	emit closeWindow();
};

void NBTitleBar::minBtnClicked() {

	emit minimizeWindow();
};

void NBTitleBar::maxBtnClicked() {

	if ( isMaximized )
		emit restoreWindow();
	else
		emit maximizeWindow();
};

void NBTitleBar::mouseDoubleClickEvent( QMouseEvent * ) {

	if ( isMaximized )
		emit restoreWindow();
	else
		emit maximizeWindow();
};

void NBTitleBar::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( mpEvent->button() == Qt::LeftButton ) {
		emit titlebarMousePress( mpEvent );
		mpEvent->accept();
	}
};

void NBTitleBar::mouseMoveEvent( QMouseEvent *mmEvent ) {

	if ( mmEvent->buttons() & Qt::LeftButton ) {
		emit titlebarMouseMove( mmEvent );
		mmEvent->accept();
	}
};
