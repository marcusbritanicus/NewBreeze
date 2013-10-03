/*
	*
	* NBSidePanel.hpp - NBSidePanel.cpp header
	*
*/

#ifndef NBSIDEPANEL_HPP
#define NBSIDEPANEL_HPP

#include <Global.hpp>
#include <NBSidePanelModel.hpp>

class NBSidePanel : public QTreeView {
	Q_OBJECT

	public:
		NBSidePanel();

	private:
		void setupView();

		NBSidePanelModel *spModel;

	public slots:
		void updateBookmarks();
		void updateDevices();

	private slots:
		void handleClick( const QModelIndex );

	Q_SIGNALS:
		void driveClicked( QString );
};

#endif
