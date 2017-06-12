/*
	*
	* NBFileDialog.hpp - NBFileDialog.cpp Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBAddressBar.hpp"
#include "NBButtons.hpp"
#include "NBIconViewRestricted.hpp"
#include "NBDirectoryView.hpp"
#include "NBSideBar.hpp"
#include "NBDialog.hpp"
#include "NBMessageDialog.hpp"
#include "NBGuiWidgets.hpp"

class NBFileDialog : public QDialog {
	Q_OBJECT

	public:
		void setNameFilters( QStringList, QString );
		void clearNameFilter();

		QString selectedItem();
		QStringList selectedItems();

		QString selectedFilter();

		static QString getFileName( QWidget*, QString, QString, QStringList, QString );

	private:
		NBFileDialog( QWidget *parent, QString title, QString location );

		void createGUI();
		void createAndSetupActions();
		void setWindowProperties();

		NBItemViewModel *fsModel;

		NBAddressWidget *addressBar;
		NBSideBar *sidePanel;

		NBIconView *mainView;

		NBLineEdit *nameLE;
		QComboBox *filtersCB;

		QStringList filters;

		QString title;
		QString location;

	private slots:
		void openAddressBar();
		void open( QModelIndex );
		void open( QString );

		void resetFilters();
		void updateToolBar();
};

class NBDirectoryDialog : public NBDialog {
	Q_OBJECT

	public:
		static QString getDirectoryName( QWidget*, QString, QString );

		QString selectedItem();

	private:
		NBDirectoryDialog( QWidget *parent, QString title, QString location );

		void createGUI();
		void setWindowProperties();

		NBAddressWidget *addressBar;
		NBSideBar *sidePanel;

		NBDirectoryView *dirView;

		NBLineEdit *nameLE;
		QComboBox *filtersCB;

		QStringList filters;

		QString title;
		QString location;
};
