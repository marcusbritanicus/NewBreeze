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

	private:
		NBSystemMenu *menu;

	public Q_SLOTS:
		void showMenu();
		void updateViewMode( QString );

	Q_SIGNALS:
		void openLocation( QString );
		void changeViewMode( int );
};
