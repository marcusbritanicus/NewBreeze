/*
	*
	* NBQuickMenu.cpp - NewBreeze AddressBar Header
	*
*/

#pragma once
#ifndef NBQUICKMENU_HPP
#define NBQUICKMENU_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>
#include <NBButton.hpp>

class NBQuickMenu : public QWidget {
	Q_OBJECT

	public:
		NBQuickMenu( QWidget *parent = NULL );

	protected :
		void keyPressEvent( QKeyEvent* );

	Q_SIGNALS:
		void showOpenWithMenu();

		void openWithClicked();
		void deleteClicked();
		void propsClicked();
		void permsClicked();
};

#endif
