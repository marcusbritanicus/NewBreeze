/*
	*
	* NBAppEditorDialog.hpp - AppEditor class header for NewBreeze
	*
*/

#pragma once

#include "Global.hpp"

typedef QMap<QString, QStringList> MDict;

class NBMimeTypeSelector : public QTreeWidget {
	Q_OBJECT

	public :
		NBMimeTypeSelector( QWidget *parent = 0  );
		QStringList selectedItems();

		void setChecked( QString, bool );
		bool isChecked( QString );

	private :
		void populateTree();

		MDict mdict;
		QStringList checked;

	public slots :
		void changeSelection( QTreeWidgetItem*, int );
};

class NBAppIconButton : public QToolButton {
	Q_OBJECT

	public:
		NBAppIconButton( QWidget *parent );

		void setIcon( QString );
		QString iconPath();

	private:
		QString mIconFile;

	private Q_SLOTS:
		void selectIcon();
};

class NBDesktopFileEditor : public QDialog {
	Q_OBJECT

	public:
		NBDesktopFileEditor( QWidget*, NewBreeze::DesktopFile );

	private:
		void createGUI();
		void updateEntries();
		void setDialogProperties();

		QLineEdit *nameLE, *descrLE, *cmtLE, *execLE;
		NBAppIconButton *iconBtn;

		QLineEdit *wrkPthLE;
		QCheckBox *hiddenCB, *termCB;
		NBMimeTypeSelector *mimesTW;

		QPushButton *saveBtn, *cancelBtn;

		NewBreeze::DesktopFile mApp;

	public Q_SLOTS:
		void accept();
};
