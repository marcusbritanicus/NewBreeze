/*
	*
	* NBQuickMenuBar.cpp - NewBreeze AddressBar Header
	*
*/

#ifndef NBQUICKMENUBAR_HPP
#define NBQUICKMENUBAR_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>
#include <NBButtons.hpp>

class NBQuickMenuBar : public QWidget {
	Q_OBJECT

	public:
		NBQuickMenuBar();

		NBButtons *quickBtns;
		NBMenu openWithMenu;

	Q_SIGNALS:
		void showOpenWithMenu();

		void openWithClicked();
		void deleteClicked();
		void propsClicked();
		void permsClicked();
};

#endif
