/*
	*
	* NBProcessManager.hpp - NBProcessManager.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBDeviceInfo.hpp>
#include <NBAbstractProcess.hpp>
#include <NBIOProcess.hpp>

typedef QList<quint64> ProcessIDList;
typedef QList<NBAbstractProcess*> ProcessList;
typedef QList<NBProcess::Progress *> ProgressList;

class NBProcessManager : public QObject {
	Q_OBJECT

	public:
		/* One single global instance */
		static NBProcessManager* instance();

		/* Total number of processes */
		quint64 processCount();

		/* All the processes */
		ProcessList processes();

		/* All the processes */
		ProgressList progresses();

		/* Number of active processes */
		quint64 activeProcessCount();

		/* All the processes */
		ProcessList activeProcesses();

		/* All the processes */
		ProgressList activeProgresses();

		/* Add a process */
		quint64 addProcess( NBAbstractProcess *, NBProcess::Progress *progress );

		/* Get the process for a process ID */
		NBAbstractProcess* process( quint64 );

		/* Get the progress details for a process ID */
		NBProcess::Progress* progress( quint64 );

	private:
		NBProcessManager() : QObject() {

			init = true;
		};

		/* Our internal global static instance */
		static NBProcessManager *pMgr;

		/* Init flag */
		bool init;

		ProcessIDList completedProcIDs;
		ProgressList progressList;
		ProcessList processList;

	private Q_SLOTS:
		void handleProcessComplete();

	Q_SIGNALS:
		void processAdded( NBProcess::Progress* );
};
