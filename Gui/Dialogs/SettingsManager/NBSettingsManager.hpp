/*
	*
	* NBSettingsManager.hpp - NBSettingsManager.cpp header
	*
*/

#pragma once
#ifndef NBSETTINGSMANAGER_HPP
#define NBSETTINGSMANAGER_HPP

#include <Global.hpp>
#include <NBDialog.hpp>
#include <NBGuiWidgets.hpp>
#include <NBSettingsWidgets.hpp>

class NBViewsWidget : public QWidget {
	Q_OBJECT

	public:
		NBViewsWidget();

	private:
		void createGUI();

		QRadioButton *tlRB, *tdRB, *dbRB, *ntRB;
		QLabel *imageLbl;
		QCheckBox *nativeTitleBarCB;
		QCheckBox *TrayIconCB;
		QCheckBox *sidePanelOpen;
		QCheckBox *openWithCatalogCB;

	private slots:
		void handleThemeChanged();
		void handleNativeTitleBarToggle( bool );
		void handleTrayIconChanged( bool );
		void handleSidePanelOpenToggled( bool );
		void handleOpenWithCatalogToggled( bool );
};

class NBIconThemeWidget : public QWidget {
	Q_OBJECT

	public:
		NBIconThemeWidget();

	private:
		void createGUI();
		void updateListView();

		NBIconThemeChooserWidget *iconThemesWidget;
		NBIconThemeViewerWidget *folderViewWidget;
		QCheckBox *imagePreviewCB;

	private slots:
		void setIconTheme();
		void handleCheckStateChanged( int );
};

class NBKeyBindingsWidget : public QWidget {
	Q_OBJECT

	public:
		NBKeyBindingsWidget();

	private:
		void createGUI();
};

class NBSettingsManager : public NBDialog {
	Q_OBJECT

	public:
		NBSettingsManager();
		void addNewTerminal();

	private:
		void createGUI();

		QHBoxLayout *createBodyLayout();
		QHBoxLayout *createFooterLayout();

		void setWindowProperties();

		QStackedLayout *widgetLyt;
		NBSettingChooserWidget *chooserWidget;
		NBViewsWidget *viewWidget;
		NBIconThemeWidget *iconWidget;
		NBKeyBindingsWidget *keysWidget;

	private slots:
		void settingCategoryChosen( int );

	protected:
		void keyPressEvent( QKeyEvent* );
};

#endif
