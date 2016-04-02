/*
	*
	* NBSidePanel.hpp - NBSidePanel.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBFlashLabel.hpp>
#include <NBDeviceView.hpp>
#include <NBVfsView.hpp>
#include <NBBookmarkView.hpp>

class NBSidePanel : public QWidget {
	Q_OBJECT

	public :
		NBSidePanel( QWidget *parent );

	private:
		void populateSidePanel();

		NBFlashLabel *dirLbl, *appLbl, *ctlLbl;
		NBTrashLabel *trashLabel;
		NBDevicesIcon *devIcon;
		NBVfsIcon *vfsIcon;
		NBBookmarksIcon *bmkIcon;

	protected :
		void mousePressEvent( QMouseEvent *mEvent );
		void mouseMoveEvent( QMouseEvent *mEvent );

	public slots:
		void flashApplications();
		void flashCatalogs();
		void flashFolders();
		void flashDevices();
		void flashVfs();
		void flashBookmarks();

	Q_SIGNALS:
		void showFolders();
		void showApplications();
		void showCatalogs();

		void showTrash();

		void driveClicked( QString );
};
