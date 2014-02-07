/*
	*
	* NBSidePanelView.hpp - NBSidePanelView.cpp header
	*
*/

#pragma once
#ifndef NBSIDEPANELVIEW_HPP
#define NBSIDEPANELVIEW_HPP

#include <Global.hpp>
#include <NBSidePanelModel.hpp>
#include <NBSidePanelDelegate.hpp>
#include <NBGuiWidgets.hpp>

class NBSidePanelView : public QListView {
	Q_OBJECT

	public:
		NBSidePanelView();

		int idealWidth();

	private:
		void setupView();
		void showMenu( QModelIndex, QPoint );

		NBSidePanelModel *spModel;
		NBSidePanelDelegate *iDelegate;

		int mIdealWidth;
		bool showingDevices = true;

	public slots:
		void updateBookmarks();
		void updateDevices();

	private slots:
		void handleClick( const QModelIndex );

		void moveBookmarkUp();
		void moveBookmarkDown();
		void removeBookmark();

	protected:
		void mousePressEvent( QMouseEvent * );

		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );

	Q_SIGNALS:
		void driveClicked( QString );

		void copy( QStringList, QString, NBIOMode::Mode );
		void move( QStringList, QString, NBIOMode::Mode );

		void showingMenu();
		void hidingMenu();
		void bookmarkRemoved();
};

#endif
