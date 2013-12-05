/*
	*
	* NBGuiWidgets.cpp - The GUI classes for NewBreeze
	*
*/

#include <NBGuiWidgets.hpp>

NBSearchBar::NBSearchBar() : QWidget() {

	searchLE = new QLineEdit();
	searchLE->setFocusPolicy( Qt::ClickFocus );
	QLabel *searchLbl = new QLabel();

	if ( Settings->General.Style == "Natural" )
		searchLbl->setPixmap( QPixmap( ":icons/searchn.png" ).scaled( 20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	else
		searchLbl->setPixmap( QPixmap( ":icons/search.png" ).scaled( 20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	QHBoxLayout *srchLyt = new QHBoxLayout();
	srchLyt->setContentsMargins( QMargins() );

	srchLyt->addWidget( searchLbl );
	srchLyt->addWidget( searchLE );

	setLayout( srchLyt );

	setFixedWidth( 150 );
	setStyleSheet( getStyleSheet( "NBAddressBar", Settings->General.Style ) );
	connect( searchLE, SIGNAL( textEdited( QString ) ), this, SLOT( searchChanged( QString ) ) );
};

void NBSearchBar::searchChanged( QString query ) {

	if ( query.isEmpty() or query.isNull() )
		emit searchCleared();

	else
		emit searchString( query );
};

NBMenu::NBMenu( QWidget *parent ) : QMenu( parent ) {

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	setStyleSheet( getStyleSheet( "NBMenu", Settings->General.Style ) );
};

NBMenu::NBMenu( QString title, QWidget *parent ) : QMenu( title, parent ) {

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	setStyleSheet( getStyleSheet( "NBMenu", Settings->General.Style ) );
};

NBToolButton::NBToolButton( QString themeIcon, QString customIcon ) : QToolButton() {

	if ( customIcon.isEmpty() )
		customIcon = QString( themeIcon );

	setFixedSize( QSize( 24, 24 ) );
	setIconSize( QSize( 20, 20 ) );
	setAutoRaise( true );

	setIcon( QIcon::fromTheme( themeIcon, QIcon( customIcon ) ) );
	setStyleSheet( getStyleSheet( "NBToolButton", Settings->General.Style ) );
}

NBPushButton::NBPushButton( QIcon icon, QString text ) : QPushButton() {

	setFixedHeight( 24 );
	setMinimumWidth( 128 );
	setIconSize( QSize( 16, 16 ) );

	setIcon( icon );
	setText( text );

	setStyleSheet( getStyleSheet( "NBToolButton", Settings->General.Style ) );
};

NBTitleIcon::NBTitleIcon( QString icon ) : QLabel() {

	setPixmap( QPixmap( icon ).scaled( QSize( 21, 21 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	setContextMenuPolicy( Qt::CustomContextMenu );

	menu.setAttribute( Qt::WA_TranslucentBackground );
	menu.setStyleSheet( getStyleSheet( "NBMenu", Settings->General.Style ) );

	QAction *aboutNBAct = new QAction( QIcon( ":/icons/info.png" ), "&About NewBreeze", this );
	connect( aboutNBAct, SIGNAL( triggered() ), this, SIGNAL( aboutNB() ) );

	QAction *aboutQt4Act = new QAction( QIcon( ":/icons/info.png" ), "About &Qt4", this );
	connect( aboutQt4Act, SIGNAL( triggered() ), this, SIGNAL( aboutQt4() ) );

	addAction( aboutNBAct );

	menu.addAction( aboutNBAct );
	menu.addAction( aboutQt4Act );
};

void NBTitleIcon::mousePressEvent( QMouseEvent *mpEvent ) {

	menu.exec( QWidget::mapToGlobal( QPoint( x() + width() / 2, y() + height() / 2 ) ) );
	mpEvent->accept();
};

NBPicLabel::NBPicLabel( QPixmap pixmap, QString style ) : QLabel() {

	setPixmap( pixmap );
	Style = QString( style );
};

void NBPicLabel::mousePressEvent( QMouseEvent *mEvent ) {

	emit clicked();
	mEvent->accept();
}

NBClickLabel::NBClickLabel( QPixmap icon ) : QLabel() {

	setPixmap( icon );
	clickEnabled = false;
};

NBClickLabel::NBClickLabel( QString text ) : QLabel() {

	setText( text );
	clickEnabled = false;
};

void NBClickLabel::mousePressEvent( QMouseEvent *mEvent ) {

	if ( clickEnabled )
		emit clicked();

	mEvent->accept();
}

void NBClickLabel::setClickable( bool canClick ) {

	clickEnabled = canClick;
}

QWidget* Separator::vertical() {

	QWidget *vSep = new QWidget();
	vSep->setContentsMargins( QMargins() );
	vSep->setFixedWidth( 1 );
	vSep->setStyleSheet( "background-color: gray;" );

	return vSep;
};

QWidget* Separator::horizontal() {

	QWidget *hSep = new QWidget();
	hSep->setContentsMargins( QMargins() );
	hSep->setFixedHeight( 1 );
	hSep->setStyleSheet( "background-color: gray;" );

	return hSep;
};

NBViewModeButton::NBViewModeButton() : QToolButton() {

	setStyleSheet( getStyleSheet( "NBToolButton", Settings->General.Style ) );

	setFixedHeight( 24 );
	setMinimumWidth( 32 );

	setIcon( QIcon( ":/icons/view-choose.png" ) );
	setPopupMode( QToolButton::InstantPopup );

	connect( this, SIGNAL( clicked() ), this, SIGNAL( switchToNextView() ) );

	NBMenu *menu = new NBMenu( this );

	smallListAct = new QAction( "&Small List", this );
	smallListAct->setCheckable( true );
	connect( smallListAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	mediumListAct = new QAction( "&Large List", this );
	mediumListAct->setCheckable( true );
	connect( mediumListAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	tilesAct = new QAction( "&Tiles", this );
	tilesAct->setCheckable( true );
	connect( tilesAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	smallIconsAct = new QAction( "S&mall Icons", this );
	smallIconsAct->setCheckable( true );
	connect( smallIconsAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	normalIconsAct = new QAction( "&Normal Icons", this );
	normalIconsAct->setCheckable( true );
	connect( normalIconsAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	largeIconsAct = new QAction( "L&arge Icons", this );
	largeIconsAct->setCheckable( true );
	connect( largeIconsAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	hugeIconsAct = new QAction( "&Huge Icons", this );
	hugeIconsAct->setCheckable( true );
	connect( hugeIconsAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	smallDetailsAct = new QAction( "Small &Details", this );
	smallDetailsAct->setCheckable( true );
	connect( smallDetailsAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	normalDetailsAct = new QAction( "La&rge Details", this );
	normalDetailsAct->setCheckable( true );
	connect( normalDetailsAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	viewsGroup = new QActionGroup( this );

	menu->addAction( viewsGroup->addAction( smallListAct ) );
	menu->addAction( viewsGroup->addAction( mediumListAct ) );
	menu->addAction( viewsGroup->addAction( tilesAct ) );
	menu->addAction( viewsGroup->addAction( smallIconsAct ) );
	menu->addAction( viewsGroup->addAction( normalIconsAct ) );
	menu->addAction( viewsGroup->addAction( largeIconsAct ) );
	menu->addAction( viewsGroup->addAction( hugeIconsAct ) );
	menu->addAction( viewsGroup->addAction( smallDetailsAct ) );
	menu->addAction( viewsGroup->addAction( normalDetailsAct ) );

	setMenu( menu );

	if ( Settings->General.FolderView == QString( "SmallListView" ) )
		smallListAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "NormalListView" ) )
		mediumListAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "TilesView" ) )
		tilesAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "SmallIconsView" ) )
		smallIconsAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "NormalIconsView" ) )
		normalIconsAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "LargeIconsView" ) )
		largeIconsAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "HugeIconsView" ) )
		hugeIconsAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "SDetailsView" ) )
		smallDetailsAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "NDetailsView" ) )
		normalDetailsAct->setChecked( true );
};

int NBViewModeButton::checkedAction() {

	if ( viewsGroup->checkedAction() == smallListAct )
		return 0;

	else if ( viewsGroup->checkedAction() == mediumListAct )
		return 1;

	else if ( viewsGroup->checkedAction() == tilesAct )
		return 2;

	else if ( viewsGroup->checkedAction() == smallIconsAct )
		return 3;

	else if ( viewsGroup->checkedAction() == normalIconsAct )
		return 4;

	else if ( viewsGroup->checkedAction() == largeIconsAct )
		return 5;

	else if ( viewsGroup->checkedAction() == hugeIconsAct )
		return 6;

	else if ( viewsGroup->checkedAction() == smallDetailsAct )
		return 7;

	else if ( viewsGroup->checkedAction() == normalDetailsAct )
		return 8;

	return 4;
};

void NBViewModeButton::showMenu() {

	if ( Settings->General.FolderView == QString( "SmallListView" ) )
		smallListAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "NormalListView" ) )
		mediumListAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "TilesView" ) )
		tilesAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "SmallIconsView" ) )
		smallIconsAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "NormalIconsView" ) )
		normalIconsAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "LargeIconsView" ) )
		largeIconsAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "HugeIconsView" ) )
		hugeIconsAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "SDetailsView" ) )
		smallDetailsAct->setChecked( true );

	else if ( Settings->General.FolderView == QString( "NDetailsView" ) )
		normalDetailsAct->setChecked( true );

	QToolButton::showMenu();
};

NBInfoLabel::NBInfoLabel() : QLabel() {

	setAlignment( Qt::AlignCenter );
	setStyleSheet( "QLabel { font-family: Courier 10 Pitch, Courier New, Monotype; font-size: 12pt; }" );
};

void NBInfoLabel::setText( QString name, QString size, QString type, QString perm ) {

	QString text = QString(
		"<table style='width: 700px; height: 56px;'>"			\
		"	<tr>"												\
		"		<td colspan=3 align = 'left'>%1</td>"			\
		"	</tr>"												\
		"	<tr>"												\
		"		<td width=300 align='left'>%2</td>"				\
		"		<td width=300 align='left'>%3</td>"				\
		"		<td width=100 align='center'>%4</td>"			\
		"	</tr>"												\
		"</table>"
	);

	QLabel::setText( text.arg( name ).arg( size ).arg( type ).arg( perm ) );
};

NBDriveLabel::NBDriveLabel( const QString path ) : QWidget() {

	painter = new QPainter();
	label = QString( path );

	setFixedHeight( 32 );
	QFontMetrics fm( font() );
	setMinimumWidth( 42 + fm.maxWidth() * path.count() );
};

NBDriveLabel::~NBDriveLabel() {

	if ( painter->isActive() )
		painter->end();

	delete painter;
};

void NBDriveLabel::paintEvent( QPaintEvent *pEvent ) {

	painter->begin( this );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	QPixmap pix = QIcon::fromTheme( "drive-harddisk" ).pixmap( 28, 28 );

	if ( Settings->General.Style == "Natural" )
		painter->setPen( QColor( 0, 0, 0 ) );

	else
		painter->setPen( QColor( 255, 255, 255 ) );


	// 10 px padding along X
	int pixX = 7;

	// Centering along Y
	int pixY = ( height() - 28 ) / 2;

	// icon padding-left + icons size + text padding-left
	int textX = 7 + 28 + 7;
	// Text is painted using Qt Alignent Flags
	int textY = 0;
	// Total width - text start - text-padding-right - right icon size - icon padding-right
	int textW = width() - textX - 7 - 28 - 7;
	// Text height is full label height
	int textH = height();

	// painter->begin( this );
	painter->drawPixmap( pixX, pixY, 28, 28, pix );
	painter->drawText( textX, textY, textW, textH, Qt::AlignVCenter + Qt::TextWordWrap, label );
	painter->end();

	pEvent->accept();
};

NBDriveInfo::NBDriveInfo( qint64 used, qint64 total ) : QFrame() {

	setFrameStyle( QFrame::StyledPanel | QFrame::Plain );

	painter = new QPainter();

	percent = 1.0 * used / total;
	QString freeStr = formatSize( total - used );
	QString totalStr = formatSize( total );

	QFont xFont( font() );
	xFont.setBold( true );

	QLabel *label = new QLabel( QString( "%1 free of %2 (%3% used)" ).arg( freeStr ).arg( totalStr ).arg( 100 * percent, 0, 'f', 2 ) );
	label->setFont( xFont );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->addWidget( label );
	lyt->setAlignment( Qt::AlignCenter );

	setLayout( lyt );
};

NBDriveInfo::~NBDriveInfo() {

	if ( painter->isActive() )
		painter->end();

	delete painter;
};

void NBDriveInfo::paintEvent( QPaintEvent *pEvent ) {

	painter->begin( this );
	painter->setRenderHints( QPainter::Antialiasing );

	painter->setPen( Qt::NoPen );

	// Draw the percentage bar with the correct color
	if ( percent > 0.9 )
		painter->setBrush( QBrush( Qt::darkRed ) );

	else
		painter->setBrush( QBrush( Qt::darkGreen ) );

	painter->drawRoundedRect( 1, 1, width() * percent, height() - 2, 5, 5 );

	// Now draw a border
	painter->setPen( QColor( Qt::gray ) );
	painter->setBrush( Qt::transparent );
	painter->drawRoundedRect( 0, 0, width(), height(), 6, 6 );

	painter->end();

	pEvent->accept();
};
