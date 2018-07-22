/*
	*
	* NBCrumb.cpp - Alternate PushButton/ToolButton for NewBreeze
	*
*/

#include "NBCrumb.hpp"

NBCrumb::NBCrumb( QString text, bool currentPath ) : QWidget() {

	/* Track mouse movements */
	setMouseTracking( true );

	/* NoFocus */
	setFocusPolicy( Qt::NoFocus );

	/* Font */
	setFont( QFont( "DejaVu Sans Mono", 9 ) );

	if ( text == "/" ) {
		/* Set the root path */
		mPath = "/";
		mCrumbText = "Root";
	}

	else if ( QFileInfo( text ).canonicalFilePath() == QDir::homePath() ) {

		mPath = text;
		mCrumbText = ":HOME:";
	}

	else {
		/* Set the other path */
		mPath = text;
		mCrumbText = baseName( text );
	}

	cwd = QDir( text );
	cwd.setFilter( QDir::Dirs | QDir::NoDotAndDotDot );

	/* Current Dir? */
	m_Current = currentPath;

	/* Set the size of the crumb: buffer 10 ( maxSize = 500 ) */
	if ( currentPath and mCrumbText != ":HOME:" ) {
		/* If this is the directory we are showing, make it bold */
		QFontMetrics fm( QFont( font().family(), font().pointSize(), QFont::Bold ) );
		/* width = text width + menu button + buffer */
		mWidth = fm.width( mCrumbText ) + 16 + 10;
		if ( mWidth > 500 ) {
			mWidth = 500;
			/* Buffer 10, hence width: 490 */
			mCrumbText = fm.elidedText( mCrumbText, Qt::ElideMiddle, 480 );
		}
	}

	else if ( mCrumbText == ":HOME:" ) {

		/* Icon width + arrow width + buffer */
		mWidth = 32 + 16 + 10;
	}

	else {
		/* Otherwise, normal font suffices */
		QFontMetrics fm( font() );
		/* width = text width + menu button + buffer */
		mWidth = fm.width( mCrumbText ) + 16 + 10;
		if ( mWidth > 500 ) {
			mWidth = 500;
			/* Buffer 10, hence width: 490 */
			mCrumbText = fm.elidedText( mCrumbText, Qt::ElideMiddle, 480 );
		}
	}

	/* Crumb was pressed */
	m_Pressed = false;

	/* Menu button was pressed */
	m_MenuButtonPressed = false;

	/* Tooltip */
	setToolTip( mPath );

	/* Cursor Shape */
	if ( not currentPath )
		setCursor( Qt::PointingHandCursor );

	setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
	updateGeometry();
};

QSize NBCrumb::sizeHint() const {

	return QSize( mWidth, 24 );
};

QString NBCrumb::path() {

	return mPath;
};

void NBCrumb::showMenu() {

	m_MenuButtonPressed = true;
	repaint();

	QMenu *menu = new QMenu( this );
	if ( mPath.startsWith( "NB://SuperStart" ) ) {
		QSettings sett( "NewBreeze", "SuperStart" );
		sett.beginGroup( "Places" );

		if ( sett.childKeys().count() ) {
			foreach( QString dir, sett.childKeys() ) {
				if ( not exists( dir ) )
					continue;

				QAction *act = new QAction( QIcon::fromTheme( "folder" ), baseName( dir ), this );
				act->setData( sett.value( dir ) );
				connect( act, SIGNAL( triggered() ), this, SLOT( onMenuItemClicked() ) );
				menu->addAction( act );
			}
		}

		else {
			QAction *action = menu->addAction( QIcon::fromTheme( "folder" ), "No folders" );
			action->setDisabled( true );
		}

		sett.endGroup();
	}

	else if ( mPath.startsWith( "NB://Catalogs" ) ) {
		QSettings sett( "NewBreeze", "Catalogs" );

		Q_FOREACH( QString key, sett.childKeys() ) {
			if ( not sett.value( key ).toStringList().count() )
				continue;

			QAction *act = new QAction( QIcon::fromTheme( "folder" ), key, this );
			act->setData( "NB://Catalogs/" + key );
			connect( act, SIGNAL( triggered() ), this, SLOT( onMenuItemClicked() ) );
			menu->addAction( act );
		}

		sett.beginGroup( "Custom" );
		Q_FOREACH( QString key, sett.childKeys() ) {
			if ( not sett.value( key ).toStringList().count() )
				continue;

			QAction *act = new QAction( QIcon::fromTheme( "folder" ), key, this );
			act->setData( "NB://Catalogs/" + key );
			connect( act, SIGNAL( triggered() ), this, SLOT( onMenuItemClicked() ) );
			menu->addAction( act );
		}
		sett.endGroup();
	}

	else if ( mPath.startsWith( "NB://" ) ) {

		QAction *action = menu->addAction( QIcon::fromTheme( "folder" ), "No folders" );
		action->setDisabled( true );
	}

	else {

		if ( cwd.entryList().count() ) {
			foreach( QString dir, cwd.entryList() ) {
				QAction *act = new QAction( QIcon::fromTheme( "folder" ), dir, this );
				act->setData( cwd.absoluteFilePath( dir ) );
				connect( act, SIGNAL( triggered() ), this, SLOT( onMenuItemClicked() ) );
				menu->addAction( act );
			}

		}

		else {
			QAction *action = menu->addAction( QIcon::fromTheme( "folder" ), "No folders" );
			action->setDisabled( true );
		}
	}

	menu->exec( mapToGlobal( rect().bottomLeft() ) );

	m_MenuButtonPressed = false;
	repaint();
};

void NBCrumb::onMenuItemClicked() {

	emit loadPath( qobject_cast<QAction *>( sender() )->data().toString() );
};

void NBCrumb::mousePressEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() != Qt::LeftButton ) {

		mEvent->ignore();
		return;
	}

	QPoint clickPos = mEvent->pos();

	/* If a menu has been set, and we clicked the menu button */
	if ( QRect( mWidth - 16, 0, 16, 24 ).contains( clickPos ) )
		showMenu();

	/* Not a menu click */
	else {
		m_Pressed = true;
	}

	mEvent->accept();
};

void NBCrumb::mouseReleaseEvent( QMouseEvent *mEvent ) {

	m_Pressed = false;
	emit loadPath( mPath );

	repaint();

	mEvent->accept();
};

void NBCrumb::enterEvent( QEvent *eEvent ) {

	repaint();
	eEvent->accept();
};

void NBCrumb::leaveEvent( QEvent *lEvent ) {

	repaint();
	lEvent->accept();
};

void NBCrumb::paintEvent( QPaintEvent *pEvent ) {

	QPainter *painter = new QPainter( this );

	/* Clear the paint area */
	painter->fillRect( rect(), Qt::transparent );

	/* Paint a background */
	if ( m_MenuButtonPressed or m_Pressed ) {
		painter->save();
		painter->setPen( Qt::NoPen );
		painter->setRenderHint( QPainter::Antialiasing );
		painter->setBrush( palette().color( QPalette::Highlight ) );
		painter->drawRoundedRect( QRect( QPoint( 0, 0 ), QSize( mWidth, 14 ) ), 3.0 ,3.0 );
		painter->drawRect( QRect( 0, 10, mWidth, 14 ) );
		painter->restore();
	}

	else if (  rect().contains( mapFromGlobal( QCursor::pos() ) ) ) {
		painter->save();
		painter->setPen( Qt::NoPen );
		painter->setRenderHint( QPainter::Antialiasing );
		painter->setBrush( palette().color( QPalette::Highlight ).lighter( 175 ) );
		painter->drawRoundedRect( QRect( QPoint( 0, 0 ), QSize( mWidth, 14 ) ), 3.0 ,3.0 );
		painter->drawRect( QRect( 0, 10, mWidth, 14 ) );
		painter->restore();
	}

	/* Width if menu is not there: effectiveWidth */
	int eWidth = mWidth - 16;

	/* painter pen color */
	if ( m_MenuButtonPressed or m_Pressed )
		painter->setPen( palette().color( QPalette::HighlightedText ) );

	else if ( rect().contains( mapFromGlobal( QCursor::pos() ) ) )
		painter->setPen( palette().color( QPalette::HighlightedText ).darker( 175 ) );

	else
		painter->setPen( palette().color( QPalette::WindowText ) );

	/* painter font */
	if ( m_Current )
		painter->setFont( QFont( font().family(), font().pointSize(), QFont::Bold ) );

	/* Draw the text */
	if ( mCrumbText == ":HOME:" )
		painter->drawPixmap( QRect( 8, 0, 24, 24 ), QIcon::fromTheme( "go-home", QIcon( ":/icons/home.png" ) ).pixmap( 24 ) );

	else
		painter->drawText( QRect( 1, 1, eWidth, 24 ), Qt::AlignCenter, mCrumbText );

	/* Draw the menu */
	if ( m_MenuButtonPressed ) {
		/* We are showing a menu, hence, arrow down */
		QStyleOption options;
		options.rect = QRect( eWidth, 0, 16, 24 );
		options.state = QStyle::State_MouseOver | QStyle::State_Enabled;
		options.palette = palette();

		qApp->style()->drawPrimitive( QStyle::PE_IndicatorArrowDown, &options, painter );
	}

	else {
		/* Normal situation, arrow right */
		QStyleOption options;
		options.rect = QRect( eWidth, 0, 16, 24 );
		options.state = QStyle::State_Off | QStyle::State_Enabled;
		options.palette = palette();

		qApp->style()->drawPrimitive( QStyle::PE_IndicatorArrowRight, &options, painter );
	}

	painter->end();

	pEvent->accept();
};
