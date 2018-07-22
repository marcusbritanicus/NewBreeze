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

	devices = 0;
	catalogs = 0;
	bookmarks = 0;
	quickfiles = 0;

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

	devices = 0;
	catalogs = 0;
	bookmarks = 0;
	quickfiles = 0;

	scrollLyt = new QVBoxLayout();
	scrollLyt->setContentsMargins( QMargins() );
	scrollLyt->setSpacing( 0 );

	devicesLabel = new NBSidePanelLabel( ":/icons/comp.png", "Devices", this );
	connect( devicesLabel, SIGNAL( clicked() ), this, SLOT( populateSidePanel() ) );
	scrollLyt->addWidget( devicesLabel );
	loadDevices();
	if ( not devices )
		devicesLabel->hide();

	catalogsLabel = new NBSidePanelLabel( ":/icons/catalogs.png", "Catalogs", this );
	connect( catalogsLabel, SIGNAL( clicked() ), this, SLOT( populateSidePanel() ) );
	scrollLyt->addWidget( catalogsLabel );
	loadCatalogs();
	if ( not catalogs )
		catalogsLabel->hide();

	bookmarksLabel = new NBSidePanelLabel( ":/icons/bookmark.png", "Bookmarks", this );
	connect( bookmarksLabel, SIGNAL( clicked() ), this, SLOT( populateSidePanel() ) );
	scrollLyt->addWidget( bookmarksLabel );
	loadBookmarks();
	if ( not bookmarks )
		bookmarksLabel->hide();

	quickFilesLabel = new NBSidePanelLabel( ":/icons/files.png", "Quick Files", this );
	connect( quickFilesLabel, SIGNAL( clicked() ), this, SLOT( populateSidePanel() ) );
	scrollLyt->addWidget( quickFilesLabel );
	loadQuickFiles();
	if ( not quickfiles )
		quickFilesLabel->hide();

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
		devices++;
	}
};

void NBSidePanel::loadCatalogs() {

	QSettings ctlList( "NewBreeze", "Catalogs" );
	Q_FOREACH( QString key, ctlList.childKeys() ) {
		if ( ctlList.value( key ).toStringList().count() ) {
			NBSidePanelItem *item = new NBSidePanelItem( key, ":/icons/catalogs.png", "NB://Catalogs/" + key + "/", NBSidePanelItem::Catalogs, this );
			connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
			item->setToolTip( key + " Catalog" );
			scrollLyt->addWidget( item );
			catalogs++;
		}
	}

	ctlList.beginGroup( "Custom" );
	Q_FOREACH( QString key, ctlList.childKeys() ) {
		if ( ctlList.value( key ).toStringList().count() ) {
			NBSidePanelItem *item = new NBSidePanelItem( key, ":/icons/catalogs.png", "NB://Catalogs/" + key + "/", NBSidePanelItem::Catalogs, this );
			connect( item, SIGNAL( clicked( QString ) ), this, SIGNAL( driveClicked( QString ) ) );
			item->setToolTip( key + " Catalog" );
			scrollLyt->addWidget( item );
			catalogs++;
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
		bookmarks++;
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
		quickfiles++;
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
