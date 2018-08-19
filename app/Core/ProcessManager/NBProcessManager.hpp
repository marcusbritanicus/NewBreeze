/*
	*
	* NBProcessManager.hpp - NBProcessManager.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBAbstractProcess.hpp"
#include "NBIOProcess.hpp"

typedef QList<qint64> ProcessIDList;
typedef QList<NBAbstractProcess*> ProcessList;
typedef QList<NBProcess::Progress *> ProgressList;

class NBProcessManager : public QObject {
	Q_OBJECT

	public:
		/* One single global instance */
		static NBProcessManager* instance();

		/* Total number of processes */
		qint64 processCount();

		/* All the processes */
		ProcessList processes();

		/* All the processes */
		ProgressList progresses();

		/* Number of active processes */
		qint64 activeProcessCount();

		/* All the processes */
		ProcessList activeProcesses();

		/* All the processes */
		ProgressList activeProgresses();

		/* Add a process */
		qint64 addProcess( NBProcess::Progress *progress, NBAbstractProcess * );

		/* Get the process for a process ID */
		NBAbstractProcess* process( qint64 );

		/* Get the progress details for a process ID */
		NBProcess::Progress* progress( qint64 );

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
		void processAdded( NBProcess::Progress*, NBAbstractProcess* );
		void activeProcessCountChanged( qint64 );
		void processCompleted( NBProcess::Progress* );
};
