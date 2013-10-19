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

	NBActionButtons *actBtns = new NBActionButtons( true, true, true );
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

NBActionButtons::NBActionButtons( bool minBtn, bool maxBtn, bool closeBtn ) : QWidget() {

	painter = new QPainter();

	minBtnEnabled = minBtn;
	maxBtnEnabled = maxBtn;
	closeBtnEnabled = closeBtn;

	widgetWidth = 0;
	int startx = 5;

	if ( minBtn ) {
		widgetWidth += 16 + 5;
		minRect = QRect( startx, 2, 16, 16 );
		startx += 16 + 5;
	}

	if ( maxBtn ) {
		widgetWidth += 16 + 5;
		maxRect = QRect( startx, 2, 16, 16 );
		startx += 16 + 5;
	}

	if ( closeBtn ) {
		widgetWidth += 16 + 5;
		clsRect = QRect( startx, 2, 16, 16 );
	}

	if ( widgetWidth )
		widgetWidth += 5;

	setFixedSize( widgetWidth, 20 );

	setMouseTracking( true );
};

NBActionButtons::~NBActionButtons() {

	if ( painter->isActive() )
		painter->end();

	delete painter;
};

void NBActionButtons::mousePressEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		if ( minRect.contains( mEvent->pos() ) )
			emit minimizeWindow();

		else if ( maxRect.contains( mEvent->pos() ) )
			emit maximizeWindow();

		else if ( clsRect.contains( mEvent->pos() ) )
			emit closeWindow();

		else
			QWidget::mousePressEvent( mEvent );
	}

	else {
		QWidget::mousePressEvent( mEvent );
	}

	mEvent->accept();
};

void NBActionButtons::mouseMoveEvent( QMouseEvent *mEvent ) {

	repaint();
	mEvent->accept();
};

void NBActionButtons::paintEvent( QPaintEvent *pEvent ) {

	if ( not widgetWidth ) {
		pEvent->ignore();
		return;
	}

	painter->begin( this );
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( Qt::NoPen );

	QPoint mousePos = mapFromGlobal( QCursor::pos() );

	int startx = 5;
	if ( minBtnEnabled ) {
		if ( minRect.contains( mousePos ) ) {
			painter->setBrush( QBrush( QColor( 0, 100, 0, 180 ) ) );
			painter->drawRoundedRect( startx -1, 1, 18, 18, 3, 3 );
			painter->setBrush( Qt::NoBrush );
		}
		QPixmap minIcon = QIcon( ":/icons/arrow-down.png" ).pixmap( 16, 16 );
		painter->drawPixmap( startx, 2, 16, 16, minIcon );
		startx += 16 + 5;
	}

	if ( maxBtnEnabled ) {
		if ( maxRect.contains( mousePos ) ) {
			painter->setBrush( QBrush( QColor( 0, 0, 100, 180 ) ) );
			painter->drawRoundedRect( startx -1, 1, 18, 18, 3, 3 );
			painter->setBrush( Qt::NoBrush );
		}
		QPixmap maxIcon = QIcon( ":/icons/arrow-up.png" ).pixmap( 16, 16 );
		painter->drawPixmap( startx, 2, 16, 16, maxIcon );
		startx += 16 + 5;
	}

	if ( closeBtnEnabled ) {
		if ( clsRect.contains( mousePos ) ) {
			painter->setBrush( QBrush( QColor( 100, 0, 0, 180 ) ) );
			painter->drawRoundedRect( startx -1, 1, 18, 18, 3, 3 );
			painter->setBrush( Qt::NoBrush );
		}
		QPixmap closeIcon = QIcon( ":/icons/delete.png" ).pixmap( 16, 16 );
		painter->drawPixmap( startx, 2, 16, 16, closeIcon );
	}

	painter->end();
	pEvent->accept();
};
