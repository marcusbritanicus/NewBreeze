/*
	*
	* NBServer.cpp - NewBreeze Server Module Class
	*
*/

#include <NBServer.hpp>
#include <NBIconProvider.hpp>
#include <NBTools.hpp>

NBServer::NBServer() : QLocalServer() {

	mServerName = QString( "NewBreeze-%1" ).arg( getuid() );
	removeServer( mServerName );

	connect( this, SIGNAL( newConnection() ), this, SLOT( handleNewConnection() ) );
};

bool NBServer::start() {

	if ( listen( mServerName ) ) {
		// qDebug() << "Server started...!";
		// qDebug() << "Addr:" << mServerName;

		return true;
	}

	else {
		qCritical() << "Unable to start server" << errorString();
		return false;
	}
};

void NBServer::handleNewConnection() {

	QLocalSocket *socket = nextPendingConnection();

	/* If the client is read only, then say 'Welcome ...' and end the story. */
	qDebug() << "Client connected. Welcoming...";
	socket->write( "Welcome to NewBreeze!" );
	socket->waitForBytesWritten( -1 );

	if ( socket->openMode() == QIODevice::ReadWrite ) {
		/* Set up reply interface, if the client is read-write */
		ThreadedReplier *replier = new ThreadedReplier( socket );
		connect( replier, SIGNAL( finished() ), replier, SLOT( deleteLater() ) );
		connect( replier, SIGNAL( newWindow( QString ) ), this, SIGNAL( newWindow( QString ) ) );
		connect( replier, SIGNAL( showTrayIcon() ), this, SIGNAL( showTrayIcon() ) );
	};
};

ThreadedReplier::ThreadedReplier( QLocalSocket *socket ) : QObject(), client( socket ) {

	/* We had said Welcome. Flush the stream. */
	client->flush();

	/* If the client disconnects, quit */
	connect( client, SIGNAL( disconnected() ), this, SIGNAL( finished() ) );

	/* If client queries something, respond to it */
	connect( client, SIGNAL( readyRead() ), this, SLOT( sendReplyToClient() ) );

	/* If the client had queried by the time we set up shop, respond to it. */
	if ( client->bytesAvailable() )
		sendReplyToClient();
};

void ThreadedReplier::sendReplyToClient() {

	/*
		*
		* The query process is as follows:
		*
		* Q: Good Morning!
		* R: Welcome to NewBreeze!
		*
		* Q: NewWindow @ <URL>
		* R: Done
		* R: <Open a new window>
		*
		* Q: <Anything else>
		* R: Try: 'Good Morning!' or 'NewWindow @ <URL>'
		*
	*/

	/* The client must be connected at this stage. Otherwise terminate. */
	if ( client->state() != QLocalSocket::ConnectedState ) {
		emit finished();
		return;
	}

	/* We are here because we gor a readyRead(). So there must be some data. No waiting... */
	QString query = QString::fromLocal8Bit( client->readAll().data() );

	if ( query.startsWith( QString( "NewWindow @ " ) ) ) {
		qDebug() << "Client requested New Window:" << query.mid( 12 );
		emit newWindow( query.mid( 12 ) );

		client->write( "Done" );
		client->flush();

		/* Safety measure: We mostly won't nedd this blocking call */
		if ( client->state() == QLocalSocket::ConnectedState )
			client->waitForBytesWritten( -1 );
	}

	else if ( query.startsWith( QString( "Open in SystemTray" ) ) ) {
		qDebug() << "Client requested TrayIcon";
		emit showTrayIcon();

		/* Client usually disconnects, but we try to make sure */
		if ( client->state() == QLocalSocket::ConnectedState )
			client->disconnectFromServer();
	}

	else {
		client->write( "Try: 'NewWindow @ <URL>'" );
		client->flush();

		/* Safety measure: We mostly won't nedd this blocking call */
		if ( client->state() == QLocalSocket::ConnectedState )
			client->waitForBytesWritten( -1 );
	}
};

bool isServerRunning() {

	QLocalSocket *client = new QLocalSocket();
	client->connectToServer( "NewBreeze-1000", QLocalSocket::ReadOnly );

	if ( client->waitForConnected( 100 ) ) {
		/* We must get a immediate reply */
		client->waitForReadyRead();
		if ( client->readAll() == "Welcome to NewBreeze!" ) {
			client->disconnectFromServer();
			return true;
		}

		else {
			client->disconnectFromServer();
			return false;
		}
	}

	else {
		return false;
	}
};
