/*
	*
	* NBActionButtons.cpp - Minimize/Maximize/Close Button Widget Class
	*
*/

#include <NBActionButtons.hpp>

NBActionButtons::NBActionButtons( bool minBtn, bool maxBtn, bool closeBtn ) : QWidget() {

	minRect = QRect( 0, 0, 0, 0 );
	maxRect = QRect( 0, 0, 0, 0 );
	clsRect = QRect( 0, 0, 0, 0 );

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
