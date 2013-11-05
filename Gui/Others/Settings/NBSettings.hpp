/*
    *
    * NBSettings.hpp - NBSettings class header
    *
*/

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
				QString Style;
				QString FolderView;
				QString IconTheme;
				bool ImagePreviews;
				bool NativeTitleBar;
				int MaxIOJobs;
		} General;

		// Session Settings
		class SessionSettings {
			public:
				QRect Geometry;
				QString LastDir;
				bool Maximized;
				QSize Splitter;
				bool SidePanel;
				bool ShowHidden;
				bool ExpandDevices;
				bool ExpandBookmarks;
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
		} Shortcuts;

		// Color Settings
		class ColorSettings {
			public:
				QColor ExecPenColor;
				QColor HiddenPenColor;
				QColor HiddenPenColorAlt;
				QColor NoReadPenColor;
				QColor TextPenColor;
				QColor TextPenColorAlt;
				QColor SymLinkPenColor;
				QColor SymLinkPenColorAlt;
				QColor FocusPenColor;
				QColor FocusPenColorAlt;
				QColor SelectionBrushColor;
				QColor MouseBrushColor;
				QColor SelectionMouseBrushColor;
		} Colors;

		static NBSettings* defaultInstance();
		static NBSettings* instance();

		void reload();
		void setValue( QString, QVariant );

	private:
		NBSettings() {};

		bool init = false;

		static NBSettings *settings;
		static NBSettings *defaultSettings;
};

#endif
