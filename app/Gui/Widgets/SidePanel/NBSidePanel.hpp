/*
	*
	* NBSidePanel.hpp - SidePanel class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBSidePanelItem.hpp"

#include "NBBookmarkInfo.hpp"
#include "NBIconManager.hpp"

#include "NBTrashModel.hpp"

class NBSidePanel : public QScrollArea {
	Q_OBJECT

	public:
		NBSidePanel( QWidget *parent );

	private:
		NBSidePanelLabel *devicesLabel;
		NBSidePanelLabel *catalogsLabel;
		NBSidePanelLabel *bookmarksLabel;
		NBSidePanelLabel *quickFilesLabel;
		NBSidePanelLabel *trashLabel;

		QVBoxLayout *scrollLyt;

		int devices;
		int catalogs;
		int bookmarks;
		int quickfiles;

	public Q_SLOTS:
		void loadDevices();
		void loadCatalogs();
		void loadBookmarks();
		void loadQuickFiles();

		void highlight( QString );

	private Q_SLOTS:
		void populateSidePanel();

	Q_SIGNALS:
		void showTrash();

		void driveClicked( QString text = "NB://Catalogs" );
};
