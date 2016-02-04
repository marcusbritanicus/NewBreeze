/*
	*
	* AboutNB.hpp - AboutNB.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBDialog.hpp>
#include <NBButton.hpp>
#include <NBGuiWidgets.hpp>

class AboutNB : public QDialog {
	Q_OBJECT

	public:
		AboutNB( QWidget *parent = NULL );

		void setDisplayTab( int );

	private:
		void createGui();
		void setWindowProperties();

		int mDisplayTab;
};
