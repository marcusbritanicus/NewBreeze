/*
	*
	* NBCustomActions.hpp - NBCustomActions.cpp header
	*
*/

#ifndef NBCUSTOMACTIONS_HPP
#define NBCUSTOMACTIONS_HPP

#include <Global.hpp>
#include <NBGuiWidgets.hpp>

class NBCustomActions : public QDialog {
	Q_OBJECT

	public:
		NBCustomActions();
		QString currentAction();

	private:
		void createGUI();
		void setupTable();
		void setWidgetProperties();

		QTableWidget *actionsTable;
		NBToolButton *addBtn, *delBtn;
		QPushButton *quitBtn;

		bool editing;

	private slots:
		void addCustomAction();
		void editCustomAction( int, int );
		void removeCustomActions();

		void updateButtons();
};

class NBNewAction : public QDialog {
	Q_OBJECT

	public:
		NBNewAction();

		QString actionName();
		QString actionCommand();
		QString actionFilters();

		void setActionName( QString );
		void setActionCommand( QString );
		void setActionFilters( QString );

	private:
		QLineEdit *actionNameLE, *actionCmdLE, *actionGlobLE;
		NBToolButton *addBtn, *cancelBtn;
		bool addAction;
};

#endif
