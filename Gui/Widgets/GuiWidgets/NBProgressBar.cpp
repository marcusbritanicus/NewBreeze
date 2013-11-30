/*
	*
	* NBProgressBar.cpp - NewBreeze ProgressBar Class
	*
*/

#include <NBProgressBar.hpp>

NBProgressBar::NBProgressBar() : QWidget() {

	mMin = 0;
	mMax = 0;
	mFraction = 0;

	setFixedHeight( 10 );

	uncoveredBorder = QColor( 0x80, 0x00, 0x00 );
	coveredBorder = QColor( 0x00, 0x80, 0x00 );

	uncoveredGradient = QLinearGradient( 0, 0, 0, 10 );
	uncoveredGradient.setColorAt( 0.000, QColor(  0x80, 0x00, 0x00 ) );
	uncoveredGradient.setColorAt( 0.500, Qt::transparent );
	uncoveredGradient.setColorAt( 1.000, QColor(  0x80, 0x00, 0x00 ) );

	coveredGradient = QLinearGradient( 0, 0, 0, 10 );
	coveredGradient.setColorAt( 0.000, QColor(  0x00, 0x80, 0x00 ) );
	coveredGradient.setColorAt( 0.500, QColor(  0x00, 0xFF, 0x00 ) );
	coveredGradient.setColorAt( 1.000, QColor(  0x00, 0x80, 0x00 ) );
};

void NBProgressBar::setProgressFraction( qreal fraction ) {

	if ( ( 0 < fraction ) and ( fraction < 1 ) )
		mFraction = fraction;

	else
		mFraction = 1;

	repaint();
	qApp->processEvents();
};

void NBProgressBar::setMinimum( qreal min ) {

	mMin = min;
};

qreal NBProgressBar::minimum() {

	return mMin;
};

void NBProgressBar::setMaximum( qreal max ) {

	mMax = max;
};

qreal NBProgressBar::maximum() {

	return mMax;
};

void NBProgressBar::setRange( qreal min, qreal max ) {

	mMin = min;
	mMax = max;
};

void NBProgressBar::setValue( qreal value ) {

	if ( mMin > value )
		mFraction = 0;

	else if ( value > mMax )
		mFraction = 1;

	else
		mFraction = value / mMax;

	repaint();
	qApp->processEvents();
};

void NBProgressBar::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter();
	painter->begin( this );
	painter->save();

	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

	// Border
	painter->setBrush( QBrush( uncoveredGradient ) );
	painter->setPen( uncoveredBorder );
	painter->drawRoundedRect( 0, 0, width(), 10, 2, 2 );

	// Progress
	painter->setBrush( QBrush( coveredGradient ) );
	painter->setPen( coveredBorder );
	painter->drawRoundedRect( 0, 0, width() * mFraction, 10, 2, 2 );

	painter->restore();
	painter->end();

	pEvent->accept();
};
