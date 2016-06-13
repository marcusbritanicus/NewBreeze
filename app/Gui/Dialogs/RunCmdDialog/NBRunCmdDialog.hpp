/*
	*
	* NBRunCmdDialog.hpp - NBRunCmdDialog.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDialog.hpp"
#include "NBButton.hpp"
#include "NBButtons.hpp"
#include "NBGuiWidgets.hpp"
#include "NBOpenWithView.hpp"

class NBRunCmdDialog : public NBDialog {
	Q_OBJECT

	public :
		NBRunCmdDialog( QString, QWidget *parent = 0 );
		QString commandString();

		bool canRun();

	private:
		void createGUI();
		void createAndSetupActions();
		void setWindowProperties();

		QString execCmd;
		NBLineEdit *le;
		NBButton *okBtn, *cancelBtn, *defaultAndRunBtn;
		NBOpenWithView *appList;
		QString fileName;
		bool runOk;

	private slots :
		void runCommand();
		void runCommand( QModelIndex );
		void cancel();
		void appSelected( QModelIndex );
		void handleTextChanged( QString newText );
		void makeDefaultAndRunCommand();
};
