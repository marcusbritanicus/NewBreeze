/*
	*
	* NBProcessManagerGUI.hpp - NBProcessManagerGUI.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBIconProvider.hpp>
#include <NBProcessManager.hpp>
#include <NBProgressBar.hpp>
#include <NBMessageDialog.hpp>

class NBProcessWidget : public QWidget {
	Q_OBJECT

	public:
		NBProcessWidget( NBProcess::Progress *progress, NBAbstractProcess *proc, QWidget *parent );

	private:
		/* Create a GUI */
		void createGUI();

		/* Icon for the process */
		QLabel *iconLbl;

		/* Copying/Moving/Permissions/Deleting etc */
		QLabel *titleLbl;

		/* Source and target */
		QLabel *srcLbl, *tgtLbl;

		/* Speed and ETA */
		QLabel *speedLbl, *etcLbl;

		/* Current file */
		QLabel *cFileLbl;

		/* ProgressBar */
		NBProgressBar *totalPB, *cfilePB;

		/* Buttons */
		QToolButton *pauseBtn, *detailsBtn;

		/* Refresh timer */
		QBasicTimer updateTimer;

		/* Time spend on puase */
		QTime pauseTime;
		quint64 pausedSecs;

		/* Toggle details flag */
		bool detailsShown;

		NBProcess::Progress *mProgress;
		NBAbstractProcess *mProcess;

	private slots:
		void toggleDetails();
		void togglePauseResume();

	protected:
		void timerEvent( QTimerEvent* );
		void paintEvent( QPaintEvent* );
};
