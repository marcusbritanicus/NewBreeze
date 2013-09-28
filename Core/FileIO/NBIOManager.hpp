/*
	*
	* NBFileCopy.hpp - NewBreeze FileCopy Class Header
	*
*/

#ifndef NBIOMANAGER_HPP
#define NBIOMANAGER_HPP

#include <Global.hpp>
#include <NBFileIO.hpp>

class NBIOManager : public QObject {
	Q_OBJECT

	public:
		// Init all the variables
		NBIOManager();

		// addJob( sourceList, target, IOMode, JobMode );
		quint64 addJob( QStringList, QString, NBIOMode::Mode, NBJobMode::Mode );

		// cancelJob( jobID );
		bool cancelJob( quint64 );

		// hasPendingJobs();
		bool hasPendingJobs();

		// pendingJobsCount();
		bool pendingJobsCount();

		// progress of job @jobID
		float jobProgress( quint64 jobID );

		// total percentage completed ( of all jobs )
		float totalProgress();

		// Number of active jobs
		quint64 activeJobs();

	private:
		QList<NBFileIO*> jobList;
		QTimer *timer;

	private slots:
		void manageTimer();
		void progressClick();

	Q_SIGNALS:
		// emit jobAdded( jobID, sourceList, target, iomode );
		void jobAdded( quint64, QStringList, QString, NBIOMode::Mode );

		// emit jobComplete( jobID, sourceList, target, errorNodes );
		void jobComplete( quint64, QStringList, QString, QStringList );

		// emit allJobsCompleted();
		void allJobsCompleted();

		// emit copied( jobID, cfilePC, totalPC );
		void copied( quint64, QString, float, float );

		// emit newNode( targetNode );
		void newNode( QString );

		// emit ioVolume( jobID, files, totalBytesToBeCopied );
		void ioVolume( quint64, quint64, quint64 );

		// emit copied( percentage );
		void copied( float );
};

#endif
