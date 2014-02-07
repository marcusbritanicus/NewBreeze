/*
	*
	* NBGuiWidgets.hpp - NBGuiWidgets.cpp header
	*
*/

#pragma once
#ifndef NBSETTINGSWIDGETS_HPP
#define NBSETTINGSWIDGETS_HPP

#include <Global.hpp>
#include <NBGuiWidgets.hpp>
#include <NBKeyBinder.hpp>
#include <NBIconProvider.hpp>

class NBSettingChooserWidget : public QListWidget {
	Q_OBJECT

	public:
		NBSettingChooserWidget();
};

class NBSCWidgetItem : public QListWidgetItem {

	public:
		NBSCWidgetItem( QIcon, QString );
};

class NBIconThemeChooserWidget : public QWidget {
	Q_OBJECT

	public:
		NBIconThemeChooserWidget();
		void loadThemes();
		QStringList themesList;

	private:
		int current;
		NBToolButton *prevBtn, *nextBtn;
		QComboBox *themeCB;

	private slots:
		void switchTheme( int );
		void nextTheme();
		void previousTheme();

	signals:
		void reloadIcons();
};

class NBIconThemeViewerWidget: public QListWidget {
	Q_OBJECT

	public:
		NBIconThemeViewerWidget();

	public slots:
		void loadIcons();
};

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
