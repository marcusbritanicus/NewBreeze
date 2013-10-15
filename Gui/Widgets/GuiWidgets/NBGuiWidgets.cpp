/*
	*
	* NBGuiWidgets.cpp - The GUI classes for NewBreeze
	*
*/

#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>

NBSearchBar::NBSearchBar() : QWidget() {

	searchLE = new QLineEdit();
	searchLE->setFocusPolicy( Qt::ClickFocus );
	QLabel *searchLbl = new QLabel();

	if ( Settings.General.Style == "Natural" )
		searchLbl->setPixmap( QPixmap( ":icons/searchn.png" ).scaled( 20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	else
		searchLbl->setPixmap( QPixmap( ":icons/search.png" ).scaled( 20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	QHBoxLayout *srchLyt = new QHBoxLayout();
	srchLyt->setContentsMargins( QMargins() );

	srchLyt->addWidget( searchLbl );
	srchLyt->addWidget( searchLE );

	setLayout( srchLyt );

	setFixedWidth( 150 );
	setStyleSheet( getStyleSheet( "NBAddressBar", Settings.General.Style ) );
	connect( searchLE, SIGNAL( textEdited( QString ) ), this, SLOT( searchChanged( QString ) ) );
};

void NBSearchBar::searchChanged( QString query ) {

	if ( query.isEmpty() or query.isNull() )
		emit searchCleared();

	else
		emit searchString( query );
};

NBMenu::NBMenu( QWidget *parent ) : QMenu( parent ) {

	if ( ( Settings.General.Style == QString( "TransDark" ) ) or ( Settings.General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	setStyleSheet( getStyleSheet( "NBMenu", Settings.General.Style ) );
};

NBMenu::NBMenu( QString title, QWidget *parent ) : QMenu( title, parent ) {

	if ( ( Settings.General.Style == QString( "TransDark" ) ) or ( Settings.General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	setStyleSheet( getStyleSheet( "NBMenu", Settings.General.Style ) );
};

NBToolButton::NBToolButton( QString themeIcon, QString customIcon ) : QToolButton() {

	if ( customIcon.isEmpty() )
		customIcon = QString( themeIcon );

	setFixedSize( QSize( 24, 24 ) );
	setIconSize( QSize( 20, 20 ) );
	setAutoRaise( true );

	setIcon( QIcon::fromTheme( themeIcon, QIcon( customIcon ) ) );

	setStyleSheet( getStyleSheet( "NBToolButton", Settings.General.Style ) );
}

NBPushButton::NBPushButton( QIcon icon, QString text ) : QPushButton() {

	setFixedHeight( 24 );
	setMinimumWidth( 128 );
	setIconSize( QSize( 16, 16 ) );

	setIcon( icon );
	setText( text );

	setStyleSheet( getStyleSheet( "NBToolButton", settings.value( "Style" ).toString() ) );
};

NBTitleIcon::NBTitleIcon( QString icon ) : QLabel() {

	setPixmap( QPixmap( icon ).scaled( QSize( 21, 21 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	setContextMenuPolicy( Qt::CustomContextMenu );

	menu.setAttribute( Qt::WA_TranslucentBackground );
	menu.setStyleSheet( getStyleSheet( "NBMenu", Settings.General.Style ) );

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

	setStyleSheet( getStyleSheet( "NBToolButton", Settings.General.Style ) );

	setFixedHeight( 24 );
	setMinimumWidth( 32 );

	setIcon( QIcon( ":/icons/view-choose.png" ) );
	setShortcut( Settings.Shortcuts.ViewMode.at( 0 ) );
	setPopupMode( QToolButton::DelayedPopup );

	QMenu *menu = new QMenu( this );
	connect( this, SIGNAL( clicked() ), this, SIGNAL( switchToNextView() ) );

	smallListBtn = new QRadioButton( "Small List", this );
	mediumListBtn = new QRadioButton( "Large List", this );
	tilesBtn = new QRadioButton( "Tiles", this );
	smallIconsBtn = new QRadioButton( "Small Icons", this );
	normalIconsBtn = new QRadioButton( "Normal Icons", this );
	largeIconsBtn = new QRadioButton( "Large Icons", this );
	hugeIconsBtn = new QRadioButton( "Huge Icons", this );
	smallDetailsBtn = new QRadioButton( "Small Details", this );
	normalDetailsBtn = new QRadioButton( "Normal Details", this );

	smallListAct = new QWidgetAction( this );
	smallListAct->setText( "Small List" );
	smallListAct->setDefaultWidget( smallListBtn );
	connect( smallListBtn, SIGNAL( clicked() ), this, SIGNAL( changeViewMode() ) );
	connect( smallListBtn, SIGNAL( clicked() ), menu, SLOT( close() ) );

	mediumListAct = new QWidgetAction( this );
	mediumListAct->setDefaultWidget( mediumListBtn );
	connect( mediumListBtn, SIGNAL( clicked() ), this, SIGNAL( changeViewMode() ) );
	connect( mediumListBtn, SIGNAL( clicked() ), menu, SLOT( close() ) );

	tilesAct = new QWidgetAction( this );
	tilesAct->setDefaultWidget( tilesBtn );
	connect( tilesBtn, SIGNAL( clicked() ), this, SIGNAL( changeViewMode() ) );
	connect( tilesBtn, SIGNAL( clicked() ), menu, SLOT( close() ) );

	smallIconsAct = new QWidgetAction( this );
	smallIconsAct->setDefaultWidget( smallIconsBtn );
	connect( smallIconsBtn, SIGNAL( clicked() ), this, SIGNAL( changeViewMode() ) );
	connect( smallIconsBtn, SIGNAL( clicked() ), menu, SLOT( close() ) );

	normalIconsAct = new QWidgetAction( this );
	normalIconsAct->setDefaultWidget( normalIconsBtn );
	connect( normalIconsBtn, SIGNAL( clicked() ), this, SIGNAL( changeViewMode() ) );
	connect( normalIconsBtn, SIGNAL( clicked() ), menu, SLOT( close() ) );

	largeIconsAct = new QWidgetAction( this );
	largeIconsAct->setDefaultWidget( largeIconsBtn );
	connect( largeIconsBtn, SIGNAL( clicked() ), this, SIGNAL( changeViewMode() ) );
	connect( largeIconsBtn, SIGNAL( clicked() ), menu, SLOT( close() ) );

	hugeIconsAct = new QWidgetAction( this );
	hugeIconsAct->setDefaultWidget( hugeIconsBtn );
	connect( hugeIconsBtn, SIGNAL( clicked() ), this, SIGNAL( changeViewMode() ) );
	connect( hugeIconsBtn, SIGNAL( clicked() ), menu, SLOT( close() ) );

	smallDetailsAct = new QWidgetAction( this );
	smallDetailsAct->setText( "Small List" );
	smallDetailsAct->setDefaultWidget( smallDetailsBtn );
	connect( smallDetailsBtn, SIGNAL( clicked() ), this, SIGNAL( changeViewMode() ) );
	connect( smallDetailsBtn, SIGNAL( clicked() ), menu, SLOT( close() ) );

	normalDetailsAct = new QWidgetAction( this );
	normalDetailsAct->setText( "normal List" );
	normalDetailsAct->setDefaultWidget( normalDetailsBtn );
	connect( normalDetailsBtn, SIGNAL( clicked() ), this, SIGNAL( changeViewMode() ) );
	connect( normalDetailsBtn, SIGNAL( clicked() ), menu, SLOT( close() ) );

	menu->setStyleSheet( getStyleSheet( "NBMenu", Settings.General.Style ) );

	menu->addAction( smallListAct );
	menu->addAction( mediumListAct );
	menu->addAction( tilesAct );
	menu->addAction( smallIconsAct );
	menu->addAction( normalIconsAct );
	menu->addAction( largeIconsAct );
	menu->addAction( hugeIconsAct );
	menu->addAction( smallDetailsAct );
	menu->addAction( normalDetailsAct );

	setMenu( menu );

	if ( Settings.General.FolderView == QString( "SmallListView" ) )
		smallListBtn->setChecked( true );

	else if ( Settings.General.FolderView == QString( "NormalListView" ) )
		mediumListBtn->setChecked( true );

	else if ( Settings.General.FolderView == QString( "TilesView" ) )
		tilesBtn->setChecked( true );

	else if ( Settings.General.FolderView == QString( "SmallIconsView" ) )
		smallIconsBtn->setChecked( true );

	else if ( Settings.General.FolderView == QString( "NormalIconsView" ) )
		normalIconsBtn->setChecked( true );

	else if ( Settings.General.FolderView == QString( "LargeIconsView" ) )
		largeIconsBtn->setChecked( true );

	else if ( Settings.General.FolderView == QString( "HugeIconsView" ) )
		hugeIconsBtn->setChecked( true );

	else if ( Settings.General.FolderView == QString( "SDetailsView" ) )
		smallDetailsBtn->setChecked( true );

	else if ( Settings.General.FolderView == QString( "NDetailsView" ) )
		normalDetailsBtn->setChecked( true );
};

NBInfoLabel::NBInfoLabel() : QLabel() {

	setAlignment( Qt::AlignCenter );
	setStyleSheet( "QLabel { font-family: Courier 10 Pitch, Courier New, Monotype; font-size: 12pt; }" );
};

void NBInfoLabel::setText( QString size, QString type, QString perm ) {

	QString text = QString(
		"<table>"												\
		"	<tr width = 700>"									\
		"		<td width=300 align='left'>%1</td>"				\
		"		<td width=300 align='left'>%2</td>"				\
		"		<td width=100 align='center'>%3</td>"			\
		"	</tr>"												\
		"</table>"
	);

	QLabel::setText( text.arg( size ).arg( type ).arg( perm ) );
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

	if ( Settings.General.Style == "Natural" )
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

QDialog *getMessageDialog( QString title, QString icon, QString text, QString wTitle, QString wIcon, QString btns ) {

	QDialog *msgDlg = new QDialog();
	msgDlg->setAttribute( Qt::WA_TranslucentBackground );
	if ( not Settings.General.NativeTitleBar )
		msgDlg->setWindowFlags( Qt::FramelessWindowHint );

	QHBoxLayout *dlgLyt = new QHBoxLayout();
	dlgLyt->setContentsMargins( 0, 0, 0, 0 );

	QHBoxLayout *ttlLyt = new QHBoxLayout();
	QHBoxLayout *txtLyt = new QHBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	QVBoxLayout *baseLyt = new QVBoxLayout();

	ttlLyt->addWidget( new QLabel( QString( "<h3>%1</h3>" ).arg( title ) ) );
	ttlLyt->setAlignment( Qt::AlignCenter );

	QLabel *iconLbl = new QLabel();
	iconLbl->setAlignment( Qt::AlignCenter );
	iconLbl->setPixmap(  QPixmap( icon ).scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	iconLbl->setFixedWidth( 96 );

	QLabel *textLbl = new QLabel( text );
	textLbl->setWordWrap( true );
	textLbl->setAlignment( Qt::AlignLeft );

	txtLyt->addWidget( iconLbl );
	txtLyt->addWidget( textLbl );

	btnLyt->addStretch( 0 );
	if ( btns == QString( "O" ) ) {
		QPushButton *okBtn = new QPushButton( "&Ok" );
		okBtn->setObjectName( "okBtn" );
		QObject::connect( okBtn, SIGNAL( clicked() ), msgDlg, SLOT( close() ) );
		btnLyt->addWidget( okBtn );
	}

	else if ( btns == QString( "OC" ) ) {
		QPushButton *okBtn = new QPushButton( "&Ok" );
		okBtn->setObjectName( "okBtn" );
		QObject::connect( okBtn, SIGNAL( clicked() ), msgDlg, SLOT( accept() ) );

		QPushButton *cancelBtn = new QPushButton( "&Cancel" );
		cancelBtn->setObjectName( "cancelBtn" );
		QObject::connect( cancelBtn, SIGNAL( clicked() ), msgDlg, SLOT( reject() ) );

		btnLyt->addWidget( okBtn );
		btnLyt->addWidget( cancelBtn );
	}

	else if ( btns == QString( "YN" ) ) {
		QPushButton *okBtn = new QPushButton( "&Yes" );
		okBtn->setObjectName( "okBtn" );
		QObject::connect( okBtn, SIGNAL( clicked() ), msgDlg, SLOT( accept() ) );

		QPushButton *cancelBtn = new QPushButton( "&No" );
		cancelBtn->setObjectName( "cancelBtn" );
		QObject::connect( cancelBtn, SIGNAL( clicked() ), msgDlg, SLOT( reject() ) );

		btnLyt->addWidget( okBtn );
		btnLyt->addWidget( cancelBtn );
	}
	btnLyt->addStretch( 0 );

	if ( not Settings.General.NativeTitleBar )
		baseLyt->addLayout( ttlLyt );
	baseLyt->addLayout( txtLyt );
	baseLyt->addWidget( Separator::horizontal() );
	baseLyt->addLayout( btnLyt );

	QWidget *widget = new QWidget();
	widget->setObjectName( "guiBase" );
	widget->setLayout( baseLyt );

	dlgLyt->addWidget( widget );
	msgDlg->setLayout( dlgLyt );
	msgDlg->setStyleSheet( getStyleSheet( "NBDialog", Settings.General.Style ) );

	msgDlg->setWindowTitle( wTitle);
	msgDlg->setWindowIcon( QIcon( wIcon ) );

	return msgDlg;
}
