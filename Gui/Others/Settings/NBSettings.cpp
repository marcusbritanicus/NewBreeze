/*
	*
	* NBSettings.cpp - NewBreeze Settings
	*
*/

#include <NBSettings.hpp>
#include <NBSystemInfo.hpp>

inline QList<QKeySequence> getShortcuts( QString actionName ) {

	QSettings sSettings( "NewBreeze", "Shortcuts" );

	QStringList keys = sSettings.value( actionName ).toStringList();
	QList<QKeySequence> shortcuts;

	foreach( QString key, keys )
		shortcuts << QKeySequence( key );

	return shortcuts;
};

NBSettings *NBSettings::settings = NULL;
NBSettings *NBSettings::defaultSettings = NULL;

NBSettings* NBSettings::defaultInstance() {

	defaultSettings = new NBSettings();
	defaultSettings->init = true;

	defaultSettings->General.Style = QString( "LightGray" );
	defaultSettings->General.FolderView = QString( "IconsView" );
	defaultSettings->General.IconTheme = NBSystemIconTheme();
	defaultSettings->General.ImagePreviews = true;
	defaultSettings->General.NativeTitleBar = false;
	defaultSettings->General.MaxIOJobs = 5;

	defaultSettings->Special.ClosingDown = false;

	defaultSettings->Session.Geometry = QRect( 900, 600, 0, 0 );
	defaultSettings->Session.LastDir = QDir::homePath();
	defaultSettings->Session.Maximized = true;
	defaultSettings->Session.SidePanel = true;
	defaultSettings->Session.ShowHidden = false;
	defaultSettings->Session.IconSize = QSize( 48, 48 );
	defaultSettings->Session.SortColumn = 0;
	defaultSettings->Session.SortCase = false;
	defaultSettings->Session.SortCategory = false;
	defaultSettings->Session.ExpandDevices = true;
	defaultSettings->Session.ExpandBookmarks = true;

	defaultSettings->Shortcuts.AboutNB = QList<QKeySequence>() << QKeySequence( "Shift+F1" );
	defaultSettings->Shortcuts.AboutQt = QList<QKeySequence>() << QKeySequence( "Shift+F2" );
	defaultSettings->Shortcuts.ToggleCrumbLE = QList<QKeySequence>() << QKeySequence( "Ctrl+T" );
	defaultSettings->Shortcuts.ViewMode = QList<QKeySequence>() << QKeySequence( "Ctrl+Shift+V" );
	defaultSettings->Shortcuts.AddCustomAction = QList<QKeySequence>() << QKeySequence( "Ctrl+D" );
	defaultSettings->Shortcuts.GoHome = QList<QKeySequence>() << QKeySequence( "Alt+Home" );
	defaultSettings->Shortcuts.GoUp = QList<QKeySequence>() << QKeySequence( "Alt+Up" );
	defaultSettings->Shortcuts.GoLeft = QList<QKeySequence>() << QKeySequence( "Alt+Left" );
	defaultSettings->Shortcuts.GoRight = QList<QKeySequence>() << QKeySequence( "Alt+Right" ) << QKeySequence( "Alt+Down" );
	defaultSettings->Shortcuts.NewFolder = QList<QKeySequence>() << QKeySequence( "F10" );
	defaultSettings->Shortcuts.NewFile = QList<QKeySequence>() << QKeySequence( "Ctrl+Shift+N" );
	defaultSettings->Shortcuts.Peek = QList<QKeySequence>() << QKeySequence( "Ctrl+Return" );
	defaultSettings->Shortcuts.Reload = QList<QKeySequence>() << QKeySequence( "F5" ) << QKeySequence( "Ctrl+R" );
	defaultSettings->Shortcuts.ToggleHidden = QList<QKeySequence>() << QKeySequence( "Alt+." ) << QKeySequence( "Ctrl+H" );
	defaultSettings->Shortcuts.SelectAll = QList<QKeySequence>() << QKeySequence( "Ctrl+A" );
	defaultSettings->Shortcuts.ToggleSideBar = QList<QKeySequence>() << QKeySequence( "F9" );
	defaultSettings->Shortcuts.Cut = QList<QKeySequence>() << QKeySequence( "Ctrl+X" );
	defaultSettings->Shortcuts.Copy = QList<QKeySequence>() << QKeySequence( "Ctrl+C" );
	defaultSettings->Shortcuts.Paste = QList<QKeySequence>() << QKeySequence( "Ctrl+V" );
	defaultSettings->Shortcuts.Rename = QList<QKeySequence>() << QKeySequence( "F2" );
	defaultSettings->Shortcuts.Delete = QList<QKeySequence>() << QKeySequence( "Shift+Delete" );
	defaultSettings->Shortcuts.Trash = QList<QKeySequence>() << QKeySequence( "Delete" );
	defaultSettings->Shortcuts.Properties = QList<QKeySequence>() << QKeySequence( "Alt+Return" );
	defaultSettings->Shortcuts.Permissions = QList<QKeySequence>() << QKeySequence( "Alt+Shift+Return" );
	defaultSettings->Shortcuts.Terminal = QList<QKeySequence>() << QKeySequence( "F4" );
	defaultSettings->Shortcuts.InlineTerminal = QList<QKeySequence>() << QKeySequence( "Shift+F4" );
	defaultSettings->Shortcuts.NBInfo = QList<QKeySequence>() << QKeySequence( "Ctrl+I" );
	defaultSettings->Shortcuts.Settings = QList<QKeySequence>() << QKeySequence( "Ctrl+Shift+S" );
	defaultSettings->Shortcuts.CustomActions = QList<QKeySequence>() << QKeySequence( "Ctrl+Shift+D" );
	defaultSettings->Shortcuts.FocusAddressBar = QList<QKeySequence>() << QKeySequence( "Ctrl+L" ) << QKeySequence( "F6" );
	defaultSettings->Shortcuts.NewWindow = QList<QKeySequence>() << QKeySequence( "Ctrl+N" );
	defaultSettings->Shortcuts.AddBookmark = QList<QKeySequence>() << QKeySequence( "Ctrl+D" );
	defaultSettings->Shortcuts.FocusSearchBar = QList<QKeySequence>() << QKeySequence( "Ctrl+F" );
	defaultSettings->Shortcuts.ClearSearchBar = QList<QKeySequence>() << QKeySequence( "Ctrl+Shift+F" );

	defaultSettings->Colors.ExecPenColor = QColor( 0, 100, 0, 255 );
	defaultSettings->Colors.HiddenPenColor = QColor( 110, 110, 110, 255 );
	defaultSettings->Colors.HiddenPenColorAlt = QColor( 61, 61, 61, 255 );
	defaultSettings->Colors.NoReadPenColor = QColor( 162, 42, 42, 255 );
	defaultSettings->Colors.TextPenColor = QColor( 255, 255, 255, 255 );
	defaultSettings->Colors.TextPenColorAlt = QColor( 0, 0, 0, 255 );
	defaultSettings->Colors.SymLinkPenColor = QColor( 175, 238, 238, 255 );
	defaultSettings->Colors.SymLinkPenColorAlt = QColor( 124, 104, 238, 255 );
	defaultSettings->Colors.FocusPenColor = QColor( 255, 255, 255, 255 );
	defaultSettings->Colors.FocusPenColorAlt = QColor( 0, 0, 0, 255 );
	defaultSettings->Colors.SelectionBrushColor = QColor( 160, 160, 160, 90 );
	defaultSettings->Colors.MouseBrushColor = QColor( 160, 160, 160, 60 );
	defaultSettings->Colors.SelectionMouseBrushColor = QColor( 160, 160, 160, 120 );

	return defaultSettings;
};

NBSettings* NBSettings::instance() {

	if ( settings )
		return settings;

	QSettings gSettings( "NewBreeze", "NewBreeze" );
	QSettings sSettings( "NewBreeze", "Shortcuts" );

	settings = defaultInstance();

	QStringList gaKeys = gSettings.allKeys();
	QStringList saKeys = sSettings.allKeys();

	if ( gaKeys.contains( QString( "Style" ) ) )
		settings->General.Style = gSettings.value( "Style" ).toString();

	if ( gaKeys.contains( QString( "FolderView" ) ) )
		settings->General.FolderView = gSettings.value( "FolderView" ).toString();

	if ( gaKeys.contains( QString( "IconTheme" ) ) )
		settings->General.IconTheme = gSettings.value( "IconTheme" ).toString();

	if ( gaKeys.contains( QString( "ImagePreviews" ) ) )
		settings->General.ImagePreviews = gSettings.value( "ImagePreviews" ).toBool();

	if ( gaKeys.contains( QString( "NativeTitleBar" ) ) )
		settings->General.NativeTitleBar = gSettings.value( "NativeTitleBar" ).toBool();

	if ( gaKeys.contains( QString( "MaxIOJobs" ) ) )
		settings->General.MaxIOJobs = gSettings.value( "MaxIOJobs" ).toInt();

	if ( gaKeys.contains( QString( "Session/Geometry" ) ) )
		settings->Session.Geometry = gSettings.value( "Session/Geometry" ).toRect();

	if ( gaKeys.contains( QString( "Session/LastDir" ) ) )
		settings->Session.LastDir = gSettings.value( "Session/LastDir" ).toString();

	if ( gaKeys.contains( QString( "Session/Maximized" ) ) )
		settings->Session.Maximized = gSettings.value( "Session/Maximized" ).toBool();

	if ( gaKeys.contains( QString( "Session/SidePanel" ) ) )
		settings->Session.SidePanel = gSettings.value( "Session/SidePanel" ).toBool();

	if ( gaKeys.contains( QString( "Session/ShowHidden" ) ) )
		settings->Session.ShowHidden = gSettings.value( "Session/ShowHidden" ).toBool();

	if ( gaKeys.contains( QString( "Session/IconSize" ) ) )
		settings->Session.IconSize = gSettings.value( "Session/IconSize" ).toSize();

	if ( gaKeys.contains( QString( "Session/SortColumn" ) ) )
		settings->Session.SortColumn = gSettings.value( "Session/SortColumn" ).toInt();

	if ( gaKeys.contains( QString( "Session/SortCase" ) ) )
		settings->Session.SortCase = gSettings.value( "Session/SortCase" ).toBool();

	if ( gaKeys.contains( QString( "Session/SortCategory" ) ) )
		settings->Session.SortCategory = gSettings.value( "Session/SortCategory" ).toBool();

	if ( gaKeys.contains( QString( "Session/ExpandDevices" ) ) )
		settings->Session.ExpandDevices = gSettings.value( "Session/ExpandDevices" ).toBool();

	if ( gaKeys.contains( QString( "Session/ExpandBookmarks" ) ) )
		settings->Session.ExpandBookmarks = gSettings.value( "Session/ExpandBookmarks" ).toBool();

	if ( saKeys.contains( QString( "AboutNB" ) ) )
		settings->Shortcuts.AboutNB = getShortcuts( "AboutNB" );

	if ( saKeys.contains( QString( "AboutQt" ) ) )
		settings->Shortcuts.AboutQt = getShortcuts( "AboutQt" );

	if ( saKeys.contains( QString( "ToggleCrumbLE" ) ) )
		settings->Shortcuts.ToggleCrumbLE = getShortcuts( "ToggleCrumbLE" );

	if ( saKeys.contains( QString( "ViewMode" ) ) )
		settings->Shortcuts.ViewMode = getShortcuts( "ViewMode" );

	if ( saKeys.contains( QString( "AddCustomAction" ) ) )
		settings->Shortcuts.AddCustomAction = getShortcuts( "AddCustomAction" );

	if ( saKeys.contains( QString( "GoHome" ) ) )
		settings->Shortcuts.GoHome = getShortcuts( "GoHome" );

	if ( saKeys.contains( QString( "GoUp" ) ) )
		settings->Shortcuts.GoUp = getShortcuts( "GoUp" );

	if ( saKeys.contains( QString( "GoLeft" ) ) )
		settings->Shortcuts.GoLeft = getShortcuts( "GoLeft" );

	if ( saKeys.contains( QString( "GoRight" ) ) )
		settings->Shortcuts.GoRight = getShortcuts( "GoRight" );

	if ( saKeys.contains( QString( "NewFolder" ) ) )
		settings->Shortcuts.NewFolder = getShortcuts( "NewFolder" );

	if ( saKeys.contains( QString( "Style" ) ) )
		settings->Shortcuts.NewFile = getShortcuts( "NewFile" );

	if ( saKeys.contains( QString( "Peek" ) ) )
		settings->Shortcuts.Peek = getShortcuts( "Peek" );

	if ( saKeys.contains( QString( "Reload" ) ) )
		settings->Shortcuts.Reload = getShortcuts( "Reload" );

	if ( saKeys.contains( QString( "ToggleHidden" ) ) )
		settings->Shortcuts.ToggleHidden = getShortcuts( "ToggleHidden" );

	if ( saKeys.contains( QString( "SelectAll" ) ) )
		settings->Shortcuts.SelectAll = getShortcuts( "SelectAll" );

	if ( saKeys.contains( QString( "ToggleSideBar" ) ) )
		settings->Shortcuts.ToggleSideBar = getShortcuts( "ToggleSideBar" );

	if ( saKeys.contains( QString( "Cut" ) ) )
		settings->Shortcuts.Cut = getShortcuts( "Cut" );

	if ( saKeys.contains( QString( "Copy" ) ) )
		settings->Shortcuts.Copy = getShortcuts( "Copy" );

	if ( saKeys.contains( QString( "Paste" ) ) )
		settings->Shortcuts.Paste = getShortcuts( "Paste" );

	if ( saKeys.contains( QString( "Rename" ) ) )
		settings->Shortcuts.Rename = getShortcuts( "Rename" );

	if ( saKeys.contains( QString( "Delete" ) ) )
		settings->Shortcuts.Delete = getShortcuts( "Delete" );

	if ( saKeys.contains( QString( "Trash" ) ) )
		settings->Shortcuts.Trash = getShortcuts( "Trash" );

	if ( saKeys.contains( QString( "Properties" ) ) )
		settings->Shortcuts.Properties = getShortcuts( "Properties" );

	if ( saKeys.contains( QString( "Permissions" ) ) )
		settings->Shortcuts.Permissions = getShortcuts( "Permissions" );

	if ( saKeys.contains( QString( "Terminal" ) ) )
		settings->Shortcuts.Terminal = getShortcuts( "Terminal" );

	if ( saKeys.contains( QString( "InlineTerminal" ) ) )
		settings->Shortcuts.InlineTerminal = getShortcuts( "InlineTerminal" );

	if ( saKeys.contains( QString( "NBInfo" ) ) )
		settings->Shortcuts.NBInfo = getShortcuts( "NBInfo" );

	if ( saKeys.contains( QString( "Settings" ) ) )
		settings->Shortcuts.Settings = getShortcuts( "Settings" );

	if ( saKeys.contains( QString( "CustomActions" ) ) )
		settings->Shortcuts.CustomActions = getShortcuts( "CustomActions" );

	if ( saKeys.contains( QString( "FocusAddressBar" ) ) )
		settings->Shortcuts.FocusAddressBar = getShortcuts( "FocusAddressBar" );

	if ( saKeys.contains( QString( "NewWindow" ) ) )
		settings->Shortcuts.NewWindow = getShortcuts( "NewWindow" );

	if ( saKeys.contains( QString( "AddBookmark" ) ) )
		settings->Shortcuts.AddBookmark = getShortcuts( "AddBookmark" );

	if ( saKeys.contains( QString( "FocusSearchBar" ) ) )
		settings->Shortcuts.FocusSearchBar = getShortcuts( "FocusSearchBar" );

	if ( saKeys.contains( QString( "ClearSearchBar" ) ) )
		settings->Shortcuts.ClearSearchBar = getShortcuts( "ClearSearchBar" );

	return settings;
};

void NBSettings::reload() {

	if ( not defaultSettings->init )
		defaultInstance();

	if ( not settings->init )
		instance();

	QSettings gSettings( "NewBreeze", "NewBreeze" );
	QSettings sSettings( "NewBreeze", "Shortcuts" );

	QStringList gaKeys = gSettings.allKeys();
	QStringList saKeys = sSettings.allKeys();

	if ( gaKeys.contains( QString( "Style" ) ) )
		General.Style = gSettings.value( "Style" ).toString();

	if ( gaKeys.contains( QString( "FolderView" ) ) )
		General.FolderView = gSettings.value( "FolderView" ).toString();

	if ( gaKeys.contains( QString( "IconTheme" ) ) )
		General.IconTheme = gSettings.value( "IconTheme" ).toString();

	if ( gaKeys.contains( QString( "ImagePreviews" ) ) )
		General.ImagePreviews = gSettings.value( "ImagePreviews" ).toBool();

	if ( gaKeys.contains( QString( "NativeTitleBar" ) ) )
		General.NativeTitleBar = gSettings.value( "NativeTitleBar" ).toBool();

	if ( gaKeys.contains( QString( "MaxIOJobs" ) ) )
		General.MaxIOJobs = gSettings.value( "MaxIOJobs" ).toInt();

	if ( gaKeys.contains( QString( "Session/Geometry" ) ) )
		Session.Geometry = gSettings.value( "Session/Geometry" ).toRect();

	if ( gaKeys.contains( QString( "Session/LastDir" ) ) )
		Session.LastDir = gSettings.value( "Session/LastDir" ).toString();

	if ( gaKeys.contains( QString( "Session/Maximized" ) ) )
		Session.Maximized = gSettings.value( "Session/Maximized" ).toBool();

	if ( gaKeys.contains( QString( "Session/SidePanel" ) ) )
		Session.SidePanel = gSettings.value( "Session/SidePanel" ).toBool();

	if ( gaKeys.contains( QString( "Session/ShowHidden" ) ) )
		Session.ShowHidden = gSettings.value( "Session/ShowHidden" ).toBool();

	if ( gaKeys.contains( QString( "Session/IconSize" ) ) )
		Session.IconSize = gSettings.value( "Session/IconSize" ).toSize();

	if ( gaKeys.contains( QString( "Session/SortColumn" ) ) )
		Session.SortColumn = gSettings.value( "Session/SortColumn" ).toInt();

	if ( gaKeys.contains( QString( "Session/SortCase" ) ) )
		Session.SortCase = gSettings.value( "Session/SortCase" ).toBool();

	if ( gaKeys.contains( QString( "Session/SortCategory" ) ) )
		Session.SortCategory = gSettings.value( "Session/SortCategory" ).toBool();

	if ( gaKeys.contains( QString( "Session/ExpandDevices" ) ) )
		Session.ExpandDevices = gSettings.value( "Session/ExpandDevices" ).toBool();

	if ( gaKeys.contains( QString( "Session/ExpandBookmarks" ) ) )
		Session.ExpandBookmarks = gSettings.value( "Session/ExpandBookmarks" ).toBool();

	if ( saKeys.contains( QString( "AboutNB" ) ) )
		Shortcuts.AboutNB = getShortcuts( "AboutNB" );

	if ( saKeys.contains( QString( "AboutQt" ) ) )
		Shortcuts.AboutQt = getShortcuts( "AboutQt" );

	if ( saKeys.contains( QString( "ToggleCrumbLE" ) ) )
		Shortcuts.ToggleCrumbLE = getShortcuts( "ToggleCrumbLE" );

	if ( saKeys.contains( QString( "ViewMode" ) ) )
		Shortcuts.ViewMode = getShortcuts( "ViewMode" );

	if ( saKeys.contains( QString( "AddCustomAction" ) ) )
		Shortcuts.AddCustomAction = getShortcuts( "AddCustomAction" );

	if ( saKeys.contains( QString( "GoHome" ) ) )
		Shortcuts.GoHome = getShortcuts( "GoHome" );

	if ( saKeys.contains( QString( "GoUp" ) ) )
		Shortcuts.GoUp = getShortcuts( "GoUp" );

	if ( saKeys.contains( QString( "GoLeft" ) ) )
		Shortcuts.GoLeft = getShortcuts( "GoLeft" );

	if ( saKeys.contains( QString( "GoRight" ) ) )
		Shortcuts.GoRight = getShortcuts( "GoRight" );

	if ( saKeys.contains( QString( "NewFolder" ) ) )
		Shortcuts.NewFolder = getShortcuts( "NewFolder" );

	if ( saKeys.contains( QString( "Style" ) ) )
		Shortcuts.NewFile = getShortcuts( "NewFile" );

	if ( saKeys.contains( QString( "Peek" ) ) )
		Shortcuts.Peek = getShortcuts( "Peek" );

	if ( saKeys.contains( QString( "Reload" ) ) )
		Shortcuts.Reload = getShortcuts( "Reload" );

	if ( saKeys.contains( QString( "ToggleHidden" ) ) )
		Shortcuts.ToggleHidden = getShortcuts( "ToggleHidden" );

	if ( saKeys.contains( QString( "SelectAll" ) ) )
		Shortcuts.SelectAll = getShortcuts( "SelectAll" );

	if ( saKeys.contains( QString( "ToggleSideBar" ) ) )
		Shortcuts.ToggleSideBar = getShortcuts( "ToggleSideBar" );

	if ( saKeys.contains( QString( "Cut" ) ) )
		Shortcuts.Cut = getShortcuts( "Cut" );

	if ( saKeys.contains( QString( "Copy" ) ) )
		Shortcuts.Copy = getShortcuts( "Copy" );

	if ( saKeys.contains( QString( "Paste" ) ) )
		Shortcuts.Paste = getShortcuts( "Paste" );

	if ( saKeys.contains( QString( "Rename" ) ) )
		Shortcuts.Rename = getShortcuts( "Rename" );

	if ( saKeys.contains( QString( "Delete" ) ) )
		Shortcuts.Delete = getShortcuts( "Delete" );

	if ( saKeys.contains( QString( "Trash" ) ) )
		Shortcuts.Trash = getShortcuts( "Trash" );

	if ( saKeys.contains( QString( "Properties" ) ) )
		Shortcuts.Properties = getShortcuts( "Properties" );

	if ( saKeys.contains( QString( "Permissions" ) ) )
		Shortcuts.Permissions = getShortcuts( "Permissions" );

	if ( saKeys.contains( QString( "Terminal" ) ) )
		Shortcuts.Terminal = getShortcuts( "Terminal" );

	if ( saKeys.contains( QString( "InlineTerminal" ) ) )
		Shortcuts.InlineTerminal = getShortcuts( "InlineTerminal" );

	if ( saKeys.contains( QString( "NBInfo" ) ) )
		Shortcuts.NBInfo = getShortcuts( "NBInfo" );

	if ( saKeys.contains( QString( "Settings" ) ) )
		Shortcuts.Settings = getShortcuts( "Settings" );

	if ( saKeys.contains( QString( "CustomActions" ) ) )
		Shortcuts.CustomActions = getShortcuts( "CustomActions" );

	if ( saKeys.contains( QString( "FocusAddressBar" ) ) )
		Shortcuts.FocusAddressBar = getShortcuts( "FocusAddressBar" );

	if ( saKeys.contains( QString( "NewWindow" ) ) )
		Shortcuts.NewWindow = getShortcuts( "NewWindow" );

	if ( saKeys.contains( QString( "AddBookmark" ) ) )
		Shortcuts.AddBookmark = getShortcuts( "AddBookmark" );

	if ( saKeys.contains( QString( "FocusSearchBar" ) ) )
		Shortcuts.FocusSearchBar = getShortcuts( "FocusSearchBar" );

	if ( saKeys.contains( QString( "ClearSearchBar" ) ) )
		Shortcuts.ClearSearchBar = getShortcuts( "ClearSearchBar" );
};

void NBSettings::setValue( QString key, QVariant value ) {

	QSettings sett( "NewBreeze", "NewBreeze" );
	sett.setValue( key, value );
	sett.sync();

	reload();
};
