/*
	*
	* NBStartup.cpp - NewBreeze Startup Configurator
	*
*/

#include "Global.hpp"
#include "NBAutoMount.hpp"
#include "NBPluginManager.hpp"
#include "NBMessageDialog.hpp"
#include "NBBugReporter.hpp"
#include "NBStartupWizard.hpp"
#include <QtConcurrent>

void NBStartup() {

	/*
		*
		* StartupWizard
		*
	*/
	if ( not exists( NBXdg::home() + ".config/NewBreeze/NewBreeze.conf" ) ) {
		NBStartupWizard *startWiz = new NBStartupWizard();
		startWiz->exec();
	}

	/*
		*
		* StartupWizard: Start it if users have an old config
		*
		* Delete the old settings, and create a new one
		*
	*/
	QSettings sett( "NewBreeze", "NewBreeze" );
	if ( not sett.contains( "Version" ) or sett.value( "Version" ).toString() != "3.0.0" ) {

		/* Removing the old config */
		QFile::remove( sett.fileName() );

		/* Create a new config */
		NBStartupWizard *startWiz = new NBStartupWizard();
		startWiz->exec();
	}

	/*
		*
		* AutoMount service
		*
	*/
	if ( Settings->value( "AutoMount" ) )
		NBAutoMount::instance()->start();

	/*
		*
		* Check if thumbDir exists. Otherwise try to create it.
		*
	*/
	if ( !QFileInfo( thumbsDir ).exists() ) {
		bool out = QDir::home().mkpath( thumbsDir );
		if ( !out ) {
			qDebug() << "Error settings thumbnail cache path: " << thumbsDir;
			qDebug() << "Thumbnailing disabled.";
			Settings->setValue( "View/FilePreviews", false );
		}
	}

	/*
		*
		* Set the Icon Theme
		*
	*/
	QIcon::setThemeName( Settings->value( "View/IconTheme" ) );

	/*
		*
		* Application style
		*
	*/
	qApp->setStyle( QStyleFactory::create( Settings->value( "View/Style" ) ) );

	/*
		*
		* Set hard coded CustomActions: Open as SU, Open in terminal.
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
		* Detect and save the terminals available
		*
	*/

	QSettings settings( "NewBreeze", "NewBreeze" );
	if ( settings.value( "Terminals/desq-term" ).toString().isEmpty() ) {
		QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
		foreach( QString path, paths ) {
			if ( QFileInfo( path + "/desq-term" ).exists() ) {
				QStringList termInfo = QStringList() << "desq-term" << "--workdir" << "%1" << "-e" << "%2";
				settings.setValue( "Terminals/desq-term", QVariant( termInfo ) );
				break;
			}
		}
	}

	if ( settings.value( "Terminals/coreterminal" ).toString().isEmpty() ) {
			QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
		foreach( QString path, paths ) {
			if ( QFileInfo( path + "/coreterminal" ).exists() ) {
				QStringList termInfo = QStringList() << "coreterminal" << "-w" << "%1" << "-e" << "%2";
				settings.setValue( "Terminals/coreterminal", QVariant( termInfo ) );
				break;
			}
		}
	}

	if ( settings.value( "Terminals/qterminal" ).toString().isEmpty() ) {
		QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
		foreach( QString path, paths ) {
			if ( QFileInfo( path + "/qterminal" ).exists() ) {
				QStringList termInfo = QStringList() << "qterminal" << "--workdir" << "%1" << "-e" << "%2";
				settings.setValue( "Terminals/qterminal", QVariant( termInfo ) );
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

	if ( settings.value( "Terminals/lxterminal" ).toString().isEmpty() ) {
		QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
		foreach( QString path, paths ) {
			if ( QFileInfo( path + "/lxterminal" ).exists() ) {
				QStringList termInfo = QStringList() << "lxterminal" << "--working-directory" << "%1" << "-e" << "%2";
				settings.setValue( "Terminals/lxterminal", QVariant( termInfo ) );
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

	if ( not settings.contains( "Terminals/Default" ) ) {
		settings.setValue( "Terminals/Default", "Inbuilt" );
		settings.sync();
	}

	/*
		*
		* QFont Substitutions - DejaVu Sans Mono is used for addressbar, if not found use courier, or mono
		*
	*/

	QFont::insertSubstitutions( "DejaVu Sans Mono", QStringList() << "Courier" << "Courier New" << "Courier 10 Pitch" << "mono" );

	/*
		*
		* Init the plugins - only if we are not the root user
		*
	*/
	if ( getuid() )
		QtConcurrent::run( NBPluginManager::instance(), &NBPluginManager::reloadPlugins );
};
