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

class NBQuickMenuBar : public QWidget {
	Q_OBJECT

	public:
		NBQuickMenuBar();

		NBPushButton *openWithBtn, *moveToBtn, *deleteBtn, *propsBtn, *permsBtn;
		QMenu openWithMenu;

	signals:
		void showOpenWithMenu();
};

#endif
