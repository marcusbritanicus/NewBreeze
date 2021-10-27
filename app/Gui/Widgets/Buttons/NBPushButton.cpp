/*
	*
	* NBPushButton.cpp - An improved QCommandLinkButton for NewBreeze
	*
*/

#include "NBPushButton.hpp"

NBPushButton::NBPushButton( QWidget *parent ) : QPushButton( parent ) {

	// Icon
	mIcon = QIcon();

	// Text
	mText = QString();
	mDesc = QString();

	// Button Size and Icon Size
	mIdealSize = QSize( 250, 48 );
	mIconSize = QSize( 4, 4 );
	mTextSize = QSize( 0, 0 );
	mDescSize = QSize( 0, 0 );

	mWidth = 200;

	// QTextDocument to determine text size
	textFont = font();
	fmb = new QFontMetrics( QFont( textFont.family(), textFont.pointSize(), QFont::Bold ) );
	fmn = new QFontMetrics( textFont );

	// Button pressed flag
	mCheckable = false;
	mBtnDown = false;
	mChecked = false;

	// No focus
	setFocusPolicy( Qt::NoFocus );

	// Initialize the size
	resetSize();

	setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
};

void NBPushButton::setFont( QFont newFont ) {
	/* Override the inbuilt QPushButton::setFont(...); */

	QPushButton::setFont( newFont );

	textFont = newFont;
	fmb = new QFontMetrics( QFont( textFont.family(), textFont.pointSize(), QFont::Bold ) );
	fmn = new QFontMetrics( textFont );

	resetSize();
};

QString NBPushButton::text() {
	/* Return the button text */

	return mText;
};

void NBPushButton::setText( QString txt ) {
	/*setText( QString ) -> None

	Button Text

	@return None */

	mText = QString( txt );
	if ( txt.count() )
		mTextSize = QSize( fmb->width( txt ), fmb->height() );

	else
		mTextSize = QSize( 0, 0 );

	resetSize();

	if ( mIcon.isNull() )
		setMinimumSize( mTextSize + QSize( 16, 16 ) );

	else
		setMinimumSize( QSize( mIconSize.width() + mTextSize.width() + 8 + 16, qMax( mIconSize.height() + 8, mTextSize.height() + 12 ) ) );
};

QString NBPushButton::description() {
	/*description() -> QString

	Return the button description

	@return QString */

	return mDesc;
};

void NBPushButton::setDescription( QString desc ) {
	/*setDescription( QString ) -> None

	Button description

	@return QString */

	mDesc = QString( desc );

	if ( desc.count() )
		mDescSize = fmn->boundingRect( QRect( 0, 0, qMax( mWidth, width() - mIconSize.width() - 8 ), 100 ), Qt::TextWordWrap, desc ).size();

	else
		mDescSize = QSize( 0, 0 );

	resetSize();
};

QIcon NBPushButton::icon() {
	/*icon() -> QIcon

	Return the button icon

	@return QIcon */

	return mIcon;
};

void NBPushButton::setIcon( QIcon icn ) {
	/*setIcon( QIcon ) -> None

	Button icon

	@return None */

	mIcon = icn;

	if ( icn.isNull() )
		mIconSize = QSize( 0, 0 );

	resetSize();

	if ( mText.count() )
		setMinimumSize( QSize( mIconSize.width() + mTextSize.width() + 8 + 16, qMax( mIconSize.height() + 8, mTextSize.height() + 12 ) ) );

	else
		setMinimumSize( mIconSize + QSize( 8, 8 ) );
};

QSize NBPushButton::iconSize() {
	/*iconSize() -> QSize

	Return the button icon size

	@return QSize */

	return mIconSize;
};

void NBPushButton::setIconSize( QSize iSize ) {
	/*setIconSize( QSize ) -> None

	Button icon size

	@return None */

	mIconSize = iSize;
	resetSize();
};

void NBPushButton::resetSize() {
	/* Resize the Push Button */

	/* MinimumWidth = Text Width + Icon Width + 4px Icon Padding + 8px Text Padding */
	/* MinimumHeight = qMax( Icon Height + 8, Text Height + 16 ); */

	/* Init h = w = 0 */
	int h = 0, w = 0;

	/* If there is no icon or text: minimum height 24 */
	if ( mIcon.isNull() and not ( mText + mDesc ).count() ) {
		h = 24;
		w = 64;
	}

	/* No Icon, but we have text */
	else if ( mIcon.isNull() and ( mText + mDesc ).count() ) {
		h = mTextSize.height() + mDescSize.height() + 16;
		w = qMax( mTextSize.width(), mDescSize.width() ) + 16;
	}

	/* No text, but we have an icon */
	else if ( not mIcon.isNull() and not ( mText + mDesc ).count() ) {
		h = mIconSize.height() + 8;
		w = mIconSize.width() + 8;
	}

	/* We have both Icon and Text */
	else {
		h = qMax( mTextSize.height() + mDescSize.height() + 12, mIconSize.height() + 8 );
		w = qMax( mTextSize.width(), mDescSize.width() ) + mIconSize.width() + 8 + 16;
	}

	/* Set the Button Size */
	mIdealSize = QSize( w, h );

	/* Repaint*/
	repaint();
};

QSize NBPushButton::minimumSizeHint() {
	/*minimumSizeHint() -> QSize

	Override the QPushButton::minimumSizeHint(...);

	@return QSize */

	return QSize( mIconSize.width() + mTextSize.width() + 8 + 16, qMax( mIconSize.height() + 8, mTextSize.height() + 12 ) );
};

QSize NBPushButton::sizeHint() {
	/*minimumSizeHint() -> QSize

	Override the QPushButton::sizeHint(...);

	@return QSize */

	return mIdealSize;
};

bool NBPushButton::isCheckable() {
	/*isCheckable() -> bool

	Override the default QPushButton::isCheckable(...)

	@return None */

	return mCheckable;
};

void NBPushButton::setCheckable( bool checkable ) {
	/*setCheckable( bool ) -> None

	Override the default QPushButton::setCheckable(...)

	@return None */

	QPushButton::setCheckable( checkable );
	mCheckable = checkable;
};

bool NBPushButton::isChecked() {
	/*isChecked() -> bool

	Override the default QPushButton::isChecked(...)

	@return None */

	return mChecked;
};

void NBPushButton::setChecked( bool checked ) {
	/*setChecked( bool ) -> None

	Override the default QPushButton::setChecked(...)

	@return None */

	if ( mCheckable ) {
		QPushButton::setChecked( checked );
		mChecked = checked;
	}
};

void NBPushButton::mousePressEvent( QMouseEvent *mEvent ) {
	/*mousePressEvent( QMouseEvent ) -> None

	Overridde the default QPushButton::mousePressEvent(...);

	@return None */

	QPushButton::mousePressEvent( mEvent );
	mBtnDown = true;

	if ( mCheckable and not mChecked )
		mChecked = true;

	else
		mChecked = false;

	repaint();

	mEvent->accept();
};

void NBPushButton::mouseReleaseEvent( QMouseEvent *mEvent ) {
	/* mouseReleaseEvent( QMouseEvent ) -> None

	Overridde the default QPushButton::mouseReleaseEvent(...);

	@return None */

	QPushButton::mouseReleaseEvent( mEvent );
	mBtnDown = false;

	repaint();

	mEvent->accept();
};

void NBPushButton::resizeEvent( QResizeEvent *rEvent ) {
	/* Overridde the default QPushButton::resizeEvent(...); */

	QPushButton::resizeEvent( rEvent );

	/* Get the new width */
	int rWidth = rEvent->size().width();

	/* Width available for description */
	if ( not mIcon.isNull() )
		rWidth -= ( mIconSize.width() + 8 );

	/* Correct the area required by the button description */
	if ( mDesc.count() )
		/* Normal weight for description */
		mDescSize = fmn->boundingRect( QRect( 0, 0, rWidth, 100 ), Qt::TextWordWrap, mDesc ).size();

	/* Reset the ideal size */
	resetSize();

	rEvent->accept();
};

void NBPushButton::paintEvent( QPaintEvent *pEvent ) {
	/* Overridde the default QPushButton::paintEvent(...); */

	QPainter painter( this );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	// Init the style option
	QStyleOptionButton btnOpt;
	btnOpt.initFrom( this );
	btnOpt.iconSize = mIconSize;
	btnOpt.rect = rect();

	if ( mBtnDown or mChecked )
		btnOpt.state = btnOpt.state | QStyle::State_Sunken;

	// Paint the QPushButton Control
	style()->drawControl( QStyle::CE_PushButtonBevel, &btnOpt, &painter, this );

	QPoint iconPos;
	QPoint textPos;
	QPoint descPos;

	// icon and text positioning
	//== Empty Icon and Empty Text
	if ( mIcon.isNull() and not ( mText + mDesc ).count() ) {
		iconPos = QPoint( 0, 0 );
		textPos = QPoint( 0, 0 );
	}

	//== Valid Icon and Empty Text
	else if ( not mIcon.isNull() and not ( mText + mDesc ).count() ) {
		iconPos = QPoint( ( width() - mIconSize.width() ) / 2, ( height() - mIconSize.height() ) / 2 );
		textPos = QPoint( 0, 0 );
	}

	//== Empty Icon and Valid Text
	else if ( mIcon.isNull() and ( mText + mDesc ).count() ) {
		iconPos = QPoint( 0, 0 );

		// Both button text and button description
		if ( mText.count() and mDesc.count() ) {
			textPos = QPoint( 8, ( height() - mTextSize.height() - mDescSize.height() ) / 2 );
			descPos = textPos + QPoint( 0, mTextSize.height() );
		}

		// Valid button text and no button description
		else if ( mText.count() and not mDesc.count() ) {
			textPos = QPoint( 8, ( height() - mTextSize.height() ) / 2 );
			descPos = QPoint( 0, 0 );
		}

		// No button text and valid button description
		else {
			textPos = QPoint( 0, 0 );
			descPos = QPoint( 8, ( height() - mDescSize.height() ) / 2 );
		}
	}

	//== Valid Icon and Valid Text
	else {
		// Text + Description + Buffer size is greater than Icon + Buffer size
		if ( mIconSize.height() + 8 < mTextSize.height() + mDescSize.height() + 12 ) {
			// Both button text and button description
			if ( mText.count() and mDesc.count() ) {
				iconPos = QPoint( 4, qMax( 4, ( height() - mTextSize.height() - mDescSize.height() ) / 2 ) );
				textPos = QPoint( mIconSize.width() + 8, 0 ) + QPoint( 0, qMax( 6, ( height() - mTextSize.height() - mDescSize.height() ) / 2 ) );
				descPos = textPos + QPoint( 0, mTextSize.height() );
			}

			// Valid button text and no button description
			else if ( mText.count() and not mDesc.count() ) {
				iconPos = QPoint( 4, qMax( 4, ( height() - mTextSize.height() ) / 2 ) );
				textPos = QPoint( mIconSize.width() + 8, 0 ) + QPoint( 0, qMax( 6, ( height() - mTextSize.height() ) / 2 ) );
				descPos = QPoint( 0, 0 );
			}

			// No button text and valid button description
			else {
				iconPos = QPoint( 4, qMax( 4, ( height() - mDescSize.height() ) / 2 ) );
				textPos = QPoint( 0, 0 );
				descPos = QPoint( mIconSize.width() + 8, 0 ) + QPoint( 0, qMax( 6, ( height() - mDescSize.height() ) / 2 ) );
			}
		}

		else {
			iconPos = QPoint( 4, qMax( 4, ( height() - mIconSize.height() ) / 2 ) );

			// Both button text and button description
			if ( mText.count() and mDesc.count() ) {
				textPos = QPoint( mIconSize.width() + 8, 0 ) + QPoint( 0, ( height() - mTextSize.height() - mDescSize.height() ) / 2 );
				descPos = textPos + QPoint( 0, mTextSize.height() );
			}

			// Valid button text and no button description
			else if ( mText.count() and not mDesc.count() ) {
				textPos = QPoint( mIconSize.width() + 8, 0 ) + QPoint( 0, ( height() - mTextSize.height() - mDescSize.height() ) / 2 );
				descPos = QPoint( 0, 0 );
			}

			// No button text and valid button description
			else {
				textPos = QPoint( 0, 0 );
				descPos = QPoint( mIconSize.width() + 8, 0 ) + QPoint( 0, ( height() - mTextSize.height() - mDescSize.height() ) / 2 );
			}
		}
	}

	/* Shift contents when pressed */
	short cShift = 0;
	if ( mBtnDown or mChecked )
		cShift = 1;

	/* Gray text when disabled */
	painter.setPen( Qt::gray );
	if ( btnOpt.state & QStyle::State_Enabled )
		painter.setPen( Qt::black );

	// Draw Icon, if available
	if ( not mIcon.isNull() )
		painter.drawPixmap( QRect( iconPos + QPoint( cShift, cShift ), mIconSize ), mIcon.pixmap( mIconSize ) );

	// Draw text, if available
	if ( mText.count() ) {
		// Painter bold text
		painter.setFont( QFont( textFont.family(), textFont.pointSize(), QFont::Bold ) );
		painter.drawText( QRect( textPos + QPoint( cShift, cShift ), mTextSize ), Qt::TextWordWrap | Qt::TextShowMnemonic, mText );
	}

	// Draw description, if available
	if ( mDesc.count() ) {
		// Painter bold text
		painter.setFont( textFont );
		painter.drawText( QRect( descPos + QPoint( cShift, cShift ), mDescSize ), Qt::TextWordWrap | Qt::TextShowMnemonic, mDesc );
	}

	painter.end();
	pEvent->accept();
};

// int main( int argc, char *argv[] ) {

	// QApplication app( argc, argv );

	// NBPushButton *Gui = new NBPushButton();

	// Gui->setIcon( QIcon::fromTheme( "view-list-icons" ) );
	// Gui->setIconSize( QSize( 48, 48 ) );
	// Gui->setText( "Icon View" );
	// Gui->setDescription( "View mode with Name below Icons" );

	// Gui->show();

	// return app.exec();
// }
