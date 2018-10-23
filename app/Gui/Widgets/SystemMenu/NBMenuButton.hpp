/*
	*
	* NBMenuButton.hpp - The NBMenuButton Class, the Chromium style MenuButton
	*
*/

#pragma once

#include "Global.hpp"
#include "NBButtons.hpp"
#include "NBSystemMenu.hpp"

class NBMenuButton : public QToolButton {
	Q_OBJECT

	public:
		/* A button */
		NBMenuButton( QWidget *parent = NULL );

	public Q_SLOTS:
		void showMenu();

	Q_SIGNALS:
		void newWindow();

		void zoomIn();
		void zoomOut();

		void cut();
		void copy();
		void paste();

		void openVTE();

		void changeViewMode( int );

		void sortByName();
		void sortByType();
		void sortBySize();
		void sortByDate();
		void toggleHidden();
		void toggleGrouping();

		void showSettings();

		void closeWindow();
		void quit();
};
