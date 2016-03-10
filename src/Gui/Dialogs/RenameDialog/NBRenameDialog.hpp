/*
	*
	* NBRenameDialog.hpp - NBRenameDialog.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBDialog.hpp>
#include <NBButtons.hpp>
#include <NBTools.hpp>

class NBRenameDialog : public NBDialog {
	Q_OBJECT

	public :
		NBRenameDialog( QString origName, QDir curDir, QWidget *parent = 0 );
		QString newName();
		bool canRename();

	private:
		void createGUI();

		NBSegmentButton *segBtns;

		NBLineEdit *le;

		QCheckBox *replaceCB;
		QString name;
		QDir dir;
		bool renameOk;

	private slots :
		void handleSegmentClick( int );
		void rename();
		void cancel();
		void handleTextChanged( QString newText );
		void handleCBStateChanged();
};
