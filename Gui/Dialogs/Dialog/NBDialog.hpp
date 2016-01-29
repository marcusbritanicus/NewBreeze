/*
	*
	* NBDialog.hpp - NBDialog.cpp header
	*
*/

#pragma once
#ifndef NBDIALOG_HPP
#define NBDIALOG_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>

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

		QFrame *BodyFrame;
};

#endif
