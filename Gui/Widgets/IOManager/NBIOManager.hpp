/*
	*
	* NBIOManager.hpp - NBIOManager.cpp header
	*
*/

#ifndef NBIOManager_HPP
#define NBIOManager_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBPasteDialog.hpp>
#include <NBFileIO.hpp>

class NBIOManager : public QFrame {
	Q_OBJECT

	public:
		// Init all variables
		NBIOManager();

		// Destroy all variables
		~NBIOManager();

		// addJob( sourceList, target, IOMode, JobMode );
		void addJob( QStringList, QString, NBIOMode::Mode );

		// pendingJobsCount();
		quint64 activeJobs();

	private:
		QList<NBPasteDialog *> jobList;

		QPainter *painter;
		qreal totalF;

		bool checked;

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
		// handleJobComplete()
		void handleJobComplete();
};

#endif
