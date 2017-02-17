/*
	*
	* NBErrorsDialog.hpp - NBErrorsDialog.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDialog.hpp"
#include "NBGuiWidgets.hpp"
#include "NBIconManager.hpp"
#include "NBButtons.hpp"

class NBErrorsDialog : public NBDialog {
	Q_OBJECT

	public:
		NBErrorsDialog( QString title, QString text, QStringList, QWidget *parent = 0 );

	private:
		QLabel *iconLbl, *textLbl;
		NBSegmentButton *segBtns;
		NBButton *okayBtn;
		QTableWidget *table;

		QStringList nodeList;

		QString mTitle, mText;

		void setupGUI();
		void setupTable();
		void addEntry( QString );

	protected:
		void keyPressEvent( QKeyEvent *keyEvent );

	public slots:
		void close();
		int exec();

	signals:
		void closed();
};
