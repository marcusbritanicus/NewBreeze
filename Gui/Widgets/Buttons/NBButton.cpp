/*
	*
	* NBButton.cpp - Alternate PushButton/ToolButton for NewBreeze
	*
*/

#include <NBButton.hpp>

NBButton::NBButton( QWidget *parent ) : QAbstractButton( parent ) {

	initPrivateVariables();

	/* We assume we want a push button */
	mHeight = 24;
	mWidth = 48;

	/* PushButton */
	m_ButtonType = 1;

	setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
};

NBButton::NBButton( QIcon icon, QWidget *parent ) : QAbstractButton( parent ) {

	initPrivateVariables();

	/* Set the icon */
	mIcon = icon;

	/* ToolButton */
	m_ButtonType = 0;

	setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
};

NBButton::NBButton( QString text, QWidget *parent ) : QAbstractButton( parent ) {

	initPrivateVariables();

	/* Set the text */
	mText = text;

	/* PushButton Width */
	mWidth = 120;

	/* Make sure the size is larger than the text: Buffer 10 */
	QFontMetrics fm( font() );
	if ( mWidth < fm.width( mText ) + 24 + 10 )
		mWidth = fm.width( mText ) + 24 + 10;

	/* PushButton */
	m_ButtonType = 1;

	setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );

	/* Keyboard Mnemonic */
	QKeySequence newMnemonic = QKeySequence::mnemonic( text );
	setShortcut( newMnemonic );
};

NBButton::NBButton( QIcon icon, QString text, QWidget *parent ) : QAbstractButton( parent ) {

	initPrivateVariables();

	mIcon = icon;
	mText = text;

	/* PushButton Width */
	mWidth = 120;

	/* Make sure the size is larger than the text: Buffer 10 */
	QFontMetrics fm( font() );
	if ( mWidth < fm.width( mText ) + 24 + 10 )
		mWidth = fm.width( mText ) + 24 + 10;

	/* PushButton */
	m_ButtonType = 1;

	setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );

	/* Keyboard Mnemonic */
	setShortcut( QKeySequence::mnemonic( text ) );
};

QString NBButton::text() {

	return mText;
};

void NBButton::setText( QString text ) {

	if ( mText == text )
		return;

	mText = text;

	if ( text.isEmpty() ) {
		/* Its a toolbutton with default size: 24x24 */
		mHeight = mIconSize.height() + 8;
		mWidth = mIconSize.width() + 8;

		/* ToolButton */
		m_ButtonType = 0;

		setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	}

	else {
		/* Its a pushbutton with default size: 120x24 */
		mHeight = mIconSize.height() + 8;
		mWidth = 120;

		/* Make sure the size is larger than the text: Buffer 10 */
		QFontMetrics fm( font() );
		if ( mWidth < fm.width( mText ) + mIconSize.width() + 8 + 10 )
			mWidth = fm.width( mText ) + mIconSize.width() + 8 + 10;

		/* PushButton */
		m_ButtonType = 1;

		setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
	}

	/* Keyboard Mnemonic */
	QKeySequence newMnemonic = QKeySequence::mnemonic( text );
	setShortcut( newMnemonic );

	updateGeometry();
	QAbstractButton::setText( text );
};

QIcon NBButton::icon() {

	return mIcon;
};

void NBButton::setIcon( QIcon icon ) {

	mIcon = icon;

	if ( mText.isEmpty() ) {
		/* Its a toolbutton with default size: 24x24 */
		mHeight = mIconSize.height() + 8;
		mWidth = mIconSize.width() + 8;

		/* ToolButton */
		m_ButtonType = 0;

		setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	}

	else {
		/* Its a pushbutton with default size: 120x24 */
		mHeight = mIconSize.height() + 8;
		mWidth = 120;

		/* Make sure the size is larger than the text: Buffer 10 */
		QFontMetrics fm( font() );
		if ( mWidth < fm.width( mText ) + mIconSize.width() + 8 + 10 )
			mWidth = fm.width( mText ) + mIconSize.width() + 8 + 10;

		/* PushButton */
		m_ButtonType = 1;

		setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
	}

	updateGeometry();
	QAbstractButton::setIcon( icon );
}

QSize NBButton::iconSize() {

	return mIconSize;
};

void NBButton::setIconSize( QSize iconSize ) {

	mIconSize = iconSize;

	if ( mText.isEmpty() ) {
		/* Its a toolbutton with default size: 24x24 */
		mHeight = mIconSize.height() + 8;
		mWidth = mIconSize.width() + 8;

		setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	}

	else {
		/* Its a pushbutton with default size: 120x24 */
		mHeight = mIconSize.height() + 8;
		mWidth = 120;

		/* Make sure the size is larger than the text: Buffer 10 */
		QFontMetrics fm( font() );
		if ( mWidth < fm.width( mText ) + mIconSize.width() + 8 + 10 )
			mWidth = fm.width( mText ) + mIconSize.width() + 8 + 10;

		setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
	}

	updateGeometry();
	QAbstractButton::setIconSize( iconSize );
};

QMenu* NBButton::menu() {

	return mMenu;
};

void NBButton::setMenu( QMenu *newMenu ) {

	if ( mMenu == newMenu )
		return;

	if ( not newMenu )
		return;

	mMenu = newMenu;
	mMenu->setParent( this );

	/* We have a menu */
	m_HasMenu = true;

	/* Increase the size by 16px */
	mWidth += 16;
	repaint();

	updateGeometry();
};

QKeySequence NBButton::shortcut() {

	return mShortcut;
};

void NBButton::setShortcut( QKeySequence keySeq ) {

	QAction *act = new QAction( this );
	act->setShortcut( keySeq );
	connect( act, SIGNAL( triggered() ), this, SLOT( animateClick() ) );
	addAction( act );
};

QSize NBButton::size() {

	return QSize( mWidth, mHeight );
};

/* I will not change the button height for you */
void NBButton::resize( QSize newSize ) {

	mWidth = newSize.width();
	repaint();

	updateGeometry();
};

/* I will not change the button height for you */
void NBButton::resize( int newWidth, int ) {

	mWidth = newWidth;
	repaint();

	updateGeometry();
};

QSize NBButton::sizeHint() const {

	return QSize( mWidth, mHeight );
};

void NBButton::showMenu() {

	m_MenuButtonPressed = true;
	repaint();

	mMenu->exec( mapToGlobal( rect().bottomLeft() ) );

	m_MenuButtonPressed = false;
	repaint();
};

void NBButton::initPrivateVariables() {

	/* Icon/Text */
	mIcon = QIcon();
	mText = QString();

	/* Icon Size */
	mIconSize = QSize( 16, 16 );

	/* Button Size */
	mHeight = 24,
	mWidth = 24;

	/* Button type: 0 tool, 1 push */
	m_ButtonType = 1;

	/* Menu */
	mMenu = NULL;
	m_HasMenu = false;

	/* Shortcut */
	mShortcut = QKeySequence();
	mShortcutId = 0;

	/* Menu button was pressed */
	m_MenuButtonPressed = false;

	/* This button was pressed and held */
	m_Pressed = false;
};

bool NBButton::event( QEvent *evnt ) {

    if ( evnt->type() == QEvent::KeyPress ) {

		QKeyEvent *kEvent = static_cast<QKeyEvent*>( evnt );
		switch( kEvent->key() ) {
			case Qt::Key_Space :
			case Qt::Key_Return :
			case Qt::Key_Enter : {
				animateClick();
				return true;
			}

			default:
				return false;
		}
	}

    return QAbstractButton::event( evnt );
}

void NBButton::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == animateTimer.timerId() ) {
		animateTimer.stop();

		m_Pressed = false;
		repaint();
		QApplication::flush();

		emit released();
		emit clicked();
	}

	QAbstractButton::timerEvent( tEvent );
};

void NBButton::mousePressEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() != Qt::LeftButton ) {

		mEvent->ignore();
		return;
	}

	if ( not isEnabled() ) {

		mEvent->ignore();
		return;
	}

	QPoint clickPos = mEvent->pos();

	/* If a menu has been set, and we clicked the mMenu button */
	if ( mMenu and QRect( mWidth - 16, 0, 16, mHeight ).contains( clickPos ) )
		showMenu();

	/* We do not have a menu */
	else {
		m_Pressed = true;
		emit pressed();
	}

	repaint();
	QApplication::flush();

	QAbstractButton::mousePressEvent( mEvent );
	mEvent->accept();
};

void NBButton::mouseReleaseEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() != Qt::LeftButton ) {
		mEvent->ignore();
		return;
	}

	if ( not isEnabled() ) {

		mEvent->ignore();
		return;
	}

	if ( m_Pressed ) {
		m_Pressed = false;

		emit released();
		emit clicked();
	}

	repaint();
	QApplication::flush();

	QAbstractButton::mouseReleaseEvent( mEvent );
	mEvent->accept();
};

void NBButton::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );

	QFontMetrics fm( font() );

	/* ToolButton */
	if ( not m_ButtonType )
		paintToolButton( painter );

	else
		paintPushButton( painter );

	painter->end();
	pEvent->accept();
};

void NBButton::paintToolButton( QPainter *painter ) {

	painter->save();

	/* Actual Icon Size */
	QSize aIconSize = mIcon.actualSize( mIconSize );
	/* If we have a valid icon, we draw it */
	if ( aIconSize.isValid() ) {
		QRect iconRect( QPoint( ( mWidth - aIconSize.width() ) / 2, ( mHeight - aIconSize.height() ) / 2 ), aIconSize );

		if ( m_HasMenu ) {
			// Draw the pixmap
			painter->save();
			if ( m_Pressed )
				painter->drawPixmap( iconRect.adjusted( -7, 1, -8, 0 ), mIcon.pixmap( mIconSize, ( isEnabled() ? QIcon::Normal : QIcon::Disabled ) ) );

			else
				painter->drawPixmap( iconRect.adjusted( -8, 0, -8, 0 ), mIcon.pixmap( mIconSize, ( isEnabled() ? QIcon::Normal : QIcon::Disabled ) ) );
			painter->restore();
		}

		else {
			if ( m_Pressed )
				painter->drawPixmap( iconRect.adjusted( 1, 1, 0, 0 ), mIcon.pixmap( mIconSize, ( isEnabled() ? QIcon::Normal : QIcon::Disabled ) ) );

			else
				painter->drawPixmap( iconRect, mIcon.pixmap( mIconSize, ( isEnabled() ? QIcon::Normal : QIcon::Disabled ) ) );
		}
	}

	/* Draw the menu indicator */
	if ( m_HasMenu ) {
		// Draw a separator
		painter->save();
		QLinearGradient vGrad = QLinearGradient( QPoint( 0, 4 ), QPoint( 0, mHeight - 4 ) );
		vGrad.setColorAt( 0.0, Qt::transparent );
		vGrad.setColorAt( 0.4, palette().color( QPalette::Mid ) );
		vGrad.setColorAt( 0.6, palette().color( QPalette::Mid ) );
		vGrad.setColorAt( 1.0, Qt::transparent );

		painter->setPen( QPen( QBrush( vGrad ), 1.0 ) );
		painter->drawLine( QPoint( mWidth - 16, 4 ), QPoint( mWidth - 16, mHeight - 4 ) );
		painter->restore();

		// Draw the menu arrow
		QStyleOptionSpinBox options;
		options.rect = QRect( mWidth - 16, 0, 16, mHeight );
		options.state = m_MenuButtonPressed ? QStyle::State_MouseOver | QStyle::State_Enabled : QStyle::State_Off;
		options.palette = palette();

		qApp->style()->drawPrimitive( QStyle::PE_IndicatorArrowDown, &options, painter );
	}

	painter->restore();
};

void NBButton::paintPushButton( QPainter *painter ) {

	painter->save();

	/* Width if menu is not there: effectiveWidth */
	int eWidth = ( m_HasMenu ? mWidth - 16 : mWidth );

	/* Actual Icon Size */
	QSize aIconSize = mIcon.actualSize( mIconSize );

	/* Total paintWidth: icon + icon-buffer (8) + text */
	QFontMetrics fm( font() );
	int pWidth = ( aIconSize.isValid() ? 4 + mIconSize.width() + 4 + fm.width( mText ) : fm.width( mText ) );

	if ( not isEnabled() )
		painter->setPen( Qt::gray );

	else if ( hasFocus() or rect().contains( mapFromGlobal( QCursor::pos() ) ) or m_Pressed ) {
		if ( objectName() == "okBtn" )
			painter->setPen( Qt::darkGreen );

		else if ( objectName() == "cancelBtn" )
			painter->setPen( QColor( "brown" ) );

		else if ( objectName() == "abortBtn" )
			painter->setPen( Qt::red );

		else
			painter->setPen( palette().color( QPalette::WindowText ) );
	}

	/* We do not have an icon */
	if ( not aIconSize.isValid() ) {
		if ( m_Pressed )
			painter->drawText( QRect( 1, 1, eWidth, mHeight ), Qt::AlignCenter | Qt::TextShowMnemonic, mText );

		else
			painter->drawText( QRect( 0, 0, eWidth, mHeight ), Qt::AlignCenter | Qt::TextShowMnemonic, mText );
	}

	/* We have an icon to paint */
	else {
		int iconX = ( eWidth - pWidth + 8 ) / 2;
		int iconY = ( mHeight - aIconSize.height() ) / 2;

		QRect iconRect( QPoint( iconX, iconY ), aIconSize );
		QRect textRect( iconX + aIconSize.width() + 8, 0, fm.width( mText ), mHeight );

		if ( m_Pressed ) {
			painter->drawPixmap( iconRect.adjusted( 1, 1, 0, 0 ), mIcon.pixmap( mIconSize, ( isEnabled() ? QIcon::Normal : QIcon::Disabled ) ) );
			painter->drawText( textRect.adjusted( 1, 2, 0, 0 ), Qt::AlignVCenter | Qt::AlignLeft | Qt::TextShowMnemonic, mText );
		}

		else {
			painter->drawPixmap( iconRect, mIcon.pixmap( mIconSize, ( isEnabled() ? QIcon::Normal : QIcon::Disabled ) ) );
			painter->drawText( textRect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextShowMnemonic, mText );
		}
	}

	/* Drawing the menu button */
	if ( m_HasMenu ) {
		// Draw a separator: A little shorter than the height of the button
		painter->save();
		QLinearGradient vGrad = QLinearGradient( QPoint( 0, 2 ), QPoint( 0, mHeight - 2 ) );
		vGrad.setColorAt( 0.0, Qt::transparent );
		vGrad.setColorAt( 0.4, palette().color( QPalette::Mid ) );
		vGrad.setColorAt( 0.6, palette().color( QPalette::Mid ) );
		vGrad.setColorAt( 1.0, Qt::transparent );

		painter->setPen( QPen( QBrush( vGrad ), 1.0 ) );
		painter->drawLine( QPoint( mWidth - 16, 2 ), QPoint( mWidth - 16, mHeight - 4 ) );
		painter->restore();

		// Draw the menu arrow
		QStyleOptionSpinBox options;
		options.rect = QRect( mWidth - 16, 0, 16, mHeight );
		options.state = m_MenuButtonPressed ? QStyle::State_MouseOver | QStyle::State_Enabled : QStyle::State_Off;
		options.palette = palette();

		qApp->style()->drawPrimitive( QStyle::PE_IndicatorSpinDown, &options, painter );
	}

	painter->restore();
};

void NBButton::animateClick() {

	if ( not animateTimer.isActive() ) {
		animateTimer.start( 50, this );

		m_Pressed = true;
		repaint();
		QApplication::flush();

		emit pressed();
	}
};
