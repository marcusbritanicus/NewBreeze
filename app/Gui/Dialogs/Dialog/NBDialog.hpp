/*
	*
	* NBDialog.hpp - NBDialog.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBGuiWidgets.hpp"

class NBDialog : public QDialog {
	Q_OBJECT

	public:
		NBDialog( QWidget *parent = 0 );

		void setLayout( QLayout* );
		void setDialogTitle( QString );
		void setDialogIcon( QIcon );

	private:
		void setupGUI();
		void setupActions();
		void setupDialogProperties();

	protected:
		void closeEvent( QCloseEvent* );

	Q_SIGNALS:
		void closed();
};
