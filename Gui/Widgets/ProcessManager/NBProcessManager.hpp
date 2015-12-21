/*
	*
	* NBProcessManager.hpp - NBProcessManager.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <CoreProcess.hpp>
#include <PNBProcessManagerGUI.hpp>

class NBProcessManager : public QObject {
	Q_OBJECT

	public:
		NBProcessManager( QObject *parent );
		void addProcess( NBCoreProcess *proc );

	private:
		QBasicTimer updateTimer;
		NBProcessManagerGUI *procGUI;
		NBProcessManagerGUIMini *procMiniGUI;

	protected:
		void timerEvent( QTimerEvent * );
};
