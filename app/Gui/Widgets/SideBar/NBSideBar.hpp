/*
	*
	* NBSideBar.hpp - SideBar class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBSideBarEntry.hpp"
#include "NBSideBarMenu.hpp"

#include "NBBookmarkInfo.hpp"
#include "NBIconManager.hpp"

#include "NBTrashModel.hpp"

class NBSideBar : public QWidget {
	Q_OBJECT

	public:
		NBSideBar( QWidget *parent );

	private:
		NBSideBarEntry *devicesEntry;
		NBSideBarEntry *catalogsEntry;
		NBSideBarEntry *bookmarksEntry;
		NBSideBarEntry *quickFilesEntry;

		NBSideBarTrashEntry *trashEntry;

		QVBoxLayout *scrollLyt;

		int devices;
		int catalogs;
		int bookmarks;
		int quickfiles;

	public Q_SLOTS:
		void flashDevices();
		void flashCatalogs();
		void flashBookmarks();
		void flashQuickFiles();

	private Q_SLOTS:
		void updateSideBar();

	Q_SIGNALS:
		void showTrash();

		void driveClicked( QString text = "NB://Catalogs" );
};
