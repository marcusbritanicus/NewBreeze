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

class NBProcWidget : public QWidget {
	Q_OBJECT

	public:
		NBProcWidget( QWidget *parent );

		void update( NBProgress progress );

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

	private slots:
		void toggleDetails();
		void togglePauseResume();
		// void close();
};
