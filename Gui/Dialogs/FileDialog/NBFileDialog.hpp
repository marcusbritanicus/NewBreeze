/*
	*
	* NBFileDialog.hpp - NBFileDialog.cpp Header
	*
*/

#ifndef NBFILEDIALOG_HPP
#define NBFILEDIALOG_HPP

#include <Global.hpp>
#include <NBAddressBar.hpp>
#include <NBFDFolderView.hpp>
#include <NBSidePanel.hpp>
#include <NBDialog.hpp>

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

		NBFileDialog( QString icon, QString title, QString location = QDir::homePath(), FileType type = NBFileDialog::ExistingFile );
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

		NBAddressBar *toolBar;
		NBFDFolderView *mainView;
		NBSidePanel *sidePanel;

		QLineEdit *nameLE;
		QComboBox *filtersCB;

		QStringList filters;

		QString icon;
		QString title;
		QString location;
		FileType type;

	private slots:
		void openAddressBar();

		void resetFilters();
		void updateToolBar();
};

#endif
