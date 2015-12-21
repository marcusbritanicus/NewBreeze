/*
	*
	* NBBookmarkView.cpp - A highly customized QWidget for NewBreeze
	*
*/

#include <NBBookmarkView.hpp>

QList<NBBookmarkInfo> allBookmarks() {

	QList<NBBookmarkInfo> bookmarkInfoList;

	foreach( QString bookmark, bookmarkSettings.value( "Order" ).toStringList() ) {
		if ( not exists( bookmark ) )
			continue;

		QString label = bookmarkSettings.value( QUrl::toPercentEncoding( bookmark ) ).toString();

		NBBookmarkInfo bmkInfo;
		bmkInfo.displayLabel = label;
		bmkInfo.displayIcon = "bookmarks";
		bmkInfo.mountPoint = bookmark;

		bookmarkInfoList << bmkInfo;
	}

	return bookmarkInfoList;
};

NBBookmarksIcon::NBBookmarksIcon( QWidget *parent ) : QWidget( parent ) {

	// Default Pixmap
	mPixmap = QPixmap( ":/icons/bookmark.png" );

	// Flash settings
	alpha = 0;
	mAlphaDelta = 30;
	color = QColor( 255, 255, 255, 0 );

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

	// BookmarksView
	bmkView = new NBBookmarksView( NULL );
	connect( bmkView, SIGNAL( close() ), this, SLOT( hideBookmarks() ) );
	connect( bmkView, SIGNAL( driveClicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );

	// bmkView animation
	anim2 = new NBWidthAnimation( bmkView );
	anim2->setDuration( 250 );
};

/* Override the QLabel pixmap property handlers */
QPixmap NBBookmarksIcon::pixmap() {

	return mPixmap;
};

void NBBookmarksIcon::setPixmap( QPixmap newPixmap ) {

	mPixmap = newPixmap;

	repaint();
};

/* Color property handlers */
QColor NBBookmarksIcon::flashColor() {

	return color;
};

void NBBookmarksIcon::setFlashColor( QColor newColor ) {

	color = newColor;
	color.setAlpha( alpha );

	repaint();
};

/* Alpha property handlers */
qint64 NBBookmarksIcon::alphaDelta() {

	return mAlphaDelta;
};

void NBBookmarksIcon::setAlphaDelta( qint64 newAlphaDelta ) {

	if ( ( newAlphaDelta * 10 ) > 255 )
		mAlphaDelta = 25;

	else
		mAlphaDelta = newAlphaDelta;
};

/* Flash duration property handlers */
int NBBookmarksIcon::flashDuration() {

	return mFlashDuration;
};

void NBBookmarksIcon::setFlashDuration( int newFlashDuration ) {

	mFlashDuration = ( newFlashDuration >= 10 ? newFlashDuration : 10 );
	timer.setInterval( mFlashDuration / flashSteps );
};

/* Number of Flashes property handlers */
int NBBookmarksIcon::numberOfFlashes() {

	return maxFlashes;
};

void NBBookmarksIcon::setNumberOfFlashes( int newNumOfFlashes ) {

	maxFlashes = newNumOfFlashes;
};

/* flashFrames property handlers - Number of frames per flash */
int NBBookmarksIcon::flashFrames() {

	return flashSteps;
};

void NBBookmarksIcon::setFlashFrames( int newFlashSteps ) {

	/* Persistence of vision is 62.5 (~63) ms */

	flashSteps = ( newFlashSteps > mFlashDuration ? mFlashDuration / 63 : newFlashSteps );
	timer.setInterval( mFlashDuration / flashSteps );
};

/* Overriding of paint event for showing flashes */
void NBBookmarksIcon::paintEvent( QPaintEvent *pEvent ) {

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
void NBBookmarksIcon::mousePressEvent( QMouseEvent *mEvent ) {

	mEvent->accept();
};

/* Overriding QLabel::mouseMoveEvent to expand the view */
void NBBookmarksIcon::mouseMoveEvent( QMouseEvent *mEvent ) {

	mEvent->accept();
};

/* Overriding QLabel::enterEvent to emit entered signal */
void NBBookmarksIcon::enterEvent( QEvent *eEvent ) {

	if ( not QRect( 0, 0, 48, 48 ).contains( mapFromGlobal( QCursor::pos() ) ) )
		return;

	/* Start the delay timer */
	delayTimer.stop();
	delayTimer.start( 250, this );

	eEvent->accept();
};

/* Overriding QLabel::leaveEvent to emit exited signal */
void NBBookmarksIcon::leaveEvent( QEvent *lEvent ) {

	/* If the mouse is in the bmkView, don't contract */
	if ( bmkView->isVisible() ) {
		if ( bmkView->rect().contains( bmkView->mapFromGlobal( QCursor::pos() ) ) )
			return;
	}

	/* Start the delay timer */
	delayTimer.stop();
	delayTimer.start( 250, this );

	lEvent->accept();
};

void NBBookmarksIcon::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == delayTimer.timerId() ) {
		delayTimer.stop();
		if ( QRect( 0, 0, 48, 48 ).contains( mapFromGlobal( QCursor::pos() ) ) )
			showBookmarks();

		else
			hideBookmarks();
	}
};

/* Slot to access the flashing */
void NBBookmarksIcon::flashLabel() {

	if ( colorFlash ) {
		colorFlash = false;
		color = QColor( 255, 255, 255, 0 );
	}

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;

	bmkView->repopulate();
};

/* Slot to access the flashing with a given color */
void NBBookmarksIcon::flashLabel( QColor newColor ) {

	setFlashColor( newColor );

	if ( timer.isActive() )
		timer.stop();

	timer.start();
	flash = true;
};

void NBBookmarksIcon::showBookmarks() {

	bmkView->move( mapToGlobal( QPoint( 48, 0 ) ) );
	bmkView->show();

	anim2->stop();

	anim2->setEasingCurve( QEasingCurve( QEasingCurve::OutQuart ) );
	anim2->setStartValue( width() );
	anim2->setEndValue( bmkView->idealWidth() );
	anim2->start();
};

void NBBookmarksIcon::hideBookmarks() {

	if ( QRect( 0, 0, 48, 48 ).contains( mapFromGlobal( QCursor::pos() ) ) )
		return;

	anim2->stop();

	anim2->setEasingCurve( QEasingCurve( QEasingCurve::InQuart ) );
	anim2->setStartValue( width() );
	anim2->setEndValue( 0 );
	anim2->start();
};

NBBookmarksView::NBBookmarksView( QWidget *parent ) : QWidget( parent ) {

	/* To track mouse movements */
	setMouseTracking( true );

	/* Cursor position */
	cursor = QPoint();

	/* Compute the ideal width and set the default size */
	computeIdealWidth();
	setFixedSize( QSize( 0, allBookmarks().count() * 32 ) );

	/* Update the bookmarks-rect list */
	bmkRectMap.clear();
	int y = 0;
	Q_FOREACH( NBBookmarkInfo info, allBookmarks() ) {
		bmkRectMap[ y ] = info.mountPoint;
		y += 32;
	}

	/* Window Flags; */
	setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
	setWindowModality( Qt::ApplicationModal );
};

void NBBookmarksView::repopulate() {

	/* Update the bookmarks-rect list */
	bmkRectMap.clear();
	int y = 0;
	Q_FOREACH( NBBookmarkInfo info, allBookmarks() ) {
		bmkRectMap[ y ] = info.mountPoint;
		y += 32;
	}

	if ( isVisible() ) {

		setFixedHeight( allBookmarks().count() * 32 );
		computeIdealWidth();
		repaint();
	}
};

int NBBookmarksView::idealWidth() {

	return mIdealWidth;
};

/* Overriding of paint event for showing flashes */
void NBBookmarksView::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

	painter->save();

	/* Normal Text */
	painter->setFont( QFont( font().family(), font().pointSize(), QFont::Normal, false ) );

	int y = 0;

	QFontMetrics fm( font() );

	Q_FOREACH( NBBookmarkInfo info, allBookmarks() ) {
		QRect devRect( QPoint( 0, y ), QSize( mIdealWidth, 32 ) );
		y += 32;

		/* Mouse over highlight */
		if ( devRect.contains( mapFromGlobal( QCursor::pos() ) ) ) {
			painter->fillRect( devRect, palette().color( QPalette::Highlight ) );
			painter->setPen( palette().color( QPalette::HighlightedText ) );
		}

		else {
			painter->fillRect( devRect, Qt::transparent );
			painter->setPen( palette().color( QPalette::WindowText ) );
		}

		QRect iconRect( devRect.topLeft() + QPoint( 4, 4 ), QSize( 24, 24 ) );
		painter->drawPixmap( iconRect, QIcon::fromTheme( "bookmarks" ).pixmap( 24 ) );

		QRect textRect( devRect.adjusted( 32, 0, 0, 0 ) );
		painter->drawText( textRect, Qt::AlignVCenter, fm.elidedText( info.displayLabel, Qt::ElideRight, mIdealWidth - 32 - 48 ) );
	}

	painter->restore();

	/* Widget border */
	painter->save();
	painter->setPen( QPen( Qt::darkGray, 1.0 ) );
	painter->drawRect( rect() );
	painter->restore();

	painter->end();
	pEvent->accept();
};

void NBBookmarksView::computeIdealWidth() {

	/* IconWidth (32) + MaxTextWidth (computed below) + Buffer (48) */
	mIdealWidth = 32 + 48;

	int maxWidth = -1;

	QFontMetrics fm( font() );
	Q_FOREACH( NBBookmarkInfo info, allBookmarks() ) {
		if ( fm.width( info.displayLabel ) > maxWidth )
			maxWidth = fm.width( info.displayLabel );
	}

	mIdealWidth += maxWidth;
	if ( mIdealWidth < 180 )
		mIdealWidth = 180;
};

/* Overriding QLabel::mousePressEvent to emit clicked signal */
void NBBookmarksView::mousePressEvent( QMouseEvent *mEvent ) {

	Q_FOREACH( int y, bmkRectMap.keys() ) {
		if ( QRect( QPoint( 0, y ), QSize( mIdealWidth, 32 ) ).contains( mEvent->pos() ) ) {
			emit driveClicked( bmkRectMap.value( y ) );
			emit close();
			break;
		}
	}

	mEvent->accept();
};

/* Overriding QLabel::mouseMoveEvent to expand the view */
void NBBookmarksView::mouseMoveEvent( QMouseEvent *mEvent ) {

	Q_FOREACH( int y, bmkRectMap.keys() ) {
		QRect devRect( QPoint( 0, y ), QSize( mIdealWidth, 32 ) );
		if ( devRect.contains( mEvent->pos() ) ) {
			/* It by default, comes under the mouse, so push it down right */
			QToolTip::showText( mapToGlobal( mEvent->pos() ) + QPoint( 20, 20 ), bmkRectMap.value( y ) );

			cursor = mEvent->pos();

			repaint();
			qApp->processEvents();

			break;
		}
	}

	mEvent->accept();
};

/* Overriding QWidget::keyPressEvent to handle escape */
void NBBookmarksView::keyPressEvent( QKeyEvent *kEvent ) {

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

/* Overriding QLabel::enterEvent to emit entered signal */
void NBBookmarksView::enterEvent( QEvent *eEvent ) {

	/* Stop the close timer */
	closeTimer.stop();
	eEvent->accept();
};

/* Overriding QLabel::leaveEvent to emit exited signal */
void NBBookmarksView::leaveEvent( QEvent *lEvent ) {

	/* Start the close timer: in 500 ms the panel closes */
	closeTimer.stop();
	closeTimer.start( 500, this );

	lEvent->accept();
};

void NBBookmarksView::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == closeTimer.timerId() ) {
		if ( not rect().contains( mapFromGlobal( QCursor::pos() ) ) ) {
			emit close();

			closeTimer.stop();
		}
	}
};
