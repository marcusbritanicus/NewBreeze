/*
	*
	* NBStartup.cpp - NewBreeze Startup Configurator
	*
*/

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBConfigParser.hpp>
#include <NBSystemInfo.hpp>

NBSettings Settings;
void NBStartup() {
	/*
		*
		* Check if each of the fields have a value. If not set them.
		*
	*/

	if ( !QFileInfo( thumbsDir ).exists() ) {
		bool out = QDir::home().mkpath( thumbsDir );
		if ( !out ) {
			qDebug() << "Error settings thumbnail cache path: " << thumbsDir;
			abort();
		}
	}

	/*
		*
		* Icon Theme
		*
	*/
	QIcon::setThemeName( NBSystemIconTheme() );

	/*
		*
		* Some general and basic settings
		*
	*/
	if ( settings.value( QObject::tr( "Style" ) ).isNull() )
		settings.setValue( QObject::tr( "Style" ), QObject::tr( "LightGray" ) );

	if ( settings.value( QObject::tr( "ImagePreviews" ) ).isNull() )
		settings.setValue( QObject::tr( "ImagePreviews" ), true );

	if ( settings.value( QObject::tr( "FolderView" ) ).isNull() )
		settings.setValue( QObject::tr( "FolderView" ), QObject::tr( "NormalIconsView" ) );

	if ( settings.value( QObject::tr( "FolderView" ) ).isNull() )
		settings.setValue( QObject::tr( "FolderView" ), QObject::tr( "NormalIconsView" ) );

	if ( settings.value( QObject::tr( "IconTheme" ) ).isNull() )
		settings.setValue( QObject::tr( "IconTheme" ), QIcon::themeName() );

	if ( settings.value( QObject::tr( "NativeTitleBar" ) ).isNull() )
		settings.setValue( QObject::tr( "NativeTitleBar" ), false );

	if ( settings.value( QObject::tr( "MaxIOJobs" ) ).isNull() )
		settings.setValue( QObject::tr( "MaxIOJobs" ), 5 );

	if ( settings.value( QObject::tr( "SymLinkPenColorAlt" ) ).isNull() )
		settings.setValue( QObject::tr( "SymLinkPenColorAlt" ), QColor( 124, 104, 238, 255 ) );

	if ( settings.value( QObject::tr( "SymLinkPenColor" ) ).isNull() )
		settings.setValue( QObject::tr( "SymLinkPenColor" ), QColor( 175, 238, 238, 255 ) );

	if ( settings.value( QObject::tr( "ExecPenColor" ) ).isNull() )
		settings.setValue( QObject::tr( "ExecPenColor" ), QColor( 0, 100, 0, 255 ) );

	if ( settings.value( QObject::tr( "HiddenPenColorAlt" ) ).isNull() )
		settings.setValue( QObject::tr( "HiddenPenColorAlt" ), QColor( 61, 61, 61, 255 ) );

	if ( settings.value( QObject::tr( "HiddenPenColor" ) ).isNull() )
		settings.setValue( QObject::tr( "HiddenPenColor" ), QColor( 110, 110, 110, 255 ) );

	if ( settings.value( QObject::tr( "NoReadPenColor" ) ).isNull() )
		settings.setValue( QObject::tr( "NoReadPenColor" ), QColor( 162, 42, 42, 255 ) );

	if ( settings.value( QObject::tr( "TextPenColorAlt" ) ).isNull() )
		settings.setValue( QObject::tr( "TextPenColorAlt" ), QColor( 0, 0, 0, 255 ) );

	if ( settings.value( QObject::tr( "TextPenColor" ) ).isNull() )
		settings.setValue( QObject::tr( "TextPenColor" ), QColor( 255, 255, 255, 255 ) );

	if ( settings.value( QObject::tr( "FocusPenColor" ) ).isNull() )
		settings.setValue( QObject::tr( "FocusPenColor" ), QColor( 255, 255, 255, 255 ) );

	if ( settings.value( QObject::tr( "FocusPenColorAlt" ) ).isNull() )
		settings.setValue( QObject::tr( "FocusPenColorAlt" ), QColor( 0, 0, 0, 255 ) );

	if ( settings.value( QObject::tr( "SelectionBrushColor" ) ).isNull() )
		settings.setValue( QObject::tr( "SelectionBrushColor" ), QColor( 160, 160, 160, 90 ) );

	if ( settings.value( QObject::tr( "MouseBrushColor" ) ).isNull() )
		settings.setValue( QObject::tr( "MouseBrushColor" ), QColor( 160, 160, 160, 60 ) );

	if ( settings.value( QObject::tr(   "SelectionMouseBrushColor" ) ).isNull() )
		settings.setValue( QObject::tr( "SelectionMouseBrushColor" ), QColor( 160, 160, 160, 120 ) );

	/*
		*
		* Shortcut settings
		*
	*/

	if ( not shortcutSettings.allKeys().contains( "AboutNB" ) )
		shortcutSettings.setValue( QObject::tr( "AboutNB" ), "Shift+F1" );

	if ( not shortcutSettings.allKeys().contains( "ToggleCrumbLE" ) )
		shortcutSettings.setValue( QObject::tr( "ToggleCrumbLE" ), "Ctrl+T" );

	if ( not shortcutSettings.allKeys().contains( "ViewMode" ) )
		shortcutSettings.setValue( QObject::tr( "ViewMode" ), "Ctrl+Shift+V" );

	if ( not shortcutSettings.allKeys().contains( "AddCustomAction" ) )
		shortcutSettings.setValue( QObject::tr( "AddCustomAction" ), "Ctrl+D" );

	if ( not shortcutSettings.allKeys().contains( "GoHome" ) )
		shortcutSettings.setValue( QObject::tr( "GoHome" ), "Alt+Home" );

	if ( not shortcutSettings.allKeys().contains( "GoUp" ) )
		shortcutSettings.setValue( QObject::tr( "GoUp" ), "Alt+Up" );

	if ( not shortcutSettings.allKeys().contains( "GoLeft" ) )
		shortcutSettings.setValue( QObject::tr( "GoLeft" ), "Alt+Left" );

	if ( not shortcutSettings.allKeys().contains( "GoRight" ) )
		shortcutSettings.setValue( QObject::tr( "GoRight" ), "Alt+Right" );

	if ( not shortcutSettings.allKeys().contains( "NewFolder" ) )
		shortcutSettings.setValue( QObject::tr( "NewFolder" ), "F10" );

	if ( not shortcutSettings.allKeys().contains( "NewFile" ) )
		shortcutSettings.setValue( QObject::tr( "NewFile" ), "Ctrl+Shift+N" );

	if ( not shortcutSettings.allKeys().contains( "Peek" ) )
		shortcutSettings.setValue( QObject::tr( "Peek" ), "Ctrl+Return" );

	if ( not shortcutSettings.allKeys().contains( "Reload" ) )
		shortcutSettings.setValue( QObject::tr( "Reload" ), QStringList() << "F5" << "Ctrl+R" );

	if ( not shortcutSettings.allKeys().contains( "ToggleHidden" ) )
		shortcutSettings.setValue( QObject::tr( "ToggleHidden" ), QStringList() << "Alt+." << "Ctrl+H" );

	if ( not shortcutSettings.allKeys().contains( "SelectAll" ) )
		shortcutSettings.setValue( QObject::tr( "SelectAll" ), "Ctrl+A" );

	if ( not shortcutSettings.allKeys().contains( "ToggleSideBar" ) )
		shortcutSettings.setValue( QObject::tr( "ToggleSideBar" ), "F9" );

	if ( not shortcutSettings.allKeys().contains( "Cut" ) )
		shortcutSettings.setValue( QObject::tr( "Cut" ), "Ctrl+X" );

	if ( not shortcutSettings.allKeys().contains( "Copy" ) )
		shortcutSettings.setValue( QObject::tr( "Copy" ), "Ctrl+C" );

	if ( not shortcutSettings.allKeys().contains( "Paste" ) )
		shortcutSettings.setValue( QObject::tr( "Paste" ), "Ctrl+V" );

	if ( not shortcutSettings.allKeys().contains( "Rename" ) )
		shortcutSettings.setValue( QObject::tr( "Rename" ), "F2" );

	if ( not shortcutSettings.allKeys().contains( "Delete" ) )
		shortcutSettings.setValue( QObject::tr( "Delete" ), "Shift+Delete" );

	if ( not shortcutSettings.allKeys().contains( "Trash" ) )
		shortcutSettings.setValue( QObject::tr( "Trash" ), "Delete" );

	if ( not shortcutSettings.allKeys().contains( "Properties" ) )
		shortcutSettings.setValue( QObject::tr( "Properties" ), "Alt+Return" );

	if ( not shortcutSettings.allKeys().contains( "Terminal" ) )
		shortcutSettings.setValue( QObject::tr( "Terminal" ), "F4" );

	if ( not shortcutSettings.allKeys().contains( "InlineTerminal" ) )
		shortcutSettings.setValue( QObject::tr( "InlineTerminal" ), "Shift+F4" );

	if ( not shortcutSettings.allKeys().contains( "NBInfo" ) )
		shortcutSettings.setValue( QObject::tr( "NBInfo" ), "Ctrl+I" );

	if ( not shortcutSettings.allKeys().contains( "Settings" ) )
		shortcutSettings.setValue( QObject::tr( "Settings" ), "Ctrl+S" );

	if ( not shortcutSettings.allKeys().contains( "CustomActions" ) )
		shortcutSettings.setValue( QObject::tr( "CustomActions" ), "Ctrl+Shift+D" );

	if ( not shortcutSettings.allKeys().contains( "FocusAddressBar" ) )
		shortcutSettings.setValue( QObject::tr( "FocusAddressBar" ), QStringList() << "Ctrl+L" << "F6" );

	if ( not shortcutSettings.allKeys().contains( "NewWindow" ) )
		shortcutSettings.setValue( QObject::tr( "NewWindow" ), "Ctrl+N" );

	if ( not shortcutSettings.allKeys().contains( "AddBookmark" ) )
		shortcutSettings.setValue( QObject::tr( "AddBookmark" ), "Ctrl+D" );

	/*
		*
		* Handle CustomActions
		*
	*/

	if ( actionSettings.value( "OpenAsSuperUser/Name" ).toString().isEmpty() ) {
		actionSettings.setValue( "OpenAsSuperUser/Name", "Open as SuperUser" );
		actionSettings.setValue( "OpenAsSuperUser/Exec", "sudo xdg-open %f" );
		actionSettings.setValue( "OpenAsSuperUser/Glob", QStringList() << "*" );
	}

	if ( actionSettings.value( "OpenInTerminal" ).toString().isEmpty() ) {
		actionSettings.setValue( "OpenInTerminal/Name", "Open In Terminal" );
		actionSettings.setValue( "OpenInTerminal/Exec", "%term" );
		actionSettings.setValue( "OpenInTerminal/Glob", QStringList() << "folder" );
	}

	/*
		*
		* Detect and parse various .desktop files for appications-mimetype cache
		*
	*/

	QStringList paths = QStringList() << "/usr/share/applications" << QDir::home().filePath( ".local/share/applications/" );
	paths << "/usr/share/applications/kde4" << "/usr/share/gnome/applications/" << "/usr/local/share/applications/";
	NBConfigParser::dirPaths << paths;
	QtConcurrent::run( NBConfigParser::createCache );

	/*
		*
		* Detect and save the terminals available
		*
	*/

	if ( settings.value( "Terminals/nbterminal" ).toString().isEmpty() ) {
		QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
		foreach( QString path, paths ) {
			if ( QFileInfo( path + "/nbterminal" ).exists() ) {
				QStringList termInfo = QStringList() << "nbterminal" << "--workdir" << "%1" << "-e" << "%2";
				settings.setValue( "Terminals/nbterminal", QVariant( termInfo ) );
				break;
			}
		}
	}

	if ( settings.value( "Terminals/konsole" ).toString().isEmpty() ) {
		QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
		foreach( QString path, paths ) {
			if ( QFileInfo( path + "/konsole" ).exists() ) {
				QStringList termInfo = QStringList() << "konsole" << "--workdir" << "%1" << "-e" << "%2";
				settings.setValue( "Terminals/konsole", QVariant( termInfo ) );
				break;
			}
		}
	}

	if ( settings.value( "Terminals/gnome-terminal" ).toString().isEmpty() ) {
		QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
		foreach( QString path, paths ) {
			if ( QFileInfo( path + "/gnome-terminal" ).exists() ) {
				QStringList termInfo = QStringList() << "gnome-terminal" << "--working-directory" << "%1" << "-x" << "%2";
				settings.setValue( "Terminals/gnome-terminal", QVariant( termInfo ) );
				break;
			}
		}
	}

	if ( settings.value( "Terminals/mate-terminal" ).toString().isEmpty() ) {
		QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
		foreach( QString path, paths ) {
			if ( QFileInfo( path + "/mate-terminal" ).exists() ) {
				QStringList termInfo = QStringList() << "mate-terminal" << "--working-directory" << "%1" << "-x" << "%2";
				settings.setValue( "Terminals/mate-terminal", QVariant( termInfo ) );
				break;
			}
		}
	}

	if ( settings.value( "Terminals/xfce4-terminal" ).toString().isEmpty() ) {
		QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
		foreach( QString path, paths ) {
			if ( QFileInfo( path + "/xfce4-terminal" ).exists() ) {
				QStringList termInfo = QStringList() << "xfce4-terminal" << "--working-directory" << "%1" << "-x" << "%2";
				settings.setValue( "Terminals/xfce4-terminal", QVariant( termInfo ) );
				break;
			}
		}
	}

	if ( settings.value( "Terminals/xterm" ).toString().isEmpty() ) {
		QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
		foreach( QString path, paths ) {
			if ( QFileInfo( path + "/xterm" ).exists() ) {
				settings.setValue( "Terminals/xterm", QVariant( QStringList() << "xterm" << "-e" << "cd %1 && %2" ) );
				break;
			}
		}
	}

	/*
		*
		* Store the settings from QSettings to NBSettings
		*
	*/

	Settings.readSettings();
};
