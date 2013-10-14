/*
	*
	* NBIOManager.hpp - NBIOManager.cpp header
	*
*/

#ifndef NBIOManager_HPP
#define NBIOManager_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBProgressDialog.hpp>
#include <NBFileIO.hpp>

class NBProgressDisplay : public QWidget {
	Q_OBJECT

	public:
		NBProgressDisplay( QStringList sourceList, QString target );

		// updateIndo( totalFiles, totalBytes );
		void updateInfo( quint64, quint64 );

		// updateProgress( curFile, totalProgress, cfileProgress )
		void updateProgress( QString, qreal, qreal );

	private:
		QLabel *sourceLbl, *targetLbl;
		QLabel *curFileLbl;
		QProgressBar *totalPBar, *cfilePBar;
};

class NBProgressPopup : public QScrollArea {
	Q_OBJECT

	public:
		NBProgressPopup();

		// addLob( jobID, sourceList, target, iomode )
		void addJob( quint64, QStringList, QString, NBIOMode::Mode );
		NBProgressDisplay* job( quint64 );

	private:
		QList<NBProgressDisplay*> jobProgressList;
};

class NBIOManager : public QFrame {
	Q_OBJECT

	public:
		// Init all variables
		NBIOManager();

		// Destroy all variables
		~NBIOManager();

		// addJob( sourceList, target, IOMode, JobMode );
		void addJob( QStringList, QString, NBIOMode::Mode );

		// cancelJob( jobID );
		bool cancelJob( QString );

		// hasPendingJobs();
		bool hasPendingJobs();

		// pendingJobsCount();
		quint64 pendingJobsCount();

		// progress of job @jobID
		qreal jobProgress( QString jobID );

		// total percentage completed ( of all jobs )
		qreal totalProgress();

		// Number of active jobs
		int activeJobs();

	private:
		QMap<QString, Job> jobList;
		QMap<QString, Job> pendingJobs;

		QPainter *painter;
		qreal totalF, cfileF;

		bool checked;

		NBProgressPopup *progressPopup;
		QTimer *timer;

	protected:
		void paintEvent( QPaintEvent * );
		void mousePressEvent( QMouseEvent * );

	private slots:
		void uncheck();

		void manageTimer();

		// Update the progress information for the job @jobID
		// updateProgress( jobID, cFileName, cfileF, totalF );
		void updateProgress();

		// Intimate the user about completion of a job
		// handleJobComplete( job )
		void handleJobComplete( Job * );
};

#endif
