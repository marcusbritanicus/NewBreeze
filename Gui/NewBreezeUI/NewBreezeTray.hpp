/*
	*
	* NewBreeze.hpp - NewBreeze Header
	*
*/

#pragma once

#include <Global.hpp>
#include <NewBreeze.hpp>

class NBTrayIcon : public QSystemTrayIcon {
	Q_OBJECT

	public :
		NBTrayIcon();

	public slots:
		void handleActivation( QSystemTrayIcon::ActivationReason reason );

	private slots:
		void toggleVisible();
		void showInfo();
		void quit();

	Q_SIGNALS:
		void newWindow();
};
