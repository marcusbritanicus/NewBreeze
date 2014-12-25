/*
	*
	* NBSidePanel.hpp - NBSidePanel.cpp header
	*
*/

#pragma once
#ifndef NBSIDEPANEL_HPP
#define NBSIDEPANEL_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBSidePanelModel.hpp>
#include <NBFlashLabel.hpp>
#include <NBGuiWidgets.hpp>
#include <NBSidePanelView.hpp>

class NBSidePanel : public QWidget {
	Q_OBJECT

	public :
		NBSidePanel( QWidget *parent );

		void expand();
		void contract();

	private :
		void setupPanel();

		QWidget *vSpacer;
		NBSidePanelView *spView;
		NBFlashLabel *dirLbl, *appLbl, *ctlLbl, *devLbl, *bmkLbl;
		NBDeleteLabel *deleteLbl;

		bool forcedOpen;
		bool forcedOpenOnMenuShow;

		bool showingDevices;
		bool showingBookMarks;
		bool animating;

		QTimer *expandingTimer;
		QTimer *contractingTimer;

		int maxWidth;

	public slots :
		void flashBookmarks();
		void flashDevices();

		void flashFolders();
		void flashCatalogs();
		void flashApplications();

		void updateBookmarks();
		void updateDevices();

	private slots:
		void increaseWidth();
		void decreaseWidth();

		void toggleExpandContract();

		void stayOpenOnMenuShow();
		void restoreExpansionState();

	protected :
		bool event( QEvent* );

		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent *dmEvent );
		void dragLeaveEvent( QDragLeaveEvent* );

		void enterEvent( QEvent *eEvent );
		void leaveEvent( QEvent *lEvent );

		void paintEvent( QPaintEvent *pEvent );

	Q_SIGNALS :
		void showFolders();
		void showApplications();
		void showCatalogs();

		void showTrash();

		void driveClicked( QString );

		void copy( QStringList, QString, NBIOMode::Mode );
		void move( QStringList, QString, NBIOMode::Mode );
};

#endif
