/*
	*
	* NBFileDialog.hpp - NBFileDialog.cpp Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBAddressBar.hpp"
#include "NBIconViewRestricted.hpp"
#include "NBSidePanel.hpp"
#include "NBDialog.hpp"
#include "NBMessageDialog.hpp"
#include "NBGuiWidgets.hpp"

class NBFileDialog : public NBDialog {
	Q_OBJECT

	public:
		enum FileType {
			ExistingFile,
			SaveFile,
			ExistingDirectory,
			SaveDirectory,
			ExistingFiles,
			ExistingDirectories
		};

		NBFileDialog( QString icon, QString title, QString location = QDir::homePath(), FileType type = NBFileDialog::ExistingFile, QWidget *parent = 0 );
		void setNameFilters( QStringList, QString );

		QString selectedItem();
		QStringList selectedItems();

		QString selectedFilter();

		static QString getExistingFileName( QString, QString, QString, QStringList, QString );
		static QString getSaveFileName( QString, QString, QString, QStringList, QString );
		static QString getExistingDirectory( QString, QString, QString );

	private:
		void createGUI();
		void createAndSetupActions();
		void setWindowProperties();

		NBItemViewModel *fsModel;

		NBAddressWidget *addressWidget;
		NBIconViewRestricted *mainView;
		NBSidePanel *sidePanel;

		NBLineEdit *nameLE;
		QComboBox *filtersCB;

		QStringList filters;

		QString icon;
		QString title;
		QString location;
		FileType type;

	private slots:
		void openAddressBar();
		void open( QModelIndex );
		void open( QString );

		void resetFilters();
		void updateToolBar();
};
