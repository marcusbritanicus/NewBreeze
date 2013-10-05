/*
	*
	* NBTitleBar.cpp - Custom TitleBar for NB
	*
*/

#include <NBTools.hpp>
#include <NBTitleBar.hpp>

NBTitleBar::NBTitleBar() : QWidget() {

	painter = new QPainter();

	setObjectName( tr( "TitleBar") );
	setFixedHeight( 24 );

	isMaximized = true;

	m_icon = QIcon( ":/icons/newbreeze.png" );
	m_title = QString( "NewBreeze" );

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

	lyt->addStretch( 0 );
	lyt->addWidget( minBtn );
	lyt->addWidget( maxBtn );
	lyt->addWidget( closeBtn );

	setLayout( lyt );

	setStyleSheet( getStyleSheet( "NBTitleBar", Settings.General.Style ) );

	QAction *aboutNBAct = new QAction( "About NewBreeze", this );
	aboutNBAct->setShortcuts( Settings.Shortcuts.AboutNB );
	connect( aboutNBAct, SIGNAL( triggered() ), this, SIGNAL( aboutNB() ) );

	QAction *aboutQt4Act = new QAction( "About Qt4", this );
	aboutQt4Act->setShortcuts( Settings.Shortcuts.AboutQt );
	connect( aboutQt4Act, SIGNAL( triggered() ), this, SIGNAL( aboutQt4() ) );
};

NBTitleBar::~NBTitleBar() {

	if ( painter->isActive() )
		painter->end();

	delete painter;
};

void NBTitleBar::setTitle( QString title ) {

	m_title = title;
	repaint();
};

void NBTitleBar::setIcon( QIcon icon ) {

	m_icon = icon;
	repaint();
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

void NBTitleBar::paintEvent( QPaintEvent *pEvent ) {

	painter->begin( this );
	painter->setRenderHints( QPainter::Antialiasing );

	// We leave 5 pixels each on the top bottom and left
	// Vertically we need to drop 7 to make it appear VCentered
	QPixmap iconPix = m_icon.pixmap( 20, 20 );
	painter->drawPixmap( 5, 7, iconPix );

	painter->drawText( 0, 0, width(), height(), Qt::AlignCenter, m_title );

	painter->end();
	pEvent->accept();
};
