/*
	*
	* NBSTerminalWidget.hpp - NBSTerminalWidget.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDialog.hpp"
#include "NBButtons.hpp"
#include "NBButton.hpp"

class NBSTerminalWidget : public QWidget {
	Q_OBJECT

	public:
		NBSTerminalWidget( QWidget* );

	private:
		/* Terminal */
		QGroupBox *termGB;
		QListWidget *termsLW;					// List of terminals detected/added
		QPushButton *addBtn;					// Add a terminal
		QPushButton *defaultBtn;				// Make this terminal default

	private slots:
		/* Terminal */
		void handleDefaultClicked();
		void handleAddTermClicked();
};

class NBAddVTEDialog : public NBDialog {
	Q_OBJECT

	public :
		NBAddVTEDialog( QWidget *parent );

	private:
		NBSegmentButton *btns;
		QCheckBox *wDirCB;
		QLineEdit *execLE, *workLE, *cmdLE;

	private Q_SLOTS:
		void addTerminal();
		void handleCBPressed();
};
