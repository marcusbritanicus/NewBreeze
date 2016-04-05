/*
	*
	* NBWindowButton.cpp - A stylish minimize/maximize/closeButton for NewBreeze
	*
*/

#include <NBWindowButton.hpp>

NBWindowButton::NBWindowButton( QColor color, QWidget *parent ) : QWidget( parent ) {

	mColor = color;
	cStop = 0.2;
};

void NBWindowButton::enterEvent( QEvent *eEvent ) {

	timer.start( 15, this );
	cStop += 0.01;

	repaint();

	eEvent->accept();
};

void NBWindowButton::leaveEvent( QEvent *lEvent ) {

	timer.stop();
	cStop = 0.2;

	repaint();

	lEvent->accept();
};

void NBWindowButton::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	emit clicked();
	mrEvent->accept();
};

void NBWindowButton::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setRenderHints( QPainter::HighQualityAntialiasing  | QPainter::Antialiasing );
	painter.setPen( Qt::NoPen );

	painter.setBrush( Qt::white );
	painter.drawEllipse( QRect( QPoint( 0, 0 ), QSize( width(), height() ) ) );

	QRadialGradient rg( QPoint( width() / 2, height() / 2 ), size().width() / 2, QPoint( width() / 2, height() / 2 ) );

	rg.setColorAt( 1.0, Qt::transparent );
	rg.setColorAt( 1 - cStop, Qt::transparent );
	rg.setColorAt( cStop, mColor );
	rg.setColorAt( 0.0, mColor );

	painter.setBrush( rg );
	painter.drawEllipse( QRect( QPoint( 0, 0 ), size() ) );

	painter.end();

	pEvent->accept();
};

void NBWindowButton::timerEvent( QTimerEvent *tEvent ) {

	if ( timer.timerId() == tEvent->timerId() ) {
		cStop += 0.05;
		if ( cStop > 1 ) {
			timer.stop();
			cStop = 1;
		}

		repaint();

		return;
	}

	QWidget::timerEvent( tEvent );
};
