/*
	*
	* NBSystemMenu.hpp - NewBreeze Menu Class
	*
*/

#include "NBSystemMenu.hpp"
#include "NBIconManager.hpp"
#include "NBGuiFunctions.hpp"
#include "NBGuiWidgets.hpp"

NBMenuItem::NBMenuItem( QString name, QString icon, QWidget *parent ) : QLabel( parent ) {

	mName = name;
	mIcon = icon;

	mChecked = false;
	mCheckable = false;

	setContentsMargins( 32, 0, 0, 0 );
	setText( mName );
	setAlignment( Qt::AlignVCenter );

	mHover = false;
	mPressed = false;

	setFixedHeight( 28 );

	QPalette pltt( palette() );
	pltt.setColor( QPalette::Window, Qt::transparent );
	setPalette( pltt );
};

QString NBMenuItem::name() {

	return mName;
};

QIcon NBMenuItem::icon() {

	return QIcon( mIcon );
};

void NBMenuItem::setCheckable( bool checkable ) {

	mCheckable = checkable;
	if ( not checkable )
		mChecked = false;
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

		if ( mCheckable ) {
			if ( mChecked )
				mChecked = false;

			else
				mChecked = true;
		}
	}

	repaint();
	mrEvent->accept();
};

void NBMenuItem::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setPen( Qt::NoPen );

	/* Pressed */
	if ( mPressed ) {

		painter.setBrush( palette().color( QPalette::Highlight ).darker( 110 ) );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* Hover */
	else if ( mHover ) {

		painter.setBrush( palette().color( QPalette::Highlight ).lighter() );
		painter.drawRect( QRect( 0, 0, width(), height() ) );
	}

	/* Otherwise */
	else {
		if ( mChecked ) {
			painter.setBrush( palette().color( QPalette::Window ).darker( 110 ) );
			painter.drawRect( QRect( 0, 0, width(), height() ) );
		}

		else {
			painter.setBrush( palette().color( QPalette::Window ) );
			painter.drawRect( QRect( 0, 0, width(), height() ) );
		}
	}


	painter.drawPixmap( QRect( 4, 4, 20, 20 ), QIcon( mIcon ).pixmap( 20 ) );

	painter.end();

	QLabel::paintEvent( pEvent );
	pEvent->accept();
};

NBMenuItemGroup::NBMenuItemGroup() {

	mItems.clear();
};

void NBMenuItemGroup::addMenuItem( NBMenuItem *itm ) {

	mItems << itm;
	connect( itm, SIGNAL( clicked() ), this, SLOT( makeExclusive() ) );
};

void NBMenuItemGroup::makeExclusive() {

	NBMenuItem *itm = qobject_cast<NBMenuItem*>( sender() );
	Q_FOREACH( NBMenuItem *item, mItems ) {
		if ( item != itm )
			item->setChecked( false );
	}
};

NBMenuItemButton::NBMenuItemButton( QString icon, QWidget *parent ) : QToolButton( parent ) {

	setFixedSize( 28, 28 );
	setIcon( QIcon( icon ) );

	setCheckable( true );

	setStyleSheet( "QToolButton{ border: none; border-left: 1px solid lightgray; } QToolButton:hover{ background-color: #A1DFFF; } QToolButton:checked{ background: darkgray; }" );

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
	setAttribute( Qt::WA_DeleteOnClose );

	itemsLayout = new QVBoxLayout();
	itemsLayout->setContentsMargins( QMargins( 1, 1, 1, 1 ) );
	itemsLayout->setSpacing( 0 );

	QWidget *itemsBase = new QWidget( this );
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
	connect( newWinItem, SIGNAL( clicked() ), this, SIGNAL( newWindow() ) );

	/* Zoom */
	NBMenuItemButton *zOutBtn = new NBMenuItemButton( ":/icons/category-collapse.png", this );
	zOutBtn->setStyleSheet( "QToolButton{ border: none; } QToolButton:hover{ background-color: #A1DFFF } " );
	connect( zOutBtn, SIGNAL( clicked() ), this, SIGNAL( zoomOut() ) );
	connect( zOutBtn, SIGNAL( clicked() ), this, SLOT( updateZoomOut() ) );

	NBMenuItemButton *zInBtn = new NBMenuItemButton( ":/icons/category-expand.png", this );
	zInBtn->setStyleSheet( "QToolButton{ border: none; } QToolButton:hover{ background-color: #A1DFFF } " );
	connect( zInBtn, SIGNAL( clicked() ), this, SIGNAL( zoomIn() ) );
	connect( zInBtn, SIGNAL( clicked() ), this, SLOT( updateZoomIn() ) );

	zoomLbl = new QLabel( " 48px " );

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
	connect( cutBtn, SIGNAL( clicked() ), this, SIGNAL( cut() ) );
	connect( cutBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	NBMenuItemButton *copyBtn = new NBMenuItemButton( ":/icons/copy.png", this );
	connect( copyBtn, SIGNAL( clicked() ), this, SIGNAL( copy() ) );
	connect( copyBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	NBMenuItemButton *pasteBtn = new NBMenuItemButton( ":/icons/paste.png", this );
	connect( pasteBtn, SIGNAL( clicked() ), this, SIGNAL( paste() ) );
	connect( pasteBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

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
	connect( vteItem, SIGNAL( clicked() ), this, SIGNAL( openVTE() ) );
	connect( vteItem, SIGNAL( clicked() ), this, SLOT( close() ) );

	/* View: Icons Tiles Details */
	iconsBtn = new NBMenuItemButton( ":/icons/view-icon.png", this );
	iconsBtn->setToolTip( "Icons View" );
	connect( iconsBtn, SIGNAL( clicked() ), this, SLOT( viewModesClicked() ) );

	tilesBtn = new NBMenuItemButton( ":/icons/view-list.png", this );
	tilesBtn->setToolTip( "Tiles View" );
	connect( tilesBtn, SIGNAL( clicked() ), this, SLOT( viewModesClicked() ) );

	detailsBtn = new NBMenuItemButton( ":/icons/view-details.png", this );
	detailsBtn->setToolTip( "Details View" );
	connect( detailsBtn, SIGNAL( clicked() ), this, SLOT( viewModesClicked() ) );

	QHBoxLayout *vLyt = new QHBoxLayout();
	vLyt->setContentsMargins( 32, 0, 0, 0 );
	vLyt->setSpacing( 0 );
	vLyt->addWidget( new QLabel( "View" ) );
	vLyt->addStretch();
	vLyt->addWidget( iconsBtn );
	vLyt->addWidget( tilesBtn );
	vLyt->addWidget( detailsBtn );

	QButtonGroup *viewGroup = new QButtonGroup();
	viewGroup->addButton( iconsBtn );
	viewGroup->addButton( tilesBtn );
	viewGroup->addButton( detailsBtn );
	viewGroup->setExclusive( true );

	/* Show Hidden */
	hiddenCheck = new NBMenuItemCheck( "Show Hidden", this );
	hiddenCheck->setChecked( Settings->value( "ShowHidden" ) );
	connect( hiddenCheck, SIGNAL( clicked() ), this, SLOT( close() ) );
	connect( hiddenCheck, SIGNAL( clicked() ), this, SIGNAL( toggleHidden() ) );

	QHBoxLayout *hLyt = new QHBoxLayout();
	hLyt->setContentsMargins( 10, 0, 0, 0 );
	hLyt->setSpacing( 0 );
	hLyt->addWidget( hiddenCheck );
	hLyt->addStretch();

	/* Sorting */
	groupCheck = new NBMenuItemCheck( "Show in Groups", this );
	groupCheck->setChecked( Settings->value( "Grouping" ) );
	connect( groupCheck, SIGNAL( clicked() ), this, SLOT( close() ) );
	connect( groupCheck, SIGNAL( clicked() ), this, SIGNAL( toggleGrouping() ) );

	QHBoxLayout *sLyt = new QHBoxLayout();
	sLyt->setContentsMargins( 32, 0, 0, 0 );
	sLyt->setSpacing( 0 );
	sLyt->addWidget( new QLabel( "Sort" ) );
	sLyt->addStretch();
	sLyt->addWidget( groupCheck );

	nSortBtn = new NBMenuItem( "Name", ":/icons/sort-name.png", this );
	nSortBtn->setCheckable( true );
	connect( nSortBtn, SIGNAL( clicked() ), this, SIGNAL( sortByName() ) );

	tSortBtn = new NBMenuItem( "Type", ":/icons/sort-type.png", this );
	tSortBtn->setCheckable( true );
	tSortBtn->setChecked( true );
	connect( tSortBtn, SIGNAL( clicked() ), this, SIGNAL( sortByType() ) );

	sSortBtn = new NBMenuItem( "Size", ":/icons/sort-size.png", this );
	sSortBtn->setCheckable( true );
	connect( sSortBtn, SIGNAL( clicked() ), this, SIGNAL( sortBySize() ) );

	dSortBtn = new NBMenuItem( "Date", ":/icons/sort-date.png", this );
	dSortBtn->setCheckable( true );
	connect( dSortBtn, SIGNAL( clicked() ), this, SIGNAL( sortByDate() ) );

	QGridLayout *sBtnLyt = new QGridLayout();
	sBtnLyt->setContentsMargins( 0, 0, 0, 0 );
	sBtnLyt->setSpacing( 0 );
	sBtnLyt->addWidget( nSortBtn, 0, 0 );
	sBtnLyt->addWidget( tSortBtn, 0, 1 );
	sBtnLyt->addWidget( sSortBtn, 1, 0 );
	sBtnLyt->addWidget( dSortBtn, 1, 1 );

	NBMenuItemGroup *sortGroup = new NBMenuItemGroup();
	sortGroup->addMenuItem( nSortBtn );
	sortGroup->addMenuItem( tSortBtn );
	sortGroup->addMenuItem( sSortBtn );
	sortGroup->addMenuItem( dSortBtn );

	/* Report Bug */
	NBMenuItem *reportbugItem = new NBMenuItem( "Report Bug", ":/icons/reportbug.png", this );
	connect( reportbugItem, SIGNAL( clicked() ), this, SLOT( reportBug() ) );
	connect( reportbugItem, SIGNAL( clicked() ), this, SLOT( close() ) );

	/* Settings */
	NBMenuItem *settingsItem = new NBMenuItem( "Settings", ":/icons/exec.png", this );
	connect( settingsItem, SIGNAL( clicked() ), this, SLOT( close() ) );
	connect( settingsItem, SIGNAL( clicked() ), this, SIGNAL( showSettings() ) );

	/* Close and Quit */
	NBMenuItem *closeItem = new NBMenuItem( "Close Window", ":/icons/stop.png", this );
	connect( closeItem, SIGNAL( clicked() ), this, SIGNAL( closeWindow() ) );
	connect( closeItem, SIGNAL( clicked() ), this, SLOT( close() ) );

	NBMenuItem *quitItem = new NBMenuItem( "Quit NewBreeze", ":/icons/close.png", this );
	connect( quitItem, SIGNAL( clicked() ), this, SIGNAL( quit() ) );
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
	itemsLayout->addLayout( hLyt );
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

void NBSystemMenu::viewModesClicked() {

	if ( qobject_cast<NBMenuItemButton*>( sender() ) == iconsBtn )
		emit changeViewMode( 1 );

	else if ( qobject_cast<NBMenuItemButton*>( sender() ) == tilesBtn )
		emit changeViewMode( 0 );

	else
		emit changeViewMode( 2 );
};

void NBSystemMenu::reportBug() {

	qDebug() << "Showing NewBreeze BugReporter";
	NBBugReporter *bugreport = new NBBugReporter();
	bugreport->exec();
};

void NBSystemMenu::updateZoomIn() {

	int size = zoomLbl->text().simplified().replace( "px", "" ).toInt();
	if ( size < 128 )
		zoomLbl->setText( QString( " %1px " ).arg( size + 4 ) );

	else
		zoomLbl->setText( QString( " %1px " ).arg( 128 ) );
};

void NBSystemMenu::updateZoomOut() {

	int size = zoomLbl->text().simplified().replace( "px", "" ).toInt();
	if ( size > 16 )
		zoomLbl->setText( QString( " %1px " ).arg( size - 4 ) );

	else
		zoomLbl->setText( QString( " %1px " ).arg( 16 ) );
};

void NBSystemMenu::exec( QPoint point ) {

	QString viewMode;
	int iconSize, sortColumn;
	bool grouping, hidden;

	if ( mAddress.startsWith( "NB://" ) ) {
		QString location = mAddress.replace( "NB://", "" );

		viewMode = QString( Settings->value( "View/ViewMode" ) );
		NBSettings::Scope scope = ( location == "SuperStart" ? NBSettings::SuperStart : NBSettings::Catalogs );

		if ( viewMode == "Icons" ) {
			iconSize = QSize( Settings->value( "View/IconImageSize", scope ) ).width();
			iconsBtn->setChecked( true );
		}

		else if ( viewMode == "Tiles" ) {
			iconSize = QSize( Settings->value( "View/TilesImageSize", scope ) ).width();
			tilesBtn->setChecked( true );
		}

		else {
			iconSize = QSize( Settings->value( "View/DetailsImageSize", scope ) ).width();
			detailsBtn->setChecked( true );
		}

		sortColumn = Settings->value( "NewBreeze/SortColumn" );
		grouping = Settings->value( "NewBreeze/Grouping" );
		hidden = false;
	}

	else {
		viewMode = QString( Settings->value( "View/ViewMode" ) );

		if ( viewMode == "Icons" ) {
			iconSize = QSize( Settings->value( "View/IconsImageSize" ) ).width();
			iconsBtn->setChecked( true );
		}

		else if ( viewMode == "Tiles" ) {
			iconSize = QSize( Settings->value( "View/TilesImageSize" ) ).width();
			tilesBtn->setChecked( true );
		}

		else {
			iconSize = QSize( Settings->value( "View/DetailsImageSize" ) ).width();
			detailsBtn->setChecked( true );
		}

		sortColumn = Settings->value( "SortColumn" );
		grouping = Settings->value( "Grouping" );
		hidden = Settings->value( "ShowHidden" );
	}

	/* Reset View Buttons */
	iconsBtn->setChecked( false );
	tilesBtn->setChecked( false );
	detailsBtn->setChecked( false );
	if ( viewMode == "Icons" )
		iconsBtn->setChecked( true );

	else if ( viewMode == "Tiles" )
		tilesBtn->setChecked( true );

	else
		detailsBtn->setChecked( true );

	zoomLbl->setText( QString( " %1px " ).arg( iconSize ) );
	hiddenCheck->setChecked( hidden );

	/* Reset Sort Buttons */
	nSortBtn->setChecked( false );
	sSortBtn->setChecked( false );
	tSortBtn->setChecked( false );
	dSortBtn->setChecked( false );
	if ( sortColumn == 0 )
		nSortBtn->setChecked( true );

	else if ( sortColumn == 1 )
		sSortBtn->setChecked( true );

	else if ( sortColumn == 2 )
		tSortBtn->setChecked( true );

	else
		dSortBtn->setChecked( true );

	groupCheck->setChecked( grouping );

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
