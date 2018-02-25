/*
	*
	* Copyright 2018 Britanicus <marcusbritanicus@gmail.com>
	*

	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 3 of the License, or
	* (at your option) any later version.
	*

	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.
	*

	*
	* You should have received a copy of the GNU General Public License
	* along with this program; if not, write to the Free Software
	* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	* MA 02110-1301, USA.
	*
*/

#include "Global.hpp"

#include "NewBreeze.hpp"

#include "NBMessageDialog.hpp"
#include "AboutNB.hpp"
#include "NBSettingsManager.hpp"
#include "NewBreezeTray.hpp"

#include "NBLogger.hpp"
#include "NBCLParser.hpp"
#include "NBApplication.hpp"

int main( int argc, char **argv ) {

	#if QT_VERSION >= 0x050000
		qInstallMessageHandler( NBMessageOutput5 );
	#else
		qInstallMsgHandler( NBMessageOutput );
	#endif

	qRegisterMetaType<NBDeviceInfo>( "NBDeviceInfo" );

	/* Necessary when using QWebEngine from plugins */
	#if QT_VERSION >= 0x050400
		QCoreApplication::setAttribute( Qt::AA_ShareOpenGLContexts );
	#endif

	NBApplication app( argc, argv, "NewBreeze" );

	/* About Application */
	app.setOrganizationName( "NewBreeze" );
	app.setApplicationName( "NewBreeze" );

	/* Startup */
	NBStartup();

	if ( Settings->General.TrayIcon )
		app.setQuitOnLastWindowClosed( false );

	else
		app.setQuitOnLastWindowClosed( true );

	/* Proxy Setup */
	QNetworkProxyQuery npq( QUrl( "http://www.google.com" ) );
	QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery( npq );
	if ( listOfProxies.size() )
		QNetworkProxy::setApplicationProxy( listOfProxies[ 0 ] );

	/* Arg Parser */
	switch( NBArgParser( argc, argv ) ) {

		/* We want only the server running in the tray */
		case SYSTRAY : {
			/* If an instance is already running, inform the user about it. */
			if ( app.isRunning() ) {
				qDebug() << "Found a running instance. Exiting";
				NBMessageDialog::information(
					NULL,
					"NewBreeze - Open in tray",
					"NewBreeze is now open in SystemTray. Left click to show/hide all the open windows, Double click to open a new window, middle click to quit."
				);
				return 0;
			}

			/* Else, start an instance. */
			else {
				qDebug() << "No running instance of the NewBreeze found";

				/* Here we do not request the tray icon as we directly open it */
				NewBreeze *Gui = new NewBreeze( QString() );
				QObject::connect( &app, SIGNAL( messageReceived( const QString ) ), Gui, SLOT( handleMessages( const QString ) ) );

				NBTrayIcon* trayIcon = new NBTrayIcon();
				trayIcon->show();

				QObject::connect( trayIcon, SIGNAL( newWindow() ), Gui, SLOT( newWindow() ) );

				return app.exec();
			}

			break;
		}

		/* We want only the settings */
		case SETTINGS : {
			qDebug() << "Showing Settings Dialog";
			NBSettingsManager *settingsMgr = new NBSettingsManager();
			settingsMgr->exec();

			return 0;
		}

		/* We want only the about dialog */
		case ABOUTNB : {
			qDebug() << "Showing About NB dialog";
			AboutNB *aboutNB = new AboutNB();
			aboutNB->exec();

			return 0;
		}

		/* We want only the about dialog */
		case LICENSE : {
			qDebug() << "Showing NewBreeze License";
			License *license = new License();
			license->exec();

			return 0;
		}

		/* A forced opening */
		case FORCENEW : {
			if ( app.isRunning() ) {
				qCritical( "Killing the existing server" );
				qWarning( "This may have caused data loss. Be warned." );

				app.disconnect();
			}

			NewBreeze *Gui;
			if ( app.arguments().count() >= 3 )
				Gui = new NewBreeze( app.arguments().at( 2 ) );

			else
				Gui = new NewBreeze( QString() );

			QObject::connect( &app, SIGNAL( messageReceived( const QString ) ), Gui, SLOT( handleMessages( const QString ) ) );

			if ( Settings->Session.Maximized )
				Gui->showMaximized();

			else
				Gui->showNormal();

			if ( Settings->General.TrayIcon ) {
				NBTrayIcon* trayIcon = new NBTrayIcon();
				trayIcon->show();

				QObject::connect( trayIcon, SIGNAL( newWindow() ), Gui, SLOT( newWindow() ) );
			}

			return app.exec();
		}

		/* A normal opening */
		case NORMAL : {
			/* If the server is running */
			if ( app.isRunning() ) {
				qDebug( "Running server found" );
				qDebug( "Requesting server for new window" );

				/* Prepare the query */
				QString query;
				/* Assume the second argument is the path to be opened */
				if ( argc >= 2 )
					query = QString::fromLocal8Bit( argv[ 1 ] );

				/* There was no argument. Try to open the last opened folder, failing which we open the home directory */
				else
					query = ( NBSettings::instance()->Session.LastDir.isEmpty() ? QDir::homePath() : NBSettings::instance()->Session.LastDir );

				/* Query for a new window */
				int result = app.sendMessage( query );
				qDebug() << "Requesting the server to open the path:" << query << ":" << ( result ? "Request sent" : "Request failed" );

				/* Some how we were disconnected from the server before asking for new window */
				if ( not result ) {
					/* Disconnect from server */
					app.disconnect();

					/* Close all top level widgets */
					Q_FOREACH( QWidget *w, app.topLevelWidgets() )
						w->close();

					NewBreeze *Gui = new NewBreeze( query );
					QObject::connect( &app, SIGNAL( messageReceived( const QString ) ), Gui, SLOT( handleMessages( const QString ) ) );

					if ( Settings->Session.Maximized )
						Gui->showMaximized();

					else
						Gui->showNormal();

					if ( Settings->General.TrayIcon ) {
						NBTrayIcon* trayIcon = new NBTrayIcon();
						trayIcon->show();

						QObject::connect( trayIcon, SIGNAL( newWindow() ), Gui, SLOT( newWindow() ) );
					}

					return app.exec();
				}

				return 0;
			}

			/* Server not running */
			else {
				NewBreeze *Gui;
				if ( app.arguments().count() >= 2 )
					Gui = new NewBreeze( app.arguments().at( 1 ) );

				else
					Gui = new NewBreeze( QString() );

				QObject::connect( &app, SIGNAL( messageReceived( const QString ) ), Gui, SLOT( handleMessages( const QString ) ) );

				if ( Settings->Session.Maximized )
					Gui->showMaximized();

				else
					Gui->showNormal();

				if ( Settings->General.TrayIcon ) {
					NBTrayIcon* trayIcon = new NBTrayIcon();
					trayIcon->show();

					QObject::connect( trayIcon, SIGNAL( newWindow() ), Gui, SLOT( newWindow() ) );
				}

				return app.exec();
			}
		}
	}
};
