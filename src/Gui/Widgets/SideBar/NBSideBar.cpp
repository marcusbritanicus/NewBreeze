/*
	*
	* NBSideBar.cpp - SideBar class for NewBreeze
	*
*/

#include <NBSideBar.hpp>

NBSideBar::NBSideBar( QWidget *parent ) : QWidget( parent ) {

	populateSideBar();

	reloadDevices();
	reloadBookmarks();

	// Update devices list
	int mountsFD = open( "/proc/self/mounts", O_RDONLY, 0 );
	QSocketNotifier *devWatcher = new QSocketNotifier( mountsFD, QSocketNotifier::Write );
	connect( devWatcher, SIGNAL( activated( int ) ), this, SLOT( reloadDevices() ) );
};

void NBSideBar::populateSideBar() {

	dirs = new NBSideBarItem( "Folders", ":/icons/show-folders.png", "NB://Folders", this );
	connect( dirs, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );

	apps = new NBSideBarItem( "Applications", ":/icons/applications.png", "NB://Applications", this );
	connect( apps, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );

	ctlg = new NBSideBarItem( "Catalogs", ":/icons/catalogs.png", "NB://Catalogs", this );
	connect( ctlg, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );

	drives = new NBSideBarGroup( "Devices", ":/icons/comp.png", this );
	connect( drives, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );

	vfs = new NBSideBarGroup( "VFS Mounts", ":/icons/encfs.png", this );
	connect( vfs, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );

	bookmarks = new NBSideBarGroup( "Bookmarks", ":/icons/bookmark.png", this );
	connect( bookmarks, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );

	trash = new NBSideBarItem( "Trash", ":/icons/trash.png", "NB://Trash", this );
	connect( trash, SIGNAL( clicked() ), this, SIGNAL( showTrash() ) );

	QVBoxLayout *baseLayout = new QVBoxLayout();
	baseLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
	baseLayout->setSpacing( 10 );

	baseLayout->addWidget( dirs );
	baseLayout->addWidget( apps );
	baseLayout->addWidget( ctlg );
	baseLayout->addWidget( drives );
	baseLayout->addWidget( vfs );
	baseLayout->addWidget( bookmarks );
	baseLayout->addStretch();
	baseLayout->addWidget( trash );

	QWidget *base = new QWidget( this );
	base->setObjectName( "base" );
	base->setLayout( baseLayout );

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->setContentsMargins( QMargins( 1, 1, 1, 1 ) );
	lyt->setSpacing( 0 );

	lyt->addWidget( base );

	setLayout( lyt );

	setFixedWidth( 150 );
};

void NBSideBar::reloadDevices() {

	drives->clear();
	vfs->clear();

	Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allDrives() )
		drives->addItem( info.driveLabel(), ":/icons/" + info.driveType() + ".png", info.mountPoint() );

	Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allVirtualMounts() )
		vfs->addItem( info.driveLabel(), ":/icons/" + info.driveType() + ".png", info.mountPoint() );
};

void NBSideBar::reloadBookmarks() {

	bookmarks->clear();

	Q_FOREACH( NBBookmarkInfo info, NBBookmarkInfo::allBookmarks() )
		bookmarks->addItem( info.displayLabel, NBIconProvider::themeIcon( "bookmarks" ), info.mountPoint );
};

void NBSideBar::highlight( QString tgt ) {

	drives->clearHighlights();
	vfs->clearHighlights();
	bookmarks->clearHighlights();

	dirs->setHighlighted( false );
	apps->setHighlighted( false );
	ctlg->setHighlighted( false );

	if ( tgt.startsWith( "NB://Applications" ) ) {

		dirs->setHighlighted( false );
		apps->setHighlighted( true );
		ctlg->setHighlighted( false );

		return;
	}

	if ( tgt.startsWith( "NB://Catalogs" ) ) {

		dirs->setHighlighted( false );
		apps->setHighlighted( false );
		ctlg->setHighlighted( true );

		return;
	}

	for( int i = 0; i < drives->itemCount(); i++ ) {

		NBSideBarItem *item = drives->item( i );
		if ( item->target() == tgt ) {
			item->setHighlighted( true );
			dirs->setHighlighted( false );
			apps->setHighlighted( false );
			ctlg->setHighlighted( false );
			return;
		}
	}

	for( int i = 0; i < vfs->itemCount(); i++ ) {

		NBSideBarItem *item = vfs->item( i );
		if ( item->target() == tgt ) {
			item->setHighlighted( true );
			dirs->setHighlighted( false );
			apps->setHighlighted( false );
			ctlg->setHighlighted( false );
			return;
		}
	}

	for( int i = 0; i < bookmarks->itemCount(); i++ ) {

		NBSideBarItem *item = bookmarks->item( i );
		if ( item->target() == tgt ) {
			item->setHighlighted( true );
			dirs->setHighlighted( false );
			apps->setHighlighted( false );
			ctlg->setHighlighted( false );
			return;
		}
	}

	dirs->setHighlighted( true );
};

void NBSideBar::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setRenderHint( QPainter::HighQualityAntialiasing );
	painter.setPen( Qt::gray );

	painter.drawRoundedRect( QRectF( rect().adjusted( 0, 0, -1, -1 ) ), 2.0, 2.0 );
	painter.end();

	pEvent->accept();
};
