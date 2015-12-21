/*
	*
	* NBSettingsManager.hpp - NBSettingsManager.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBDialog.hpp>
#include <NBGuiWidgets.hpp>
#include <NBSGeneralWidget.hpp>
#include <NBSIconsWidget.hpp>
#include <NBSettingsWidgets.hpp>
#include <NBSKeyBindingsWidget.hpp>
#include <NBSSideBar.hpp>

class NBViewsWidget : public QWidget {
	Q_OBJECT

	public:
		NBViewsWidget();

	private:
		void createGUI();

		QRadioButton *defaultThemeRB;

	private slots:
		// void handleThemeChanged();
};

class NBSettingsManager : public NBDialog {
	Q_OBJECT

	public:
		NBSettingsManager( QWidget *parent = 0 );
		void addNewTerminal();

	private:
		void createGUI();

		QHBoxLayout *createBodyLayout();
		QHBoxLayout *createFooterLayout();

		void setWindowProperties();

		QStackedLayout *widgetLyt;
		NBSSideBar *chooserWidget;
		NBViewsWidget *viewWidget;
		NBIconThemeWidget *iconWidget;
		NBKeyBindingsWidget *keysWidget;

	private slots:
		void settingCategoryChosen( int );

	protected:
		void keyPressEvent( QKeyEvent* );
};
