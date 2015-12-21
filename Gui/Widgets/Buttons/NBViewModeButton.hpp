/*
	*
	* NBViewModeButton.hpp - NBViewModeButton.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBButton.hpp>

class NBViewModeButton: public NBButton {
	Q_OBJECT

	public:
		NBViewModeButton( QWidget* );
		int checkedAction();

	private :
		QAction *tilesAct, *iconsAct, *detailsAct;
		QActionGroup *viewsGroup;

	public slots:
		void showMenu();

	Q_SIGNALS:
		void changeViewMode();
		void switchToNextView();
};
