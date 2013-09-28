/*
	*
	* NBSidePanel.hpp - NBSidePanel.cpp header
	*
*/

#ifndef NBSIDEPANEL_HPP
#define NBSIDEPANEL_HPP

#include <Global.hpp>

class NBSidePanel : public QWidget {
    Q_OBJECT

	public:
		NBSidePanel();

	private:
		void createGUI();
		void setPanelProperties();

		int sidePanelWidth;

	signals:
		void driveClicked( QString mountPoint );
};

#endif
