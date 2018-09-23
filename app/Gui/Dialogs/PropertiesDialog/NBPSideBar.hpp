/*
	*
	* NBPSideBar.hpp - NBPSideBar.cpp header
	*
*/

#pragma once

#include "Global.hpp"

class NBPSideBarButton : public QToolButton {
	Q_OBJECT

	public:
		NBPSideBarButton( QWidget *parent );
};

class NBPSideBar : public QWidget {
	Q_OBJECT

	public:
		NBPSideBar( QWidget *parent );

		void setChecked( int id );
		int checked();

	private:
		void createGUI();

		NBPSideBarButton *propsBtn;
		NBPSideBarButton *permsBtn;
		NBPSideBarButton *defaultBtn;
		NBPSideBarButton *previewBtn;
		NBPSideBarButton *detailsBtn;

		QButtonGroup *btnGrp;

	public Q_SLOTS:
		void buttonClicked();

	Q_SIGNALS:
		void clicked( int );
};
