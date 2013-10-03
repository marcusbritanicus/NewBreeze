/*
	*
	* NBFolderView.hpp - NewBreeze FolderView Class Header
	*
*/

#ifndef NBFVDialogs_HPP
#define NBFVDialogs_HPP

#include <QtGui>
#include <NBDialog.hpp>

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
		QLineEdit *le;
		QPushButton *okBtn, *cancelBtn;
		bool runOk;

	private:
		void setWindowProperties();

	private slots :
		void runCommand();
		void cancel();
		void handleTextChanged( QString newText );
};

#endif
