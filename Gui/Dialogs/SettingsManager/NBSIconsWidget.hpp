/*
	*
	* NBSGeneralWidget.hpp - NBSGeneralWidget.cpp header
	*
*/

#pragma once
#ifndef NBSICONSWIDGET_HPP
#define NBSICONSWIDGET_HPP

#include <Global.hpp>
#include <NBButton.hpp>
#include <NBIconProvider.hpp>

class NBIconThemeChooserWidget;
class NBIconThemeViewerWidget;

class NBIconThemeWidget : public QWidget {
	Q_OBJECT

	public:
		NBIconThemeWidget();

	private:
		void createGUI();
		void updateListView();

		NBIconThemeChooserWidget *iconThemesWidget;
		NBIconThemeViewerWidget *folderViewWidget;

	private slots:
		void setIconTheme();
};

class NBIconThemeChooserWidget : public QWidget {
	Q_OBJECT

	public:
		NBIconThemeChooserWidget();
		void loadThemes();
		QStringList themesList;

	private:
		int current;
		NBButton *prevBtn, *nextBtn;
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

#endif
