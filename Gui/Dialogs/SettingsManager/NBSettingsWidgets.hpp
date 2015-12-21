/*
	*
	* NBGuiWidgets.hpp - NBGuiWidgets.cpp header
	*
*/

#pragma once
#ifndef NBSETTINGSWIDGETS_HPP
#define NBSETTINGSWIDGETS_HPP

#include <Global.hpp>
#include <NBButton.hpp>
#include <NBIconProvider.hpp>

class NBShortcutsWidget: public QWidget {
	Q_OBJECT

	public:
		NBShortcutsWidget( QString );
		static QSettings shortcutSettings;

	private:
		QPushButton *setKeyBindingBtn;
		QToolButton *clearBindingBtn;
		QToolButton *resetBindingBtn;
		QString action;

	private slots:
		void changeKeySequence();
		void resetBinding();
		void clearBinding();
};

#endif
