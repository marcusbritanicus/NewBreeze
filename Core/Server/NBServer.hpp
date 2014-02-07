/*
	*
	* NBServer.hpp - NBServer Header
	*
*/

#pragma once
#ifndef NBSERVER_HPP
#define NBSERVER_HPP

#include <Global.hpp>

class NBServer : public QLocalServer {
	Q_OBJECT

	public:
		NBServer();
		bool start();

	private:
		QString mServerName;

	private slots:
		void handleNewConnection();

	Q_SIGNALS:
		void newWindow( QString path );
};

class ThreadedReplier : public QObject {
	Q_OBJECT

	public:
		ThreadedReplier( QLocalSocket* );
		void talkToClient();

	private:
		QLocalSocket *client;

	private slots:
		void sendReplyToClient();

	Q_SIGNALS:
		void finished();
		void newWindow( QString path );
};

bool isServerRunning();

#endif
