/*
	*
	* NBBugReporter.hpp - NBBugReporter.cpp Header Class
	*
*/
 #pragma once

 #include "Global.hpp"
 #include "NBDialog.hpp"
 #include "NBMessageDialog.hpp"

class NBBugReporter : public NBDialog {
	Q_OBJECT

	public:
		NBBugReporter();

	private:
		void createGUI();
		void setWindowProperties();

		QLineEdit *titleLE;
		QPlainTextEdit *issueTE;

		QLabel *minTtlTxtLbl, *minIssTxtLbl;

		QPushButton *cancelBtn, *sendBtn;

	public Q_SLOTS:
		void sendReport();
		void enableSend();
};

class NBGitHubAuthDialog : public NBDialog {
	Q_OBJECT

	public:
		NBGitHubAuthDialog( QStringList data, QWidget *parent );

	private:
		QLineEdit *nameLE, *passLE;

		QStringList mData;

	public Q_SLOTS:
		void authenticate();
};
