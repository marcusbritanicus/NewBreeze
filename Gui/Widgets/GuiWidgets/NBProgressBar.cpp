/*
	*
	* NBProgressBar.cpp - ProgressBar Class for NewBreeze
	*
*/

#include <NBProgressBar.hpp>

NBProgressBar::NBProgressBar( QWidget *parent ) : QWidget( parent ) {

	mMin = 0;
	mMax = 0;
	mFraction = 0;

	progressText = QString();
	setFixedHeight( 24 );

	uncoveredGradient = QLinearGradient( 0, 0, 0, 24 );
	uncoveredGradient.setColorAt( 0.000, QColor(  0x80, 0x00, 0x00 ) );
	uncoveredGradient.setColorAt( 0.500, Qt::transparent );
	uncoveredGradient.setColorAt( 1.000, QColor(  0x80, 0x00, 0x00 ) );
};

void NBProgressBar::setValue( qreal value ) {

	if ( 0 > value )
		mFraction = 0;

	else if ( value > 1 )
		mFraction = 1;

	else
		mFraction = value;

	repaint();
	qApp->processEvents();
};

void NBProgressBar::setProgressText( QString text ) {

	progressText = text;
};

void NBProgressBar::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter();
	painter->begin( this );
	painter->save();

	int red = 0, green = 0;
	// Change from Red to Yellow: When mFraction = 0, green = 0; mFraction = 0.4, green = 255
	if ( mFraction <= 0.4 ) {
		red = ( int )( 255 );
		green = ( int )( mFraction * 638 );
	}

	// Remain Yellow
	else if ( mFraction <= 0.6 ) {
		red = 255;
		green = 255;
	}

	// Change from Yellow to Green: When totalF = 0.6, red = 255; mFraction = 1, red = 0;
	else {
		red = ( int )( ( 1 - mFraction ) * 638 );
		green = ( int )( 255 );
	}

	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

	// Border
	painter->setBrush( Qt::transparent );
	painter->setPen( QColor( red, green, 0 ) );
	painter->drawRoundedRect( 0, 0, width(), 24, 2, 2 );

	// Progress
	painter->setBrush( QColor( red, green, 0 ) );
	painter->setPen( Qt::NoPen );
	painter->drawRoundedRect( 0, 0, width() * mFraction, 24, 2, 2 );

	// Progress Text
	painter->setPen( Qt::black );
	painter->drawText( 0, 0, width(), 24, Qt::AlignCenter, progressText );

	painter->restore();
	painter->end();

	pEvent->accept();
};
