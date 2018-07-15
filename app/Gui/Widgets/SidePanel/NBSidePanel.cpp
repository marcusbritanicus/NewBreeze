/*
	*
	* NBSidePanel.cpp - SideBar class for NewBreeze
	*
*/

#include "NBSidePanel.hpp"
#include "NBGuiFunctions.hpp"

static const QString tooltipSkel = QString(
	"%1"
	"<table width = '100%' style = 'background-color: gray; font-size: 3pt;' CELLPADDING = 0 CELLSPACING = 0 >"
	"	<tr>"
	"		<td width = '%2%' style = 'background-color: %3;'></td>"
	"		<td></td>"
	"	</tr>"
	"</table>"
);

NBSidePanel::NBSidePanel( QWidget *parent ) : QScrollArea( parent ) {

	/* ScrollBar Settings */
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	/* Auto-resize widget */
	setWidgetResizable( true );

	/* No Border and margins */
	setStyleSheet( "QScrollArea{ border: none; border-right: 1px solid darkgray; }" );
	setContentsMargins( QMargins() );

	populateSidePanel();

	/* Update devices list */
	int mountsFD = open( "/proc/self/mounts", O_RDONLY, 0 );
	QSocketNotifier *devWatcher = new QSocketNotifier( mountsFD, QSocketNotifier::Write );
	connect( devWatcher, SIGNAL( activated( int ) ), this, SLOT( populateSidePanel() ) );
};

void NBSidePanel::populateSidePanel() {

	setFixedWidth( 150 );

	scrollLyt = new QVBoxLayout();
	scrollLyt->setContentsMargins( QMargins() );
	scrollLyt->setSpacing( 0 );

	devicesLabel = new NBSidePanelLabel( ":/icons/comp.png", "Devices", this );
	connect( devicesLabel, SIGNAL( clicked() ), this, SLOT( populateSidePanel() ) );
	scrollLyt->addWidget( devicesLabel );
	loadDevices();

	catalogsLabel = new NBSidePanelLabel( ":/icons/catalogs.png", "Catalogs", this );
	// connect( catalogsLabel, SIGNAL( clicked() ), this, SIGNAL( driveClicked() ) );
	scrollLyt->addWidget( catalogsLabel );
	loadCatalogs();

	bookmarksLabel = new NBSidePanelLabel( ":/icons/bookmark.png", "Bookmarks", this );
	connect( bookmarksLabel, SIGNAL( clicked() ), this, SLOT( populateSidePanel() ) );
	scrollLyt->addWidget( bookmarksLabel );
	loadBookmarks();

	quickFilesLabel = new NBSidePanelLabel( ":/icons/files.png", "Quick Files", this );
	connect( quickFilesLabel, SIGNAL( clicked() ), this, SLOT( populateSidePanel() ) );
	scrollLyt->addWidget( quickFilesLabel );
	loadQuickFiles();

	scrollLyt->addStretch();

	trashLabel = new NBSidePanelLabel( ":/icons/trash.png", "Trash", this );
	connect( trashLabel, SIGNAL( clicked() ), this, SIGNAL( showTrash() ) );
	scrollLyt->addWidget( trashLabel );

	QWidget *scrollBase = new QWidget();
	scrollBase->setLayout( scrollLyt );

	setWidget( scrollBase );
};

void NBSidePanel::loadDevices() {

	Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allDrives() ) {
		NBSidePanelItem *item = new NBSidePanelItem( info.displayName(), ":/icons/" + info.deviceType() + ".png", info.mountPoint(), NBSidePanelItem::Device, this );
		connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
		scrollLyt->addWidget( item );

		/* Special tooltip hack */
		int percent = 100 * info.bytesUsed() / info.bytesTotal();
		item->setToolTip( tooltipSkel.arg( info.mountPoint() ).arg( percent ).arg( percent < 90 ? "darkgreen" : "darkred" ) );
	}
};

void NBSidePanel::loadCatalogs() {

	QSettings ctlList( "NewBreeze", "Catalogs" );
	Q_FOREACH( QString key, ctlList.childKeys() ) {
		if ( ctlList.value( key ).toStringList().count() ) {
			NBSidePanelItem *item = new NBSidePanelItem( key, ":/icons/catalogs.png", "NB://Catalogs/" + key, NBSidePanelItem::Catalogs, this );
			connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
			item->setToolTip( key + " Catalog" );
			scrollLyt->addWidget( item );
		}
	}

	ctlList.beginGroup( "Custom" );
	Q_FOREACH( QString key, ctlList.childKeys() ) {
		if ( ctlList.value( key ).toStringList().count() ) {
			NBSidePanelItem *item = new NBSidePanelItem( key, ":/icons/catalogs.png", "NB://Catalogs/Custom/" + key, NBSidePanelItem::Catalogs, this );
			connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
			item->setToolTip( key + " Catalog" );
			scrollLyt->addWidget( item );
		}
	}
	ctlList.endGroup();
};

void NBSidePanel::loadBookmarks() {

	Q_FOREACH( NBBookmarkInfo info, NBBookmarkInfo::allBookmarks() ) {
		NBSidePanelItem *item = new NBSidePanelItem( info.displayLabel, NBIconManager::instance()->icon( "folder-favorites" ).at( 0 ), info.mountPoint, NBSidePanelItem::Bookmark, this );
		connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
		item->setToolTip( info.mountPoint );
		scrollLyt->addWidget( item );
	}
};

void NBSidePanel::loadQuickFiles() {

	QSettings qfList( "NewBreeze", "SuperStart" );
	qfList.beginGroup( "Files" );

	Q_FOREACH( QString key, qfList.allKeys() ) {
	NBSidePanelItem *item = new NBSidePanelItem( key, NBIconManager::instance()->icon( "bookmarks" ).at( 0 ), qfList.value( key ).toString(), NBSidePanelItem::QuickFile, this );
		connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
		item->setToolTip( qfList.value( key ).toString() );
		scrollLyt->addWidget( item );
	}

	qfList.endGroup();
};

void NBSidePanel::highlight( QString tgt ) {

	Q_FOREACH( NBSidePanelItem *item, findChildren<NBSidePanelItem*>() ) {
		if ( item->target() == tgt )
			item->setHighlighted( true );

		else
			item->setHighlighted( false );
	}
};
