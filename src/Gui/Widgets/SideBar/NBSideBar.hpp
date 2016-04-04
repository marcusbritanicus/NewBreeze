/*
	*
	* NBSideBar.hpp - SideBar class Header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBSideBarGroup.hpp>
#include <NBSideBarItem.hpp>

#include <NBDeviceInfo.hpp>
#include <NBBookmarkInfo.hpp>

#include <NBIconProvider.hpp>

class NBSideBar : public QWidget {
	Q_OBJECT

	public:
		NBSideBar( QWidget *parent );

	private:
		void populateSideBar();

		NBSideBarItem *dirs;
		NBSideBarItem *apps;
		NBSideBarItem *ctlg;

		NBSideBarGroup *drives;
		NBSideBarGroup *vfs;
		NBSideBarGroup *bookmarks;

		NBSideBarItem *trash;

	public Q_SLOTS:
		void reloadDevices();
		void reloadBookmarks();
		void highlight( QString );

	protected:
		void paintEvent( QPaintEvent* );

	Q_SIGNALS:
		void showFolders();
		void showApplications();
		void showCatalogs();

		void showTrash();

		void driveClicked( QString );
};
