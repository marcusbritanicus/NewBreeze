/*
	*
	* NBContextMenu.hpp - NBContextMenu.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBGuiWidgets.hpp"

class NBActionsMenu : public QMenu {
	Q_OBJECT

	public:
		NBActionsMenu( QList<QModelIndex>, QString, QWidget * );
		QStringList command();

	private:
		void buildDefaultActions();
		void buildPluginsActions();
		void buildCustomActionsMenu();

		QList<QModelIndex> selection;
		QString workingDir;

	private slots:
		void extract();
		void compress();
		void showCustomActionsDialog();
		void doCustomAction();

		void encrypt();
		void decrypt();

	Q_SIGNALS:
		void extractArchive( QString );
		void addToArchive( QStringList );
};

class NBOpenWithMenu : public QMenu {
	Q_OBJECT

	public:
		NBOpenWithMenu( QString, QString, QWidget* );
		void setWorkingDirectory( QString );
		void buildMenu( QList<QModelIndex> );

	private:
		QString workingDir;
};

class NBAddToCatalogMenu : public QMenu {
	Q_OBJECT

	public:
		NBAddToCatalogMenu( QString, QModelIndexList, QWidget * );

	private:
		QString workNode;
		QModelIndexList sNodes;

	private slots:
		void addToCatalog();
		void addToNewCatalog();

	Q_SIGNALS:
		void reloadCatalogs();
};

// class NBSuperStartMenu : public QMenu {
	// Q_OBJECT

	// public:
		// NBSuperStartMenu();
// };
