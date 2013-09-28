/*
	*
	* NBSettings.cpp - NewBreeze Settings
	*
*/

#include <NBSettings.hpp>

QSettings gSettings( "NewBreeze", "NewBreeze-unstable" );
QSettings sSettings( "NewBreeze", "Shortcuts-unstable" );

inline QList<QKeySequence> getShortcuts( QString actionName ) {

	QStringList keys = sSettings.value( actionName ).toStringList();
	QList<QKeySequence> shortcuts;

	foreach( QString key, keys )
		shortcuts << QKeySequence( key );

	return shortcuts;
};

void NBSettings::readSettings() {

	General.Style  = gSettings.value( "Style" ).toString();
	General.FolderView  = gSettings.value( "FolderView" ).toString();
	General.IconTheme  = gSettings.value( "IconTheme" ).toString();
	General.ImagePreviews  = gSettings.value( "ImagePreviews" ).toBool();
	General.NativeTitleBar = gSettings.value( "NativeTitleBar" ).toBool();

	Session.Geometry  = gSettings.value( "Session/Geometry" ).toRect();
	Session.LastDir  = gSettings.value( "Session/LastDir" ).toString();
	Session.Maximized  = gSettings.value( "Session/Maximized" ).toBool();
	Session.SidePanel  = gSettings.value( "Session/SidePanel" ).toBool();
	Session.ShowHidden  = gSettings.value( "Session/ShowHidden" ).toBool();

	Shortcuts.AboutNB = getShortcuts( "AboutNB" );
	Shortcuts.AboutQt = getShortcuts( "AboutQt" );
	Shortcuts.ToggleCrumbLE = getShortcuts( "ToggleCrumbLE" );
	Shortcuts.ViewMode = getShortcuts( "ViewMode" );
	Shortcuts.AddCustomAction = getShortcuts( "AddCustomAction" );
	Shortcuts.GoHome = getShortcuts( "GoHome" );
	Shortcuts.GoUp = getShortcuts( "GoUp" );
	Shortcuts.GoLeft = getShortcuts( "GoLeft" );
	Shortcuts.GoRight = getShortcuts( "GoRight" );
	Shortcuts.NewFolder = getShortcuts( "NewFolder" );
	Shortcuts.NewFile = getShortcuts( "NewFile" );
	Shortcuts.Peek = getShortcuts( "Peek" );
	Shortcuts.Reload = getShortcuts( "Reload" );
	Shortcuts.ToggleHidden = getShortcuts( "ToggleHidden" );
	Shortcuts.SelectAll = getShortcuts( "SelectAll" );
	Shortcuts.ToggleSideBar = getShortcuts( "ToggleSideBar" );
	Shortcuts.Cut = getShortcuts( "Cut" );
	Shortcuts.Copy = getShortcuts( "Copy" );
	Shortcuts.Paste = getShortcuts( "Paste" );
	Shortcuts.Rename = getShortcuts( "Rename" );
	Shortcuts.Delete = getShortcuts( "Delete" );
	Shortcuts.Trash = getShortcuts( "Trash" );
	Shortcuts.Properties = getShortcuts( "Properties" );
	Shortcuts.Terminal = getShortcuts( "Terminal" );
	Shortcuts.InlineTerminal = getShortcuts( "InlineTerminal" );
	Shortcuts.NBInfo = getShortcuts( "NBInfo" );
	Shortcuts.Settings = getShortcuts( "Settings" );
	Shortcuts.CustomActions = getShortcuts( "CustomActions" );
	Shortcuts.FocusAddressBar = getShortcuts( "FocusAddressBar" );
	Shortcuts.NewWindow = getShortcuts( "NewWindow" );
	Shortcuts.AddBookmark = getShortcuts( "AddBookmark" );
	Shortcuts.FocusSearchBar = getShortcuts( "FocusSearchBar" );
	Shortcuts.ClearSearchBar = getShortcuts( "ClearSearchBar" );

	Colors.ExecPenColor  = gSettings.value( "ExecPenColor" ).value<QColor>();
	Colors.HiddenPenColor  = gSettings.value( "HiddenPenColor" ).value<QColor>();
	Colors.HiddenPenColorAlt  = gSettings.value( "HiddenPenColorAlt" ).value<QColor>();
	Colors.NoReadPenColor  = gSettings.value( "NoReadPenColor" ).value<QColor>();
	Colors.TextPenColor  = gSettings.value( "TextPenColor" ).value<QColor>();
	Colors.TextPenColorAlt  = gSettings.value( "TextPenColorAlt" ).value<QColor>();
	Colors.SymLinkPenColor  = gSettings.value( "SymLinkPenColor" ).value<QColor>();
	Colors.SymLinkPenColorAlt  = gSettings.value( "SymLinkPenColorAlt" ).value<QColor>();
	Colors.FocusPenColor  = gSettings.value( "FocusPenColor" ).value<QColor>();
	Colors.FocusPenColorAlt  = gSettings.value( "FocusPenColorAlt" ).value<QColor>();
	Colors.SelectionBrushColor  = gSettings.value( "SelectionBrushColor" ).value<QColor>();
	Colors.MouseBrushColor  = gSettings.value( "MouseBrushColor" ).value<QColor>();
	Colors.SelectionMouseBrushColor  = gSettings.value( "SelectionMouseBrushColor" ).value<QColor>();
};
