/*
	*
	* NewBreeze.hpp - NewBreeze Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NewBreeze.hpp"

#include "NBProcessManager.hpp"
#include "NBAbstractProcess.hpp"
#include "NBIOProcess.hpp"

class NBTrayIcon : public QSystemTrayIcon {
	Q_OBJECT

	public :
		NBTrayIcon();

	public slots:
		void handleActivation( QSystemTrayIcon::ActivationReason reason );

		void processCompleted( NBProcess::Progress* );

	private slots:
		void toggleVisible();
		void showInfo();
		void quit();

	Q_SIGNALS:
		void newWindow();
};
