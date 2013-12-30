/*
	*
	* NBFVDialogs.hpp - Various dialogs for NewBReeze
	*
*/

#ifndef NBFVDIALOGS_HPP
#define NBFVDIALOGS_HPP

#include <QtGui>
#include <NBDialog.hpp>
#include <NBOpenWithView.hpp>

class NBRenameDialog : public NBDialog {
	Q_OBJECT

	public :
		NBRenameDialog( QString origName, QDir curDir );
		QLineEdit *le;
		QPushButton *okBtn, *cancelBtn;
		QCheckBox *replaceCB;
		QString name;
		QDir dir;
		bool renameOk;

	private:
		void createGUI();
		void setWindowProperties();

		QLabel *lbl;

	private slots :
		void rename();
		void cancel();
		void handleTextChanged( QString newText );
		void handleCBStateChanged();
};

class NBNewFileFolderDialog : public NBDialog {
	Q_OBJECT

	enum{ File, Directory };

	public :
		NBNewFileFolderDialog( QString, QDir, QString text = QString() );
		QLineEdit *le;
		QPushButton *okBtn, *cancelBtn;
		QString name;
		QDir dir;
		QString data;

	private:
		void setWindowProperties();

	private slots :
		void createFileFolder();
		void cancel();
		void handleTextChanged( QString newText );

	Q_SIGNALS:
		void nodeCreated( QString );
};

class NBRunCmdDialog : public NBDialog {
	Q_OBJECT

	public :
		NBRunCmdDialog( QString );
		QString commandString();

		bool canRun();

	private:
		void createGUI();
		void createAndSetupActions();
		void setWindowProperties();

		QString execCmd;
		QLineEdit *le;
		QPushButton *okBtn, *cancelBtn, *defaultAndRunBtn;
		NBOpenWithView *appList;
		QString fileName;
		bool runOk;

	private slots :
		void runCommand();
		void runCommand( QModelIndex );
		void cancel();
		void appSelected( QModelIndex );
		// void appSelected();
		void handleTextChanged( QString newText );
		void makeDefaultAndRunCommand();
};

#endif
