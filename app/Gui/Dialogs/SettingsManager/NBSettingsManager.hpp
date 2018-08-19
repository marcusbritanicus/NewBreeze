/*
	*
	* NBSettingsManager.hpp - NBSettingsManager.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDialog.hpp"
#include "NBGuiWidgets.hpp"
#include "NBSGeneralWidget.hpp"
#include "NBSIconsWidget.hpp"
#include "NBSKeyBindingsWidget.hpp"
#include "NBSStyleWidget.hpp"
#include "NBSSideBar.hpp"

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
		NBSStyleWidget *styleWidget;
		NBIconThemeWidget *iconWidget;
		NBKeyBindingsWidget *keysWidget;

	public slots:
		int exec();
		void cancel();

	private slots:
		void settingCategoryChosen( int );

	protected:
		void keyPressEvent( QKeyEvent* );
};
