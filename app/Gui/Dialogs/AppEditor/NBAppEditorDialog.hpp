/*
	*
	* NBAppEditorDialog.hpp - AppEditor class header for NewBreeze
	*
*/

#pragma once

#include "Global.hpp"

typedef QMap<QString, QStringList> MDict;

class NBDesktopFileEditor : public QDialog {
	Q_OBJECT

	public:
		NBDesktopFileEditor( QWidget*, NBAppFile ) {};

	private:
		void createGUI();

		QLineEdit *nameLE, *descrLE, *cmtLE, *execLE;
		QToolButton *iconBtn;
};

class NBMimeTypeSelector : public QDialog {
	Q_OBJECT

	public :
		NBMimeTypeSelector( QWidget *parent = 0  );
		QStringList selectedItems();

		void setChecked( QString, bool );
		bool isChecked( QString );

	private :
		void populateGUI();

		QTreeWidget *mimeTree;
		MDict mdict;

		QStringList checked;

	public slots :
		void changeSelection( QTreeWidgetItem*, int );
};
