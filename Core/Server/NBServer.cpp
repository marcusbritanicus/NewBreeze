/*
	*
	* NBServer.cpp - NewBreeze Server Module Class
	*
*/

#include <NBServer.hpp>
#include <NBIconProvider.hpp>
#include <NBTools.hpp>

NBServer::NBServer() : QTcpServer() {

	QNetworkConfigurationManager cfgMgr;
	QNetworkConfiguration config = cfgMgr.defaultConfiguration();

	networkSession = new QNetworkSession( config );
	networkSession->open();
};

bool NBServer::start() {

	if ( listen( QHostAddress( QHostAddress::LocalHost ), 14928 ) ) {
		// qDebug() << "Server started...!";
		// qDebug() << "Addr:" << serverAddress().toString();
		// qDebug() << "Port:" << serverPort();
	}

	else {
		qCritical() << "Unable to start server";
		return false;
	}

	return true;
};

void NBServer::incomingConnection( int sockDescr ) {

	ThreadedReplier *replier = new ThreadedReplier( this, sockDescr );
	connect( replier, SIGNAL( finished() ), replier, SLOT( deleteLater() ) );
	connect( replier, SIGNAL( newWindow( QString ) ), this, SIGNAL( newWindow( QString ) ) );
	replier->start();
};

ThreadedReplier::ThreadedReplier( QObject *parent, int sockDescr ) : QThread( parent ), socketDescr( sockDescr ) {
};

void ThreadedReplier::run() {

	/*
		*
		* The query process is as follows:
		*
		* Q: Good Morning!
		* R: Welcome to NBServer!
		*
		* Q: AllInfo OR Icon OR MimeType OR Description
		* R: Path
		*
		* Q : <URL>
		* R1: <URL>\n<ICON>\n<DESC>\n<MIME>\n
		* R2: <URL>\n<ICON>\n
		* R3: <URL>\n<DESC>\n
		* R4: <URL>\n<MIME>\n
		*
		* Q: <Anything else>
		* R: Invalid query
		*
	*/

	QTcpSocket *client = new QTcpSocket();
	client->setSocketDescriptor( socketDescr);

	QStringList qTypes = QStringList() << "Info" << "Icon" << "MimeType" << "Description" << "MimeAncestors";

	while ( client->state() == QTcpSocket::ConnectedState ) {
		client->waitForReadyRead( -1 );
		QString queryType = QString::fromLocal8Bit( client->readAll().data() );

		if ( queryType == QString( "Good Morning!" ) ) {
			qDebug() << "Client says Good Morning!";
			client->write( QString( "Welcome to NewBreeze!" ).toLocal8Bit() );
		}

		else if ( queryType == QString( "NewWindow" ) ) {
			qDebug() << "Client requested New Window";
			client->write( "Path" );
			client->waitForBytesWritten( -1 );
			client->waitForReadyRead( -1 );
			QString query = QString::fromLocal8Bit( client->readAll() );
			qDebug() << "Path:" << query;

			emit newWindow( query );
		}

		else {
			client->write( "Invalid query" );
		}
	}

	exec();
}

bool isServerRunning() {

	QTcpSocket *client = new QTcpSocket();
	client->connectToHost( QHostAddress::LocalHost, 14928 );

	if ( client->waitForConnected( 100 ) ) {
		client->write( "Good Morning!" );
		client->waitForReadyRead();
		if ( client->readAll() == "Welcome to NewBreeze!" ) {
			client->disconnectFromHost();
			return true;
		}

		else {
			client->disconnectFromHost();
			return false;
		}
	}

	else {
		return false;
	}
};
