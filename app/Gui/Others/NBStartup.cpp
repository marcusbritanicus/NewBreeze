/*
	*
	* NBStartup.cpp - NewBreeze Startup Configurator
	*
*/

#include "Global.hpp"
#include "NBPluginManager.hpp"
#include "NBMessageDialog.hpp"
#include "NBBugReporter.hpp"

void detectCrash() {

	/* We check for an ending good by in the last 12 chars. */
	// QFile nblogf( logPath );
	// if ( nblogf.open( QFile::ReadOnly ) ) {
		// nblogf.seek( nblogf.size() - 12 );

		// QString goodbye = QString::fromLocal8Bit( nblogf.read( 12 ) );
		// if ( not goodbye.contains( "Good Bye!" ) ) {
			// int reply = NBMessageDialog::question(
				// 0,		/* Not a child of anything */
				// "NewBreeze - Bug Report",
				// "It seems to me that NewBreeze did not close properly the last time. If you were working on something very important at the time, "
				// "I'm extremely sorry. I hope no data was lost. Would you like to report this on GitHub?"
			// );

			// if ( reply == QMessageBox::Yes ) {
				// NBBugReporter *bugreport = NBBugReporter( this );
				// bugreport->exec();
			// }

			// /*
				// *
				// * Creating an issue
				// *
				// * curl -i -u "user:passphrase" -d '{ "title": "Title", "body": "Body", "label": "Label" }' https://api.github.com/repos/marcusbritanicus/NewBreeze/issues
				// * https://curl.haxx.se/libcurl/c/http-post.html
				// *
			// */
		// }
	// }
	// nblogf.close();
};

void NBStartup() {

	/*
		*
		* Crash Detector?
		*
	*/
	detectCrash();

	/*
		*
		* Check if thumbDir exists. Otherwise try to create it.
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
		* Set the Icon Theme
		*
	*/
	QIcon::setThemeName( NBSystemIconTheme() );

	/*
		*
		* Application style
		*
	*/
	qApp->setStyle( QStyleFactory::create( Settings->General.Style ) );

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
		QTimer::singleShot( 0, NBPluginManager::instance(), SLOT( reloadPlugins() ) );
};
