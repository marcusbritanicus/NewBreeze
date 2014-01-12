/*
	*
	* Copyright 2013 Britanicus <marcusbritanicus@gmail.com>
	*

	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 2 of the License, or
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

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBServer.hpp>
#include <NewBreeze.hpp>
#include <NBSettingsManager.hpp>

#include <NBIOManager.hpp>
#include <NBFileIO.hpp>

#include <NBCLParser.hpp>

void startInstance( QApplication &app, bool startServer = false ) {

	NBServer *server;
	NewBreeze *Gui;

	if ( startServer ) {
		server = new NBServer();
		qDebug( "Starting server: %s", ( server->start() ? "[ DONE ]" :"[FAILED]" ) );
	}

	qDebug( "Starting NewBreeze instance..." );

	NBStartup();

	if ( app.arguments().count() >= 2 )
		Gui = new NewBreeze( app.arguments().at( 1 ) );

	else
		Gui = new NewBreeze();

	if ( Settings->Session.Maximized )
		Gui->showMaximized();

	else
		Gui->showNormal();

	if ( startServer )
		app.connect( server, SIGNAL( newWindow( QString ) ), Gui, SLOT( newWindow( QString ) ) );
};

int main( int argc, char **argv ) {

	#if QT_VERSION >= 0x050000
		qInstallMessageHandler( NBMessageOutput5 );
	#else
		qInstallMsgHandler( NBMessageOutput );
	#endif
	qRegisterMetaType<NBDeviceInfo>( "NBDeviceInfo" );

	QApplication app( argc, argv );
	app.setOrganizationName( "NewBreeze" );
	app.setApplicationName( "NewBreeze" );
	app.setPalette( NBStyleManager::getPalette( Settings->General.Style ) );

	switch( NBArgParser( argc, argv ) ) {

		/* We want only the settings */
		case SETTINGS : {
			NBSettingsManager *settingsMgr = new NBSettingsManager();
			settingsMgr->show();

			return app.exec();
		}

		/* A forced opening */
		case FORCENEW : {
			qCritical( "Killing the existing server" );
			qWarning( "This may have caused data loss. Be warned." );

			startInstance( app, true );
			return app.exec();
		}

		/* A normal opening */
		case NORMAL : {
			/* If the server is running */
			if ( isServerRunning() ) {
				qDebug( "Running server found" );
				qDebug( "Requesting server for new window" );

				/* Connect to the server */
				QLocalSocket *client = new QLocalSocket();
				client->connectToServer( "NewBreeze-1000" );
				client->waitForConnected( 100 );

				/* If we are connected... */
				if ( client->state() == QLocalSocket::ConnectedState ) {
					/* The server would have said 'Welcome...', wait for it and discard it */
					if ( not client->bytesAvailable() )
						client->waitForReadyRead();

					client->readAll();

					/* Prepare the query */
					QString query;
					/* Assume the second argument is the path to be opened */
					if ( argc >= 2 )
						query = QString::fromLocal8Bit( argv[ 1 ] );

					/* There was no argument. Try to open the last opened folder, failing which we open the home directory */
					else
						query = ( NBSettings::instance()->Session.LastDir.isEmpty() ? QDir::homePath() : NBSettings::instance()->Session.LastDir );

					/* Query for a new window */
					qDebug() << "Requesting the server to open the path:" << query;
					client->write( QString( "NewWindow @ %1" ).arg( query ).toLatin1() );
					qDebug() << ( client->waitForBytesWritten( -1 ) ? "Request sent" : "Request failed" );

					/* Wait 30 secs for the server to ackowledge. The reply must be within a second. */
					client->waitForReadyRead();

					/* If the server said okay */
					/* 		=> Actually the server would have said 'Welcome ...' which we did not bother reading */
					/* 		=> We must take that into consideration */
					QString reply = client->readAll();
					if ( reply.contains( "Done" ) ) {
						qDebug() << "Server opening the given path.";
						qDebug() << "Good Bye!";
					}

					/* Server didnot respond in 30 secs. Something must ahve gone wrong. Tell the user. */
					else {
						qDebug() << "Server said:" << reply;
						qCritical() << "Something wicked has happened. Server did not respond appropriately.";
						qCritical() << "Try" << argv[ 0 ] << "--force-new to open without talking to server.";
					}

					return 0;
				}

				/* Some how we were disconnected from the server before asking for new window */
				else {
					qCritical() << "Unable to communicate with Server.";
					qCritical() << "Starting separate process";

					startInstance( app, false );
				}
			}

			/* No server is running */
			else {
				startInstance( app, true );
				return app.exec();
			}
		}
	}
};
