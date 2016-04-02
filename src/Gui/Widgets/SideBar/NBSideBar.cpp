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
	connect( dirs, SIGNAL( clicked() ), this, SIGNAL( showFolders() ) );
	connect( dirs, SIGNAL( clicked( QString ) ), this, SLOT( highlight( QString ) ) );

	apps = new NBSideBarItem( "Applications", ":/icons/applications.png", "NB://Applications", this );
	connect( apps, SIGNAL( clicked() ), this, SIGNAL( showApplications() ) );
	connect( apps, SIGNAL( clicked( QString ) ), this, SLOT( highlight( QString ) ) );

	ctlg = new NBSideBarItem( "Catalogs", ":/icons/catalogs.png", "NB://Catalogs", this );
	connect( ctlg, SIGNAL( clicked() ), this, SIGNAL( showCatalogs() ) );
	connect( ctlg, SIGNAL( clicked( QString ) ), this, SLOT( highlight( QString ) ) );

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

	setLayout( baseLayout );

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

	if ( dirs->target() == tgt ) {

		dirs->setHighlighted( true );
		apps->setHighlighted( false );
		ctlg->setHighlighted( false );

		return;
	}

	if ( apps->target() == tgt ) {

		dirs->setHighlighted( false );
		apps->setHighlighted( true );
		ctlg->setHighlighted( false );

		return;
	}

	if ( ctlg->target() == tgt ) {

		dirs->setHighlighted( false );
		apps->setHighlighted( false );
		ctlg->setHighlighted( true );

		return;
	}

	else {

		dirs->setHighlighted( false );
		apps->setHighlighted( false );
		ctlg->setHighlighted( false );
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
