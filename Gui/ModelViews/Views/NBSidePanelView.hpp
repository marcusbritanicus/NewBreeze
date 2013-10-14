/*
	*
	* NBSidePanel.hpp - NBSidePanel.cpp header
	*
*/

#ifndef NBSIDEPANELVIEW_HPP
#define NBSIDEPANELVIEW_HPP

#include <Global.hpp>
#include <NBSidePanelModel.hpp>
#include <NBSidePanelDelegate.hpp>

class NBSidePanel : public QTreeView {
	Q_OBJECT

	public:
		NBSidePanel();

	private:
		void setupView();

		NBSidePanelModel *spModel;
		NBSidePanelDelegate *iDelegate;

	public slots:
		void updateBookmarks();
		void updateDevices();

	private slots:
		void handleClick( const QModelIndex );

	Q_SIGNALS:
		void driveClicked( QString );
};

#endif
