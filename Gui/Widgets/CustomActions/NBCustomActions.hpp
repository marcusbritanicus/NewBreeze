/*
	*
	* NBCustomActions.hpp - NBCustomActions.cpp header
	*
*/

#pragma once
#ifndef NBCUSTOMACTIONS_HPP
#define NBCUSTOMACTIONS_HPP

#include <Global.hpp>
#include <NBDialog.hpp>
#include <NBButton.hpp>
#include <NBTools.hpp>

class NBCustomActions : public NBDialog {
	Q_OBJECT

	public:
		NBCustomActions( QWidget *parent = 0 );
		QString currentAction();

	private:
		void createGUI();
		void setupTable();
		void setWidgetProperties();

		QTableWidget *actionsTable;
		NBButton *addBtn, *delBtn, *quitBtn;

		bool editing;

	private slots:
		void addCustomAction();
		void editCustomAction( int, int );
		void removeCustomActions();

		void updateButtons();
};

class NBNewAction : public NBDialog {
	Q_OBJECT

	public:
		NBNewAction( QWidget *parent = 0 );

		QString actionName();
		QString actionCommand();
		QString actionFilters();

		void setActionName( QString );
		void setActionCommand( QString );
		void setActionFilters( QString );

	private:
		NBLineEdit *actionNameLE, *actionCmdLE, *actionGlobLE;
		NBButton *addBtn, *cancelBtn;
		bool addAction;
};

#endif
