/*
	*
	* NBProcessManagerGUI.hpp - NBProcessManagerGUI.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBIconProvider.hpp>
#include <NBCoreProcess.hpp>
#include <NBProgressBar.hpp>
#include <NBMessageDialog.hpp>
#include <NBStyleManager.hpp>

class NBProcessWidget : public QWidget {
	Q_OBJECT

	public:
		NBProcessWidget( NBProcess::Progress *progress, QWidget *parent );

	private:
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
		QToolButton *pauseBtn, *detailsBtn, *closeBtn;

		/* Toggle details, pause */
		bool detailsShown;
		bool paused;

		NBProcess::Progress *mProgress;

	private slots:
		void toggleDetails();
		void togglePauseResume();
		// void close();
};
