/*
	*
	* NBIOManager.hpp - NBIOManager.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBDialog.hpp>
#include <NBFileIO.hpp>
#include <NBProgressBar.hpp>
#include <NBIconProvider.hpp>
#include <NBMessageDialog.hpp>
#include <NBErrorsDialog.hpp>

class NBIOWidget : public QWidget {
	Q_OBJECT

	public:
		NBIOWidget( NBFileIO *io );

	private:
		QLabel *ttlLbl, *srcLbl, *tgtLbl, *speedLbl, *etcLbl, *cfileLbl;
		NBClickLabel *toggleDetailsLbl, *togglePauseResumeLbl, *closeLbl;
		NBProgressBar *totalBar, *cfileBar;

		QTimer *timer;
		QTimer *speedTimer;
		bool detailsAreSeen;

		NBFileIO *io;
		bool paused;

		quint64 previousSize, currentSize;

	private slots:
		void toggleDetails();
		void togglePauseResume();
		void cancelIO();
		void update();
		void speedCalculator();
		void signalRemove();

	protected:
		void paintEvent( QPaintEvent *pEvent );

	Q_SIGNALS:
		void removeIO( NBFileIO* );
};

class NBIOManager : public NBDialog {
	Q_OBJECT

	public:
		NBIOManager( QList<NBFileIO*>, QWidget *parent = 0 );
		void addIO( NBFileIO * );
		void showCritical();

	private:
		QList<NBFileIO*> ioList;
		QVBoxLayout *baseLyt;

		bool killIOOnClose;

	private slots:
		void removeIO( NBFileIO* );

	protected:
		void closeEvent( QCloseEvent* );

	Q_SIGNALS:
		void closed();
};

class NBIOManagerMini : public QToolButton {
	Q_OBJECT

	public:
		// Init all variables
		NBIOManagerMini( QWidget *parent );

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
