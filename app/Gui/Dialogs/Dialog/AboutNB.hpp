/*
	*
	* AboutNB.hpp - AboutNB.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDialog.hpp"
#include "NBButton.hpp"
#include "NBGuiWidgets.hpp"

class AboutNB : public QDialog {
	Q_OBJECT

	public:
		AboutNB( QWidget *parent = NULL );

	private:
		void createGui();
		void setWindowProperties();

		QWidget* createAboutNB();
		QWidget* createLicense();
		QWidget* createCredits();
};

class License : public QDialog {
	Q_OBJECT

	public:
		License( QWidget *parent = NULL );
};
