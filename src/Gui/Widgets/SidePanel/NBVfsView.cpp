/*
	*
	* NBVfsView.cpp - Mounted VirtualFS View class for NewBreeze
	*
*/

#include <NBVfsView.hpp>

NBVfsIcon::NBVfsIcon( QWidget *parent ) : QWidget( parent ) {

	// Default Pixmap
	mPixmap = QPixmap( ":/icons/encfs.png" );

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
	devView = new NBVfsMenu( this );
	devView->setObjectName( "NBVfsMenu" );
	connect( devView, SIGNAL( triggered( QAction* ) ), this, SLOT( clickDrive( QAction* ) ) );
};

/* Override the QLabel pixmap property handlers */
QPixmap NBVfsIcon::pixmap() {

	return mPixmap;
};

void NBVfsIcon::setPixmap( QPixmap newPixmap ) {

	mPixmap = newPixmap;

	repaint();
};

/* Color property handlers */
QColor NBVfsIcon::flashColor() {

	return color;
};

void NBVfsIcon::setFlashColor( QColor newColor ) {

	color = newColor;
	color.setAlpha( alpha );

	repaint();
};

/* Alpha property handlers */
qint64 NBVfsIcon::alphaDelta() {

	return mAlphaDelta;
};

void NBVfsIcon::setAlphaDelta( qint64 newAlphaDelta ) {

	if ( ( newAlphaDelta * 10 ) > 255 )
		mAlphaDelta = 25;

	else
		mAlphaDelta = newAlphaDelta;
};

/* Flash duration property handlers */
int NBVfsIcon::flashDuration() {

	return mFlashDuration;
};

void NBVfsIcon::setFlashDuration( int newFlashDuration ) {

	mFlashDuration = ( newFlashDuration >= 10 ? newFlashDuration : 10 );
	timer.setInterval( mFlashDuration / flashSteps );
};

/* Number of Flashes property handlers */
int NBVfsIcon::numberOfFlashes() {

	return maxFlashes;
};

void NBVfsIcon::setNumberOfFlashes( int newNumOfFlashes ) {

	maxFlashes = newNumOfFlashes;
};

/* flashFrames property handlers - Number of frames per flash */
int NBVfsIcon::flashFrames() {

	return flashSteps;
};

void NBVfsIcon::setFlashFrames( int newFlashSteps ) {

	/* Persistence of vision is 62.5 (~63) ms */

	flashSteps = ( newFlashSteps > mFlashDuration ? mFlashDuration / 63 : newFlashSteps );
	timer.setInterval( mFlashDuration / flashSteps );
};

/* Overriding of paint event for showing flashes */
void NBVfsIcon::paintEvent( QPaintEvent *pEvent ) {

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
void NBVfsIcon::mousePressEvent( QMouseEvent *mEvent ) {

	if ( devView->isVisible() )
		devView->close();

	mEvent->accept();
};

/* Overriding QLabel::mouseMoveEvent to expand the view */
void NBVfsIcon::mouseMoveEvent( QMouseEvent *mEvent ) {

	mEvent->accept();
};

void NBVfsIcon::enterEvent( QEvent *eEvent ) {

	/* Start the delay timer */
	delayTimer.stop();
	delayTimer.start( 250, this );

	eEvent->accept();
};

void NBVfsIcon::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == delayTimer.timerId() ) {

		delayTimer.stop();
		if ( not devView->isVisible() and QRect( 0, 0, 48, 48 ).contains( mapFromGlobal( QCursor::pos() ) ) ) {

			showDevices();
		}
	}

	else if ( tEvent->timerId() == closeTimer.timerId() ) {

		closeTimer.stop();
		if ( devView->isVisible() )
			devView->close();
	}

	else
		QWidget::timerEvent( tEvent );

	tEvent->accept();
};

/* Slot to access the flashing */
void NBVfsIcon::flashLabel() {

	color = palette().color( QPalette::Window ).darker();
	color.setAlpha( alpha );

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;
};

/* Slot to access the flashing with a given color */
void NBVfsIcon::flashLabel( QColor newColor ) {

	setFlashColor( newColor );

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;
};

void NBVfsIcon::showDevices() {

	devView->clear();

	// Spacer Label
	QLabel *lbl = new QLabel( "<h4>&nbsp;&nbsp;&nbsp;&nbsp;VFS</h4>" );
	lbl->setFixedHeight( 48 );
	lbl->setMinimumWidth( 150 );

	QWidgetAction *lblAct = new QWidgetAction( devView );
	lblAct->setIcon( QIcon() );
	lblAct->setDefaultWidget( lbl );
	lblAct->setDisabled( true );

	devView->addAction( lblAct );

	Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allVirtualMounts() ) {

		QWidgetAction *wa = new QWidgetAction( devView );
		wa->setData( info.mountPoint() );

		NBVfsAction *actWdgt = new NBVfsAction( info, devView );
		wa->setDefaultWidget( actWdgt );

		devView->addAction( wa );
	};

	closeTimer.start( 10000, this );
	devView->exec( mapToGlobal( QPoint( 49, 0 ) ) );
};

void NBVfsIcon::clickDrive( QAction *act ) {

	QWidgetAction *wAct = qobject_cast<QWidgetAction*>( act );
	NBVfsAction *devAct = qobject_cast<NBVfsAction*>( wAct->defaultWidget() );

	if ( devAct )
		emit driveClicked( devAct->mountPoint() );
};

NBVfsAction::NBVfsAction( NBDeviceInfo info, QWidget *parent ) : QWidget( parent ) {

	setMouseTracking( true );

	select = false;

	setFixedHeight( 32 );

	QFontMetrics fm( font() );
	setMinimumWidth( qMax( 32 + fm.width( info.driveLabel() ) + 10, 150 ) );

	mDeviceLabel = info.driveLabel();
	icon = QIcon( ":/icons/" + info.driveType() + ".png" );
	mMountPoint = info.mountPoint();

	percentUsed = ( int )( info.usedSpace() * 100.0 / info.driveSize() );
};

QString NBVfsAction::mountPoint() {

	return mMountPoint;
};

void NBVfsAction::highlight( bool high ) {

	select = high;
	repaint();
}

void NBVfsAction::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );

	painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

	// Selection Background
	if ( select or hasFocus() ) {
		painter.save();
		QPalette pltt = palette();
		painter.setPen( Qt::NoPen );
		painter.setBrush( pltt.color( QPalette::Highlight ) );
		painter.drawRect( rect() );
		painter.restore();
	}

	// Label
	painter.save();
	painter.drawText( QRect( 32, 0, width() - 32 - 10, 20 ), Qt::AlignLeft | Qt::AlignVCenter, mDeviceLabel );
	painter.restore();

	// Icon
	painter.save();

	painter.setPen( Qt::NoPen );
	painter.drawPixmap( QRect( 4, 4, 24, 24 ), icon.pixmap( 24 ) );

	painter.restore();

	// Used/Free Size
	painter.save();
	double mFraction = percentUsed / 100.0;

	int red = 0, green = 0;
	// Lots of free space
	if ( mFraction <= 0.4 ) {
		green = ( int )( 255 );
		red = ( int )( mFraction * 638 );
	}

	// Around 50% free space remains
	else if ( mFraction <= 0.6 ) {
		red = 255;
		green = 255;
	}

	// Very less free space remaining
	else {
		green = ( int )( ( 1 - mFraction ) * 638 );
		red = ( int )( 255 );
	}

	painter.setPen( Qt::NoPen );
	painter.setBrush( QColor( red, green, 0 ) );
	painter.drawRoundedRect( QRect( 32, 20, ( width() - 32 - 10 ) * mFraction, 6 ), 4.0, 4.0 );

	painter.restore();

	painter.end();
};

/* Overriding QLabel::enterEvent to emit entered signal */
void NBVfsAction::enterEvent( QEvent *eEvent ) {

	select = true;
	repaint();
};

/* Overriding QLabel::leaveEvent to emit exited signal */
void NBVfsAction::leaveEvent( QEvent *lEvent ) {

	select = false;
	repaint();
};

NBVfsMenu::NBVfsMenu( QWidget *parent ) : QMenu( parent ) {

	connect( this, SIGNAL( hovered( QAction* ) ), this, SLOT( highlightAction( QAction* ) ) );
};

void NBVfsMenu::addAction( QWidgetAction *act ) {

	actionList << act;
	QMenu::addAction( act );
};

void NBVfsMenu::highlightAction( QAction *act ) {

	QWidgetAction *wAct = qobject_cast<QWidgetAction*>( act );
	Q_FOREACH( QWidgetAction *wa, actionList ) {
		NBVfsAction *devAct = qobject_cast<NBVfsAction*>( wa->defaultWidget() );
		if ( devAct and wAct )
			devAct->highlight( wAct == wa );
	}
};
