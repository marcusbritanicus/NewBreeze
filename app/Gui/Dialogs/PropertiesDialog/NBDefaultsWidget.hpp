/*
	*
	* NBDefaultsWidget.hpp - NBDefaultsWidget.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDialog.hpp"
#include "NBButton.hpp"
#include "NBButtons.hpp"
//#include "NBGuiWidgets.hpp"
#include "NBOpenWithView.hpp"

class NBDefaultsWidget : public QWidget {
	Q_OBJECT

	public:
		NBDefaultsWidget( QStringList paths, QWidget* );

	private:
		QComboBox *defaultCB;
		QListWidget *appList;

		QMimeType mimeType;

	private Q_SLOTS:
		void addApplication();
		void removeApplication();
		void makeDefault( int );
};

class NBAppSelector : public NBDialog {
	Q_OBJECT

	public :
		NBAppSelector( QWidget *parent );
		QString selectedApp();

	private:
		void createGUI();
		void setWindowProperties();

		NBButton *okBtn, *cancelBtn;
		NBOpenWithView *appList;

		QString mSelectedApp;

	private slots :
		void appSelected( QModelIndex );
};
