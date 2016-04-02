/*
    *
    * NBSettings.hpp - NBSettings class header
    *
*/

#pragma once
#ifndef NBSETTINGS_HPP
#define NBSETTINGS_HPP

#include <QString>
#include <QList>
#include <QKeySequence>
#include <QColor>
#include <QRect>
#include <QSize>
#include <QSettings>

class NBSettings {
	public:

		// General Settings
		class GeneralSettings {
			public:
				QString Style;									// Gui theme
				QString ViewMode;								// Icons, Tiles or Details
				QString IconTheme;								// Icon Theme
				bool ImagePreviews;								// Show image previews?
				bool NativeTitleBar;							// Use native title bar?
				bool TrayIcon;									// Minimize to tray?
				bool OpenWithCatalog;							// Show catalog on NB Open?
				bool SidePanel;									// Show sidepanel?
				int SidePanelType;								// Show classic or modern?
				bool ShowHidden;								// Show Hidden files and folders?
				QSize IconSize;									// Default Icon Size
				int SortColumn;									// Default Sort Column
				bool SortCase;									// Is sorting case sensitive
				bool Grouping;									// Enable grouping?
				bool PerFolderViews;							// Enable per folder views?
				bool FilterFolders;								// Filter folders while searching?
				bool CombiHome;									// Use CombiView as Home
		} General;

		// Special Settings
		class SpecialSettings {
			public:
				bool ClosingDown;
		} Special;

		// Session Settings
		class SessionSettings {
			public:
				QRect Geometry;
				QString LastDir;
				bool Maximized;
		} Session;

		// Shortcut Settings
		class ShortcutSettings {
			public :
				QList<QKeySequence> AboutNB;
				QList<QKeySequence> AboutQt;
				QList<QKeySequence> ToggleCrumbLE;
				QList<QKeySequence> ViewMode;
				QList<QKeySequence> AddCustomAction;
				QList<QKeySequence> GoHome;
				QList<QKeySequence> GoUp;
				QList<QKeySequence> GoLeft;
				QList<QKeySequence> GoRight;
				QList<QKeySequence> NewFolder;
				QList<QKeySequence> NewFile;
				QList<QKeySequence> Peek;
				QList<QKeySequence> Reload;
				QList<QKeySequence> ToggleHidden;
				QList<QKeySequence> SelectAll;
				QList<QKeySequence> ToggleSideBar;
				QList<QKeySequence> Cut;
				QList<QKeySequence> Copy;
				QList<QKeySequence> Paste;
				QList<QKeySequence> Rename;
				QList<QKeySequence> Delete;
				QList<QKeySequence> Trash;
				QList<QKeySequence> Properties;
				QList<QKeySequence> Permissions;
				QList<QKeySequence> Terminal;
				QList<QKeySequence> InlineTerminal;
				QList<QKeySequence> NBInfo;
				QList<QKeySequence> Settings;
				QList<QKeySequence> CustomActions;
				QList<QKeySequence> FocusAddressBar;
				QList<QKeySequence> NewWindow;
				QList<QKeySequence> AddBookmark;
				QList<QKeySequence> FocusSearchBar;
				QList<QKeySequence> ClearSearchBar;
				QList<QKeySequence> QuitNewBreeze;
		} Shortcuts;

		static NBSettings* defaultInstance();
		static NBSettings* instance();

		void reload();
		void setValue( QString, QVariant );

	private:
		NBSettings() {};

		bool init;

		static NBSettings *settings;
		static NBSettings *defaultSettings;
};

#endif
