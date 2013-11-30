/*
	*
	* NBIOManager.hpp - NBIOManager.cpp header
	*
*/

#ifndef NBIOManager_HPP
#define NBIOManager_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBDialog.hpp>
#include <NBFileIO.hpp>
#include <NBProgressBar.hpp>
#include <NBIconProvider.hpp>
#include <NBMessageDialog.hpp>

class NBIOWidget : public QWidget {
	Q_OBJECT

	public:
		NBIOWidget( NBFileIO *io );

	private:
		QLabel *ttlLbl, *srcLbl, *tgtLbl, *speedLbl, *etcLbl, *cfileLbl;
		NBClickLabel *toggleDetailsLbl, *togglePauseResumeLbl, *closeLbl;
		NBProgressBar *totalBar, *cfileBar;

		QTimer *timer;
		bool detailsAreSeen = true;

		NBFileIO *io;
		bool paused = false;

	private slots:
		void toggleDetails();
		void togglePauseResume();
		void cancelIO();
		void update();
};

class NBIOManager : public NBDialog {
	Q_OBJECT

	public:
		NBIOManager( QList<NBFileIO*> );
		void showCritical();
		void update();

	private:
		QList<NBFileIO*> ioList;
		bool killIOOnClose = false;

	protected:
		void closeEvent( QCloseEvent* );
};

class NBIOManagerMini : public QFrame {
	Q_OBJECT

	public:
		// Init all variables
		NBIOManagerMini();

		// Destroy all variables
		~NBIOManagerMini();

		// addJob( sourceList, target, IOMode, JobMode );
		void addJob( QStringList, QString, NBIOMode::Mode );

		// activeJobs();
		quint64 activeJobs();

		// showAllIODialogs();
		void showAllIODialogs();

	private:
		QList<NBFileIO *> jobList;
		NBIOManager *ioManager;

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
