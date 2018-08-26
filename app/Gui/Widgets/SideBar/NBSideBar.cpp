/*
	*
	* NBSideBar.cpp - SideBar class for NewBreeze
	*
*/

#include "NBSideBar.hpp"
#include "NBGuiFunctions.hpp"

NBSideBar::NBSideBar( QWidget *parent ) : QWidget( parent ) {

	setFixedWidth( 32 );

	devicesEntry = new NBSideBarEntry( ":/icons/comp.png", "Devices", this );
	connect( devicesEntry, SIGNAL( driveClicked( QString ) ), SIGNAL( driveClicked( QString ) ) );

	catalogsEntry = new NBSideBarEntry( ":/icons/catalogs.png", "Catalogs", this );
	connect( catalogsEntry, SIGNAL( driveClicked( QString ) ), SIGNAL( driveClicked( QString ) ) );

	bookmarksEntry = new NBSideBarEntry( ":/icons/bookmark.png", "Bookmarks", this );
	connect( bookmarksEntry, SIGNAL( driveClicked( QString ) ), SIGNAL( driveClicked( QString ) ) );

	quickFilesEntry = new NBSideBarEntry( ":/icons/files.png", "QuickFiles", this );
	connect( quickFilesEntry, SIGNAL( driveClicked( QString ) ), SIGNAL( driveClicked( QString ) ) );

	trashEntry = new NBSideBarTrashEntry( ":/icons/trash.png", "Trash", this );
	connect( trashEntry, SIGNAL( clicked() ), SIGNAL( showTrash() ) );

	QVBoxLayout *entryLyt = new QVBoxLayout();
	entryLyt->setContentsMargins( QMargins() );
	entryLyt->setSpacing( 0 );
	entryLyt->addWidget( devicesEntry );
	entryLyt->addWidget( catalogsEntry );
	entryLyt->addWidget( bookmarksEntry );
	entryLyt->addWidget( quickFilesEntry );
	entryLyt->addStretch();
	entryLyt->addWidget( trashEntry );

	QWidget *base = new QWidget();
	base->setObjectName( "base" );
	base->setLayout( entryLyt );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( QMargins() );
	lyt->setSpacing( 0 );
	lyt->addWidget( base );

	setLayout( lyt );

	/* No Border and margins */
	setStyleSheet( "QWidget#base{ border: none; border-right: 1px solid darkgray; }" );
	setContentsMargins( QMargins() );

	updateSideBar();
};

void NBSideBar::updateSideBar() {

	if ( not NBDeviceManager::allDrives().count() )
		devicesEntry->hide();

	if ( not NBBookmarkInfo::allBookmarks().count() )
		bookmarksEntry->hide();

	QSettings catalogs( "NewBreeze", "Catalogs" );
	int catalogCount = 0;
	Q_FOREACH( QString key, catalogs.allKeys() )
		catalogCount += catalogs.value( key ).toStringList().count();
	if ( not catalogCount )
		catalogsEntry->hide();

	QSettings superStart( "NewBreeze", "SuperStart" );
	superStart.beginGroup( "Files" );
	if ( not superStart.childKeys().count() )
		quickFilesEntry->hide();
};

void NBSideBar::flashDevices() {
};

void NBSideBar::flashCatalogs() {
};

void NBSideBar::flashBookmarks() {
};

void NBSideBar::flashQuickFiles() {
};
