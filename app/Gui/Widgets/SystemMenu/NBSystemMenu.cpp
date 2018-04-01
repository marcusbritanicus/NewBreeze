/*
	*
	* NBMenuButton.cpp - Alternate PushButton/ToolButton for NewBreeze
	*
*/

#include "NBSystemMenu.hpp"
#include "NBIconManager.hpp"
#include "NBGuiFunctions.hpp"
#include "NBGuiWidgets.hpp"

NBMenuItem::NBMenuItem( QString name, QString icon, QWidget *parent ) : QLabel( parent ) {

	mName = name;
	mIcon = icon;

	setContentsMargins( 32, 0, 0, 0 );
	setText( mName );
	setAlignment( Qt::AlignVCenter );

	mHover = false;
	mPressed = false;

	setFixedHeight( 28 );
};

QString NBMenuItem::name() {

	return mName;
};

QIcon NBMenuItem::icon() {

	return QIcon( mIcon );
};

void NBMenuItem::enterEvent( QEvent *eEvent ) {

	mHover = true;
	repaint();

	eEvent->accept();
};

void NBMenuItem::leaveEvent( QEvent *lEvent ) {

	mHover = false;
	repaint();

	lEvent->accept();
};

void NBMenuItem::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( mpEvent->button() == Qt::LeftButton )
		mPressed = true;

	repaint();
	mpEvent->accept();
};

void NBMenuItem::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	if ( mrEvent->button() == Qt::LeftButton ) {
		mPressed = false;
		emit clicked();
	}

	repaint();
	mrEvent->accept();
};

void NBMenuItem::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setPen( Qt::NoPen );

	if ( mPressed ) {

		painter.setBrush( palette().color( QPalette::Highlight ).darker( 110 ) );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* No hover */
	else if ( not mHover ) {

		painter.setBrush( palette().color( QPalette::Window ) );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* Hover */
	else if ( mHover ) {

		painter.setBrush( palette().color( QPalette::Highlight ).lighter() );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	painter.drawPixmap( QRect( 4, 4, 20, 20 ), QIcon( mIcon ).pixmap( 20 ) );

	painter.end();

	QLabel::paintEvent( pEvent );
	pEvent->accept();
};

NBMenuItemButton::NBMenuItemButton( QString icon, QWidget *parent ) : QToolButton( parent ) {

	setFixedSize( 28, 28 );
	setIcon( QIcon( icon ) );
	setStyleSheet( "QToolButton{ border: none; border-left: 1px solid lightgray; } QToolButton:hover{ background-color: #A1DFFF; } " );

	setFocusPolicy( Qt::NoFocus );
};

NBMenuItemCheck::NBMenuItemCheck( QString text, QWidget *parent ) : QCheckBox( parent ) {

	setFixedHeight( 28 );
	setText( text );

	setStyleSheet(
		"QCheckBox::indicator {"
		"	width: 13px;"
		"	height: 13px;"
		"}"
		"QCheckBox::indicator:unchecked {"
		"	image: url(:/icons/checkbox-unchecked.png);"
		"}"
		"QCheckBox::indicator:hover {"
		"	image: url(:/icons/checkbox-hover.png);"
		"}"
		"QCheckBox::indicator:checked {"
		"	image: url(:/icons/checkbox-checked.png);"
		"}"
	);

	setFocusPolicy( Qt::NoFocus );
};

NBSystemMenu::NBSystemMenu( QWidget *parent ) : QWidget( parent ) {

	setFixedWidth( 180 );
	setMinimumHeight( 48 );

	setWindowFlags( Qt::Popup );

	itemsLayout = new QVBoxLayout();
	itemsLayout->setContentsMargins( QMargins( 1, 1, 1, 1 ) );
	itemsLayout->setSpacing( 0 );

	itemsBase = new QWidget( this );
	itemsBase->setObjectName( "menuBase" );
	itemsBase->setStyleSheet( "QWidget#menuBase { border: 1px solid gray; border-radius: 2px; }" );
	itemsBase->setLayout( itemsLayout );

	QVBoxLayout *baseLayout = new QVBoxLayout();

	baseLayout->setContentsMargins( QMargins() );
	baseLayout->setSpacing( 0 );

	baseLayout->addWidget( itemsBase );

	setLayout( baseLayout );

	populateMenu();
};

void NBSystemMenu::populateMenu() {

	/* NewWindow */
	NBMenuItem *newWinItem = new NBMenuItem( "New Window", ":/icons/newwin.png", this );
	connect( newWinItem, SIGNAL( clicked() ), this, SLOT( close() ) );

	/* Zoom */
	NBMenuItemButton *zOutBtn = new NBMenuItemButton( ":/icons/category-collapse.png", this );
	zOutBtn->	setStyleSheet( "QToolButton{ border: none; } QToolButton:hover{ background-color: #A1DFFF } " );
	NBMenuItemButton *zInBtn = new NBMenuItemButton( ":/icons/category-expand.png", this );
	zInBtn->	setStyleSheet( "QToolButton{ border: none; } QToolButton:hover{ background-color: #A1DFFF } " );

	QLabel *zoomLbl = new QLabel( "  100%  " );

	QHBoxLayout *zLyt = new QHBoxLayout();
	zLyt->setContentsMargins( 32, 0, 0, 0 );
	zLyt->setSpacing( 0 );
	zLyt->addWidget( new QLabel( "Icon Size" ) );
	zLyt->addStretch();
	zLyt->addWidget( zOutBtn );
	zLyt->addWidget( zoomLbl );
	zLyt->addWidget( zInBtn );

	/* Actions: Cut Copy Paste */
	NBMenuItemButton *cutBtn = new NBMenuItemButton( ":/icons/cut.png", this );
	NBMenuItemButton *copyBtn = new NBMenuItemButton( ":/icons/copy.png", this );
	NBMenuItemButton *pasteBtn = new NBMenuItemButton( ":/icons/paste.png", this );

	QHBoxLayout *aLyt = new QHBoxLayout();
	aLyt->setContentsMargins( 32, 0, 0, 0 );
	aLyt->setSpacing( 0 );
	aLyt->addWidget( new QLabel( "Edit" ) );
	aLyt->addStretch();
	aLyt->addWidget( cutBtn );
	aLyt->addWidget( copyBtn );
	aLyt->addWidget( pasteBtn );

	/* Terminal */
	NBMenuItem *vteItem = new NBMenuItem( "Open Terminal", ":/icons/vte.png", this );
	connect( vteItem, SIGNAL( clicked() ), this, SLOT( close() ) );

	/* View: Icons Tiles Details */
	NBMenuItemButton *iconsBtn = new NBMenuItemButton( ":/icons/view-icon.png", this );
	iconsBtn->setToolTip( "Icons View" );
	NBMenuItemButton *tilesBtn = new NBMenuItemButton( ":/icons/view-list.png", this );
	tilesBtn->setToolTip( "Tiles View" );
	NBMenuItemButton *detailsBtn = new NBMenuItemButton( ":/icons/view-details.png", this );
	detailsBtn->setToolTip( "Details View" );

	QHBoxLayout *vLyt = new QHBoxLayout();
	vLyt->setContentsMargins( 32, 0, 0, 0 );
	vLyt->setSpacing( 0 );
	vLyt->addWidget( new QLabel( "View" ) );
	vLyt->addStretch();
	vLyt->addWidget( iconsBtn );
	vLyt->addWidget( tilesBtn );
	vLyt->addWidget( detailsBtn );

	/* Sorting */
	NBMenuItemCheck *groupCheck = new NBMenuItemCheck( "Show in Groups", this );
	QHBoxLayout *sLyt = new QHBoxLayout();
	sLyt->setContentsMargins( 32, 0, 0, 0 );
	sLyt->setSpacing( 0 );
	sLyt->addWidget( new QLabel( "Sort" ) );
	sLyt->addStretch();
	sLyt->addWidget( groupCheck );

	QPushButton *nSortBtn = new QPushButton( QIcon::fromTheme( "format-text-underline" ), "Name", this );
	nSortBtn->setStyleSheet( "QPushButton{border: 0px;} QPushButton:hover{border: 0px; background: #A1DFFF}" );
	nSortBtn->setFixedHeight( 28 );

	QPushButton *tSortBtn = new QPushButton( QIcon::fromTheme( "preferences-other" ), "Type", this );
	tSortBtn->setStyleSheet( "QPushButton{border: 0px;} QPushButton:hover{border: 0px; background: #A1DFFF}" );
	tSortBtn->setFixedHeight( 28 );

	QPushButton *sSortBtn = new QPushButton( QIcon( ":/icons/size.png" ), "Size", this );
	sSortBtn->setStyleSheet( "QPushButton{border: 0px;} QPushButton:hover{border: 0px; background: #A1DFFF}" );
	sSortBtn->setFixedHeight( 28 );

	QPushButton *dSortBtn = new QPushButton( QIcon::fromTheme( "office-calendar" ), "Date", this );
	dSortBtn->setStyleSheet( "QPushButton{border: 0px;} QPushButton:hover{border: 0px; background: #A1DFFF}" );
	dSortBtn->setFixedHeight( 28 );

	QGridLayout *sBtnLyt = new QGridLayout();
	sBtnLyt->setContentsMargins( 0, 0, 0, 0 );
	sBtnLyt->setSpacing( 0 );
	sBtnLyt->addWidget( nSortBtn, 0, 0 );
	sBtnLyt->addWidget( tSortBtn, 0, 1 );
	sBtnLyt->addWidget( sSortBtn, 1, 0 );
	sBtnLyt->addWidget( dSortBtn, 1, 1 );

	/* Report Bug */
	NBMenuItem *reportbugItem = new NBMenuItem( "Report Bug", ":/icons/reportbug.png", this );
	connect( reportbugItem, SIGNAL( clicked() ), this, SLOT( close() ) );

	/* Settings */
	NBMenuItem *settingsItem = new NBMenuItem( "Settings", ":/icons/exec.png", this );
	connect( settingsItem, SIGNAL( clicked() ), this, SLOT( close() ) );

	/* Close and Quit */
	NBMenuItem *closeItem = new NBMenuItem( "Close Window", ":/icons/stop.png", this );
	connect( closeItem, SIGNAL( clicked() ), this, SLOT( close() ) );
	NBMenuItem *quitItem = new NBMenuItem( "Quit NewBreeze", ":/icons/close.png", this );
	connect( quitItem, SIGNAL( clicked() ), this, SLOT( close() ) );

	/* Actual Items Layout */
	itemsLayout->addWidget( newWinItem );
	itemsLayout->addWidget( Separator::horizontal( this ) );
	itemsLayout->addLayout( zLyt );
	itemsLayout->addWidget( Separator::horizontal( this ) );
	itemsLayout->addLayout( aLyt );
	itemsLayout->addWidget( Separator::horizontal( this ) );
	itemsLayout->addWidget( vteItem );
	itemsLayout->addWidget( Separator::horizontal( this ) );
	itemsLayout->addLayout( vLyt );
	itemsLayout->addWidget( Separator::horizontal( this ) );
	itemsLayout->addLayout( sLyt );
	itemsLayout->addLayout( sBtnLyt );
	itemsLayout->addWidget( Separator::horizontal( this ) );
	itemsLayout->addWidget( reportbugItem );
	itemsLayout->addWidget( Separator::horizontal( this ) );
	itemsLayout->addWidget( settingsItem );
	itemsLayout->addWidget( Separator::horizontal( this ) );
	itemsLayout->addWidget( closeItem );
	itemsLayout->addWidget( quitItem );

	itemsLayout->addStretch( 0 );
};

void NBSystemMenu::exec( QPoint point ) {

	move( point );
	show();

	QEventLoop *loop = new QEventLoop( this );
	connect( this, SIGNAL( closed() ), loop, SLOT( quit() ) );

	loop->exec();
};

void NBSystemMenu::closeEvent( QCloseEvent *cEvent ) {

	cEvent->accept();
	emit closed();
};
