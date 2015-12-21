/*
	*
	* NBProcessManagerGUI.hpp - NBProcessManagerGUI.cpp header
	*
*/

#pragma once
#ifndef NBProcessManagerGUI_HPP
#define NBProcessManagerGUI_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBDialog.hpp>
#include <NBCoreProcess.hpp>
#include <NBProgressBar.hpp>
#include <NBIconProvider.hpp>
#include <NBMessageDialog.hpp>
#include <NBStyleManager.hpp>

class NBProcWidget : public QWidget {
	Q_OBJECT

	public:
		NBProcWidget( NBCoreProcess *proc );

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

class NBProcessManagerGUI : public NBDialog {
	Q_OBJECT

	public:
		NBProcessManagerGUI( QWidget *parent = 0 );
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
};

class NBProcessManagerGUIMini : public QToolButton {
	Q_OBJECT

	public:
		// Init all variables
		NBProcessManagerGUIMini( QWidget *parent );

		// Destroy all variables
		~NBProcessManagerGUIMini();

		// addJob( sourceList, target, IOMode, JobMode );
		void addJob( QStringList, QString, NBIOMode::Mode );

		// activeJobs();
		quint64 activeJobs();

		// showAllIODialogs();
		void showAllIODialogs();

	private:
		QList<NBFileIO *> jobList;
		NBProcessManagerGUI *ioManager;

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

#endif
