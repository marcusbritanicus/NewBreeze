/*
	*
	* NBDeviceView.cpp - A highly customized QWidget for NewBreeze
	*
*/

#include <NBDeviceView.hpp>
#include <NBDeviceManager.hpp>

NBDevicesIcon::NBDevicesIcon( QWidget *parent ) : QWidget( parent ) {

	// Default Pixmap
	mPixmap = QPixmap( ":/icons/comp.png" );

	// Flash settings
	alpha = 0;
	mAlphaDelta = 30;
	color = palette().color( QPalette::Window ).darker();
	color.setAlpha( alpha );

	currentStep = 0;
	flashSteps = 10;
	mFlashDuration = 200;

	flash = false;
	flashesCompleted = 0;
	maxFlashes = 2;

	timer.setInterval( mFlashDuration / flashSteps );
	timer.setSingleShot( false );

	connect( &timer, SIGNAL( timeout() ), this, SLOT( repaint() ) );

	// To track mouse movements
	setMouseTracking( true );

	// Set the fixed size of 48 px
	setFixedSize( 48, 48 );

	// DeviceView
	devView = new NBDeviceView( NULL );
	connect( devView, SIGNAL( close() ), this, SLOT( hideDevices() ) );
	connect( devView, SIGNAL( driveClicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );

	// DevView animation
	anim2 = new NBWidthAnimation( devView );
	anim2->setDuration( 250 );

	// Device Manager
	NBDeviceManager *mgr = new NBDeviceManager();
	connect( mgr, SIGNAL( updateDevices() ), this, SLOT( flashLabel() ) );
};

/* Override the QLabel pixmap property handlers */
QPixmap NBDevicesIcon::pixmap() {

	return mPixmap;
};

void NBDevicesIcon::setPixmap( QPixmap newPixmap ) {

	mPixmap = newPixmap;

	repaint();
};

/* Color property handlers */
QColor NBDevicesIcon::flashColor() {

	return color;
};

void NBDevicesIcon::setFlashColor( QColor newColor ) {

	color = newColor;
	color.setAlpha( alpha );

	repaint();
};

/* Alpha property handlers */
qint64 NBDevicesIcon::alphaDelta() {

	return mAlphaDelta;
};

void NBDevicesIcon::setAlphaDelta( qint64 newAlphaDelta ) {

	if ( ( newAlphaDelta * 10 ) > 255 )
		mAlphaDelta = 25;

	else
		mAlphaDelta = newAlphaDelta;
};

/* Flash duration property handlers */
int NBDevicesIcon::flashDuration() {

	return mFlashDuration;
};

void NBDevicesIcon::setFlashDuration( int newFlashDuration ) {

	mFlashDuration = ( newFlashDuration >= 10 ? newFlashDuration : 10 );
	timer.setInterval( mFlashDuration / flashSteps );
};

/* Number of Flashes property handlers */
int NBDevicesIcon::numberOfFlashes() {

	return maxFlashes;
};

void NBDevicesIcon::setNumberOfFlashes( int newNumOfFlashes ) {

	maxFlashes = newNumOfFlashes;
};

/* flashFrames property handlers - Number of frames per flash */
int NBDevicesIcon::flashFrames() {

	return flashSteps;
};

void NBDevicesIcon::setFlashFrames( int newFlashSteps ) {

	/* Persistence of vision is 62.5 (~63) ms */

	flashSteps = ( newFlashSteps > mFlashDuration ? mFlashDuration / 63 : newFlashSteps );
	timer.setInterval( mFlashDuration / flashSteps );
};

/* Overriding of paint event for showing flashes */
void NBDevicesIcon::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

	if ( flash ) {
		if ( flashesCompleted == maxFlashes ) {
			timer.stop();
			flash = false;
			currentStep = 0;
			alpha = 0;
			flashesCompleted = 0;
		}

		else if ( currentStep == flashSteps ) {
			flashesCompleted += 1;
			currentStep = 0;
		}

		else if ( currentStep < flashSteps / 2 ) {
			currentStep += 1;
			alpha += mAlphaDelta;
			color.setAlpha( alpha );
		}

		else if ( currentStep >= ( flashSteps / 2 ) ) {
			currentStep += 1;
			alpha -= mAlphaDelta;
			color.setAlpha( alpha );
		}
	}

	painter->save();

	painter->setPen( Qt::transparent );
	painter->setBrush( color );
	painter->drawRect( 0, 0, width(), height() );

	if ( not mPixmap.isNull() ) {
		painter->setBrush( Qt::transparent );
		painter->drawPixmap( 8, 8, 32, 32, mPixmap );
	}

	painter->restore();
	painter->end();

	pEvent->accept();
};

/* Overriding QLabel::mousePressEvent to emit clicked signal */
void NBDevicesIcon::mousePressEvent( QMouseEvent *mEvent ) {

	mEvent->accept();
};

/* Overriding QLabel::mouseMoveEvent to expand the view */
void NBDevicesIcon::mouseMoveEvent( QMouseEvent *mEvent ) {

	mEvent->accept();
};

void NBDevicesIcon::enterEvent( QEvent *eEvent ) {

	/* Start the delay timer */
	delayTimer.stop();
	delayTimer.start( 250, this );

	eEvent->accept();
};

void NBDevicesIcon::leaveEvent( QEvent *lEvent ) {

	/* If the mouse is in the devView, don't contract */
	if ( devView->isVisible() ) {
		if ( QRect( mapToGlobal( QPoint( 48, 0 ) ), QSize( devView->idealWidth(), devView->height() ) ).contains( QCursor::pos() ) ) {

			return;
		}
	}

	/* Start the delay timer */
	delayTimer.stop();
	delayTimer.start( 250, this );

	lEvent->accept();
};

void NBDevicesIcon::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == delayTimer.timerId() ) {
		delayTimer.stop();
		if ( devView->isVisible() ) {

			hideDevices();
		}

		else {
			if ( not QRect( 0, 0, 48, 48 ).contains( mapFromGlobal( QCursor::pos() ) ) )
				return;

			showDevices();
		}
	}
};

/* Slot to access the flashing */
void NBDevicesIcon::flashLabel() {

	color = palette().color( QPalette::Window ).darker();
	color.setAlpha( alpha );

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;

	devView->repopulate();
};

/* Slot to access the flashing with a given color */
void NBDevicesIcon::flashLabel( QColor newColor ) {

	setFlashColor( newColor );

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;
};

void NBDevicesIcon::showDevices() {

	devView->move( mapToGlobal( QPoint( 48, 0 ) ) );
	devView->show();

	anim2->stop();

	anim2->setEasingCurve( QEasingCurve( QEasingCurve::OutQuart ) );
	anim2->setStartValue( width() );
	anim2->setEndValue( devView->idealWidth() );
	anim2->start();
};

void NBDevicesIcon::hideDevices() {

	if ( QRect( 0, 0, 48, 48 ).contains( mapFromGlobal( QCursor::pos() ) ) )
		return;

	anim2->stop();

	anim2->setEasingCurve( QEasingCurve( QEasingCurve::OutQuint ) );
	anim2->setStartValue( width() );
	anim2->setEndValue( 0 );
	anim2->start();
};

NBDeviceView::NBDeviceView( QWidget *parent ) : QWidget( parent ) {

	// To track mouse movements
	setMouseTracking( true );

	// Cursor position
	cursor = QPoint();

	// Compute the ideal width and set the default size
	computeIdealWidth();
	setFixedSize( QSize( 0, 48 + NBDeviceManager::allDevices().count() * 32 ) );

	// Update the device-rect list
	repopulate();

	// Window Flags;
	setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
	setWindowModality( Qt::ApplicationModal );
};

void NBDeviceView::repopulate() {

	// Update the device-rect list
	devRectMap.clear();
	int y = 48;
	Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allDevices() ) {
		devRectMap[ y ] = info.mountPoint();
		y += 32;
	}

	setFixedHeight( y );
	computeIdealWidth();

	if ( isVisible() ) {

		resize( 0, 0 );
		updateGeometry();

		repaint();
	}
};

int NBDeviceView::idealWidth() {

	return mIdealWidth;
};

/* Overriding of paint event for showing flashes */
void NBDeviceView::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );

	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

	/* Devices label */
	painter->save();
	painter->setPen( Qt::black );
	painter->setFont( QFont( font().family(), font().pointSize(), QFont::Bold )  );
	painter->drawText( QRect( 10, 0, mIdealWidth - 10, 48 ), Qt::AlignVCenter | Qt::AlignLeft, "Devices" );
	painter->restore();

	painter->save();

	// Normal Text
	painter->setFont( QFont( font().family(), font().pointSize(), QFont::Normal, false ) );

	int y = 48;

	QFontMetrics fm( font() );

	Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allDevices() ) {
		QRect devRect( QPoint( 0, y ), QSize( mIdealWidth, 32 ) );
		y += 32;

		// Mouse over highlight
		if ( devRect.contains( mapFromGlobal( QCursor::pos() ) ) ) {
			painter->fillRect( devRect, palette().color( QPalette::Highlight ) );
			painter->setPen( palette().color( QPalette::HighlightedText ) );
		}

		else {
			painter->fillRect( devRect, Qt::transparent );
			painter->setPen( palette().color( QPalette::WindowText ) );
		}

		QRect iconRect( devRect.topLeft() + QPoint( 4, 4 ), QSize( 24, 24 ) );
		painter->drawPixmap( iconRect, QIcon( ":/icons/" + info.driveType() + ".png" ).pixmap( 24 ) );

		QRect textRect( devRect.adjusted( 32, 0, 0, 0 ) );
		painter->drawText( textRect, Qt::AlignVCenter, fm.elidedText( info.driveLabel(), Qt::ElideRight, mIdealWidth - 32 - 48 ) );
	}

	painter->restore();

	// Widget border
	painter->save();
	painter->setPen( QPen( Qt::darkGray, 1.0 ) );
	painter->drawRect( rect() );
	painter->restore();

	painter->end();
	pEvent->accept();
};

void NBDeviceView::computeIdealWidth() {

	/* IconWidth (32) + MaxTextWidth (computed below) + Buffer (48) */
	mIdealWidth = 32 + 48;

	int maxWidth = -1;

	QFontMetrics fm( font() );
	Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allDevices() ) {
		if ( fm.width( info.driveLabel() ) > maxWidth )
			maxWidth = fm.width( info.driveLabel() );
	}

	mIdealWidth += maxWidth;
	if ( mIdealWidth < 180 )
		mIdealWidth = 180;
};

/* Overriding QWidget::mousePressEvent to emit clicked signal */
void NBDeviceView::mousePressEvent( QMouseEvent *mEvent ) {

	Q_FOREACH( int y, devRectMap.keys() ) {
		if ( QRect( QPoint( 0, y ), QSize( mIdealWidth, 32 ) ).contains( mEvent->pos() ) ) {
			emit driveClicked( devRectMap.value( y ) );
			emit close();
			break;
		}
	}

	mEvent->accept();
};

/* Overriding QWidget::mouseMoveEvent to expand the view */
void NBDeviceView::mouseMoveEvent( QMouseEvent *mEvent ) {

	Q_FOREACH( int y, devRectMap.keys() ) {
		QRect devRect( QPoint( 0, y ), QSize( mIdealWidth, 32 ) );
		if ( devRect.contains( mEvent->pos() ) ) {
			/* It by default, comes under the mouse, so push it down right */
			QToolTip::showText( mapToGlobal( mEvent->pos() ) + QPoint( 20, 20 ), devRectMap.value( y ) );

			cursor = mEvent->pos();

			repaint();
			qApp->processEvents();

			break;
		}
	}

	mEvent->accept();
};

/* Overriding QWidget::keyPressEvent to handle escape */
void NBDeviceView::keyPressEvent( QKeyEvent *kEvent ) {

	switch( kEvent->key() ) {
		case Qt::Key_Escape:
			emit close();
			break;

		default:
			QWidget::keyPressEvent( kEvent );
			break;
	}

	kEvent->accept();
};

/* Overriding QWidget::enterEvent to emit entered signal */
void NBDeviceView::enterEvent( QEvent *eEvent ) {

	/* Stop the close timer */
	closeTimer.stop();
	eEvent->accept();
};

/* Overriding QWidget::leaveEvent to emit exited signal */
void NBDeviceView::leaveEvent( QEvent *lEvent ) {

	/* Start the close timer: in 500 ms the panel closes */
	closeTimer.stop();
	closeTimer.start( 500, this );

	lEvent->accept();
};

void NBDeviceView::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == closeTimer.timerId() ) {
		if ( not rect().contains( mapFromGlobal( QCursor::pos() ) ) ) {
			emit close();

			closeTimer.stop();
		}
	}
};
