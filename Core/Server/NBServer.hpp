/*
	*
	* NBServer.hpp - NBServer Header
	*
*/

#ifndef NBSERVER_HPP
#define NBSERVER_HPP

#include <Global.hpp>

class NBServer : public QTcpServer {
	Q_OBJECT

	public:
		NBServer();
		bool start();

	private:
		QNetworkSession *networkSession;

	protected:
		void incomingConnection( int );

	Q_SIGNALS:
		void newWindow( QString path );
};

class ThreadedReplier : public QThread {
	Q_OBJECT

	public:
		ThreadedReplier( QObject*, int );
		void run();

	private:
		int socketDescr;

	Q_SIGNALS:
		void newWindow( QString path );
};

bool isServerRunning();

#endif
