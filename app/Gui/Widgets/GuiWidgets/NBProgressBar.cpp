/*
	*
	* NBProgressBar.cpp - ProgressBar Class for NewBreeze
	*
*/

#include "NBProgressBar.hpp"

NBProgressBar::NBProgressBar( QWidget *parent ) : QWidget( parent ) {

	mMin = 0;
	mMax = 0;
	mFraction = 0;

	mSway = true;
	xpos = 0;
	fwd = false;

	progressText = QString();
	setFixedHeight( 24 );

	swayTimer.start( 50, this );
};

void NBProgressBar::setValue( qreal value ) {

	/* All positive values will stop the swayTimer */
	if ( value >= 0 ) {
		if ( swayTimer.isActive() ) {
			mSway = false;
			swayTimer.stop();
		}

		/* If value is greater than 1, make it one, otherwise retain its value in mFraction */
		mFraction = ( value > 1 ? 1 : value );

		repaint();
		qApp->processEvents();

		return;
	}

	/* All negative values will start the sway timer, if not already activated */
	if ( not swayTimer.isActive() ) {

		mSway = true;
		swayTimer.start( 50, this );
	}
};

void NBProgressBar::setSway( bool sway ) {

	mSway = sway;
	swayTimer.start( 50, this );
};

void NBProgressBar::setProgressText( QString text ) {

	progressText = text;
};

void NBProgressBar::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter();
	painter->begin( this );

	if ( mSway ) {
		painter->save();

		int x = 0;
		/* Forward motion */
		if ( fwd ) {
			x = xpos += 0.01 * width();
			if ( x > 0.9 * width() )
				fwd = false;
		}

		else {
			x = xpos -= 0.01 * width();
			if ( x < 0 )
				fwd = true;
		}

		painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

		// Border
		painter->setBrush( Qt::transparent );
		painter->setPen( QColor( palette().color( QPalette::Highlight ) ) );
		painter->drawRoundedRect( 0, 0, width(), 24, 2, 2 );

		// Progress
		painter->setBrush( QColor( palette().color( QPalette::Highlight ) ) );
		painter->setPen( Qt::NoPen );
		painter->drawRoundedRect( x, 0, width() * 0.1, 24, 2, 2 );

		// Progress Text
		painter->setPen( Qt::black );
		painter->drawText( 0, 0, width(), 24, Qt::AlignCenter, progressText );

		painter->restore();
	}

	else {
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
	}

	painter->end();

	pEvent->accept();
};

void NBProgressBar::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == swayTimer.timerId() )
		repaint();

	else
		QWidget::timerEvent( tEvent );

	tEvent->accept();
};
