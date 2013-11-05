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
#include <NBServer.hpp>
#include <NewBreeze.hpp>
#include <NBPasteDialog.hpp>
#include <NBSettingsManager.hpp>

int main( int argc, char **argv ) {

	qInstallMsgHandler( NBMessageOutput );
	qRegisterMetaType<NBDeviceInfo>( "NBDeviceInfo" );

	QApplication app( argc, argv );
	app.setOrganizationName( "NewBreeze" );
	app.setApplicationName( "NewBreeze" );

	if ( ( argc >= 3 ) and ( app.arguments().indexOf( QRegExp( "^--(cut|copy)pastemode$" ) ) != -1 ) ) {
		/*
			*
			* Check the format:
			*
			* NewBreeze --cutpastemode --targetdir dir --sources src1 src2 src3 src4 ...
			* NewBreeze --copypastemode --targetdir dir --sources src1 src2 src3 src4 ...
			*
			* Alternatively: NOT IMPLEMENTED
			*
			* NewBreeze --cutpastemode tgtdir src1 src2 src3 src4 ...
			* NewBreeze --copypastemode tgtdir src1 src2 src3 src4 ...
			*
		*/

		NBPasteDialog *pasteDlg = new NBPasteDialog();
		pasteDlg->show();
		return pasteDlg->result();
	}

	else if ( ( argc == 2 ) and app.arguments()[ 1 ] == QString( "--settings" ) ) {
		NBSettingsManager *settingsMgr = new NBSettingsManager();
		settingsMgr->show();
	}

	else {
		if ( isServerRunning() ) {
			qDebug() << "Running server found";
			qDebug() << "Requesting server for new window";

			QTcpSocket *client = new QTcpSocket();
			client->connectToHost( QHostAddress::LocalHost, 14928 );
			client->waitForConnected( 100 );
			if ( client->state() == QTcpSocket::ConnectedState ) {
				client->write( "NewWindow" );
				client->waitForBytesWritten( -1 );
				client->waitForReadyRead( -1 );
				if ( client->readAll() == "Path" ) {
					if ( argc >= 2 ) {
						client->write( argv[ 1 ] );
						client->waitForBytesWritten();
						qDebug() << "Requested for new window at:" << argv[ 1 ];
						client->disconnectFromHost();
					}

					else {
						client->write( "NULL" );
						client->disconnectFromHost();
					}

					return 0;
				}
			}

			else {
				qCritical() << "Unable to communicate with Server.";
				qCritical() << "Starting separate process";

				NBStartup();
				NewBreeze *Gui;
				if ( argc >= 2 )
					Gui = new NewBreeze( app.tr( argv[ 1 ] ) );

				else
					Gui = new NewBreeze();

				if ( Settings->Session.Maximized )
					Gui->showMaximized();

				else
					Gui->showNormal();
			}

			return app.exec();
		}

		qDebug() << "Server not found.";
		qDebug() << "Starting new server.";

		NBStartup();
		NBServer *server = new NBServer();
		server->start();

		NewBreeze *Gui;
		if ( argc >= 2 )
			Gui = new NewBreeze( app.tr( argv[ 1 ] ) );

		else
			Gui = new NewBreeze();

		if ( Settings->Session.Maximized )
			Gui->showMaximized();

		else
			Gui->showNormal();

		QObject::connect( server, SIGNAL( newWindow( QString ) ), Gui, SLOT( newWindow( QString ) ) );
	}

	return app.exec();
};
