/*
	*
	* NBProcessManagerUI.hpp - NBProcessManagerUI.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDialog.hpp"
#include "NBAbstractProcess.hpp"
#include "NBProcessWidget.hpp"
#include "NBIconManager.hpp"
#include "NBMessageDialog.hpp"
#include "NBProcessManager.hpp"

class NBProcessManagerUI : public NBDialog {
	Q_OBJECT

	public:
		/* Global static instance */
		static NBProcessManagerUI* instance();

	private:
		NBProcessManagerUI();

		void createGUI();
		void setWindowProperties();

		QVBoxLayout *baseLyt;

		NBProcessManager *pMgr;
		static NBProcessManagerUI* procMgrUI;

		static int procCount;

	public Q_SLOTS:
		void show();

	private Q_SLOTS:
		void addProcess( NBProcess::Progress*, NBAbstractProcess* );
};

class NBProcessManagerMini : public QToolButton {
	Q_OBJECT

	public:
		// Init all variables
		NBProcessManagerMini( QWidget *parent );

		// Show the Process Manager
		void showProcessManager();

	private:
		NBProcessManagerUI *procMgrUI;
		NBProcessManager *pMgr;

		QBasicTimer timer;

		qreal fraction;
		quint64 jobs;

	private Q_SLOTS:
		void updateTimer();

		// Show/Hide Process Manager;
		void toggleProcessManager();

	protected:
		void timerEvent( QTimerEvent * );
		void paintEvent( QPaintEvent * );
};
