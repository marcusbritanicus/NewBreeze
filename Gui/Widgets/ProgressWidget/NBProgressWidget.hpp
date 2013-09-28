/*
	*
	* NBProgressWidget.hpp - NBProgressWidget.cpp header
	*
*/

#ifndef NBPROGRESSWIDGET_HPP
#define NBPROGRESSWIDGET_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBProgressDialog.hpp>
#include <NBFileIO.hpp>

class NBProgressPopup;

class NBProgressWidget : public QFrame {
	Q_OBJECT

	public:
		// Init all variables
		NBProgressWidget();

		// Destroy all variables
		~NBProgressWidget();

		// addJob( sourceList, target, IOMode, JobMode );
		void addJob( QStringList, QString, NBIOMode::Mode, NBJobMode::Mode );

		// cancelJob( jobID );
		bool cancelJob( quint64 );

		// hasPendingJobs();
		bool hasPendingJobs();

		// pendingJobsCount();
		quint64 pendingJobsCount();

		// progress of job @jobID
		float jobProgress( quint64 jobID );

		// total percentage completed ( of all jobs )
		float totalProgress();

		// Number of active jobs
		quint64 activeJobs();

	private:
		NBProgressDialog *pDialog;

		QPainter *painter;
		float totalPC, cfilePC;

		bool checked;

		NBProgressPopup *progressPopup;
		QList<NBFileIO*> jobList;
		QTimer *timer;

	protected:
		void paintEvent( QPaintEvent * );
		void mousePressEvent( QMouseEvent * );

	private slots:
		void uncheck();

		void manageTimer();

		// Update the information for the job @jobID
		// UpdateInfo( jobID, files, bytes );
		void updateInfo( quint64, quint64, quint64 );

		// Update the progress information for the job @jobID
		// updateProgress( jobID, cFileName, cfileF, totalF );
		void updateProgress( quint64, QString, float, float );

		// Intimate the user about completion of a job
		// handleAllJobsCompleted( jobID, errorNodes )
		void handleJobComplete( quint64, QStringList );

	Q_SIGNALS:
		// emit newNode( targetNode );
		void newNode( QString );
};

class NBProgressDisplay : public QWidget {
	Q_OBJECT

	public:
		NBProgressDisplay( QStringList sourceList, QString target );

		// updateIndo( totalFiles, totalBytes );
		void updateInfo( quint64, quint64 );

		// updateProgress( curFile, totalProgress, cfileProgress )
		void updateProgress( QString, float, float );

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

#endif
