/*
	*
	* NBTitleBar.cpp - NewBreeze TitleBar Class
	*
*/

#include <NBTools.hpp>
#include <NBTitleBar.hpp>

NBTitleBar::NBTitleBar() : QWidget() {

	painter = new QPainter();

	setObjectName( tr( "TitleBar") );
	setFixedHeight( 24 );

	isMaximized = true;

	m_icon = QIcon( ":/icons/newbreeze2.png" );
	m_title = QString( "NewBreeze" );

	actBtns = new NBActionButtons( true, true, true );
	connect( actBtns, SIGNAL( minimizeWindow() ), this, SIGNAL( minimizeWindow() ) );
	connect( actBtns, SIGNAL( maximizeWindow() ), this, SLOT( maxBtnClicked() ) );
	connect( actBtns, SIGNAL( closeWindow() ), this, SIGNAL( closeWindow() ) );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( 0, 2, 0, 0 );

	lyt->addStretch( 0 );
	lyt->addWidget( actBtns );

	setLayout( lyt );

	setStyleSheet( getStyleSheet( "NBTitleBar", Settings->General.Style ) );
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

	QWidget::mouseMoveEvent( mmEvent );
};

void NBTitleBar::paintEvent( QPaintEvent *pEvent ) {

	painter->begin( this );
	painter->setRenderHints( QPainter::Antialiasing );

	// We leave 5 pixels each on the top bottom and left
	// Vertically we need to drop 7 to make it appear VCentered
	QPixmap iconPix = m_icon.pixmap( 20, 20 );
	painter->drawPixmap( 5, 7, iconPix );

	painter->drawText( 0, 2, width(), height(), Qt::AlignCenter, m_title );

	painter->end();
	pEvent->accept();
};
