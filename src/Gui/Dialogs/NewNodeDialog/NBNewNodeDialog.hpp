/*
	*
	* NBNewNodeDialog.hpp - NBNewNodeDialog.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBDialog.hpp>
#include <NBButton.hpp>
#include <NBButtons.hpp>
#include <NBMessageDialog.hpp>
#include <NBTools.hpp>

class NBNewNodeDialog : public NBDialog {
	Q_OBJECT

	enum{ File, Directory };

	public :
		NBNewNodeDialog( QString, QDir, QString text = QString(), QWidget *parent = 0 );
		NBLineEdit *le;
		NBButton *okBtn, *cancelBtn;
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
