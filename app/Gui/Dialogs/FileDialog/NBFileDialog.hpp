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

class NBFileDialog : public NBDialog {
	Q_OBJECT

	public:
		enum DialogType {
			Icon					= 0xA01B73,
			File,
			Directory
		};

		void setNameFilters( QStringList, QString );
		void clearNameFilter();

		QString selectedItem();
		QStringList selectedItems();

		QString selectedFilter();

		static QString getFileName( QWidget*, QString, QString, QStringList, QString );
		static QString getDirectoryName( QWidget*, QString, QString );

	private:
		NBFileDialog( QWidget *parent, QString title, QString location, DialogType type );

		void createGUI();
		void createAndSetupActions();
		void setWindowProperties();

		NBItemViewModel *fsModel;

		NBAddressWidget *addressBar;
		NBSideBar *sidePanel;

		NBIconView *mainView;
		NBDirectoryView *dirView;

		NBLineEdit *nameLE;
		QComboBox *filtersCB;

		QStringList filters;

		QString title;
		QString location;
		DialogType type;

	private slots:
		void openAddressBar();
		void open( QModelIndex );
		void open( QString );

		void resetFilters();
		void updateToolBar();
};
