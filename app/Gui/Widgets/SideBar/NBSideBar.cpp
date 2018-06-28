/*
	*
	* NBSideBar.cpp - The side bar showing mounted drives and bookmarks
	*
*/

#include "NBSideBar.hpp"

/* Side Panel init */
NBSideBar::NBSideBar( QWidget *parent ) : QWidget( parent ) {

	setFixedWidth( 48 );
	setMouseTracking( true );

	populateSidePanel();
};

void NBSideBar::populateSidePanel() {

	/* The 'Folders' Label */
	dirLbl = new NBFlashLabel( this );
	dirLbl->setPixmap( QPixmap( ":/icons/show-folders.png" ).scaled( QSize( 32, 32 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	dirLbl->setFixedSize( QSize( 48, 48 ) );
	dirLbl->setToolTip( "Show folders" );
	connect( dirLbl, SIGNAL( clicked() ), this, SIGNAL( showFolders() ) );

	/* The 'Applications' Label */
	appLbl = new NBFlashLabel( this );
	appLbl->setPixmap( QPixmap( ":/icons/applications.png" ).scaled( QSize( 32, 32 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	appLbl->setFixedSize( QSize( 48, 48 ) );
	appLbl->setToolTip( "Show Applications" );
	connect( appLbl, SIGNAL( clicked() ), this, SIGNAL( showApplications() ) );

	/* The 'Catalogs' Label */
	ctlLbl = new NBFlashLabel( this );
	ctlLbl->setPixmap( QPixmap( ":/icons/catalogs.png" ).scaled( QSize( 32, 32 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	ctlLbl->setFixedSize( QSize( 48, 48 ) );
	ctlLbl->setToolTip( "Show Catalogs" );
	connect( ctlLbl, SIGNAL( clicked() ), this, SIGNAL( showCatalogs() ) );

	/* The 'Devices' Label */
	devIcon = new NBDevicesIcon( this );
	connect( devIcon, SIGNAL( driveClicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
	QAction *showDevicesAct = new QAction( QIcon( ":/icons/comp.png" ), "Show &Devices", devIcon );
	showDevicesAct->setShortcut( tr( "Alt+D" ) );
	connect( showDevicesAct, SIGNAL( triggered() ), devIcon, SLOT( showDevices() ) );
	addAction( showDevicesAct );

	/* The 'VirtualFS Mounts' Label */
	vfsIcon = new NBVfsIcon( this );
	connect( vfsIcon, SIGNAL( driveClicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
	QAction *showVfsAct = new QAction( QIcon( ":/icons/encfs.png" ), "Show &VFS", vfsIcon );
	showVfsAct->setShortcut( tr( "Alt+V" ) );
	connect( showVfsAct, SIGNAL( triggered() ), vfsIcon, SLOT( showDevices() ) );
	addAction( showVfsAct );

	/* The 'Bookmarks' Label */
	bmkIcon = new NBBookmarksIcon( this );
	connect( bmkIcon, SIGNAL( driveClicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
	QAction *showBookmarksAct = new QAction( QIcon( ":/icons/bookmark.png" ), "Show &Bookmarks", devIcon );
	showBookmarksAct->setShortcut( tr( "Alt+B" ) );
	connect( showBookmarksAct, SIGNAL( triggered() ), bmkIcon, SLOT( showBookmarks() ) );
	addAction( showBookmarksAct );

	/* The 'Trash' Label */
	trashLabel = new NBTrashLabel( this );
	trashLabel->setFixedSize( QSize( 47, 48 ) );
	trashLabel->setToolTip( "Show TrashCan" );
	connect( trashLabel, SIGNAL( clicked() ), this, SIGNAL( showTrash() ) );

	/* No Margins */
	setContentsMargins( QMargins() );

	/* Layout */
	QVBoxLayout *baseLyt = new QVBoxLayout();

	/* No margins or widget spacing */
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setSpacing( 0 );

	/* Add the various icons */
	baseLyt->addWidget( dirLbl );
	baseLyt->addWidget( appLbl );
	baseLyt->addWidget( ctlLbl );
	baseLyt->addWidget( devIcon );
	baseLyt->addWidget( vfsIcon );
	baseLyt->addWidget( bmkIcon );
	baseLyt->addStretch();
	baseLyt->addWidget( trashLabel );

	QWidget *base = new QWidget( this );
	base->setObjectName( "base" );
	base->setLayout( baseLyt );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( QMargins() );
	lyt->setSpacing( 0 );
	lyt->addWidget( base );

	setLayout( lyt );

	/* Styling */
	setStyleSheet( "#base{ border-right: 1px solid darkgray; }" );
};

void NBSideBar::mousePressEvent( QMouseEvent *mEvent ) {

	mEvent->accept();
};

void NBSideBar::mouseMoveEvent( QMouseEvent *mEvent ) {

	mEvent->accept();
};

void NBSideBar::flashApplications() {

	appLbl->flashLabel();
};

void NBSideBar::flashCatalogs() {

	ctlLbl->flashLabel();
};

void NBSideBar::flashFolders() {

	dirLbl->flashLabel();
};

void NBSideBar::flashDevices() {

	devIcon->flashLabel();
};

void NBSideBar::flashVfs() {

	vfsIcon->flashLabel();
};

void NBSideBar::flashBookmarks() {

	bmkIcon->flashLabel();
};
