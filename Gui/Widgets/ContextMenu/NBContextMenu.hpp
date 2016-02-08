/*
	*
	* NBContextMenu.hpp - NBContextMenu.cpp header
	*
*/

#pragma once
#ifndef NBCONTEXTMENU_HPP
#define NBCONTEXTMENU_HPP

#include <Global.hpp>
#include <NBAppEngine.hpp>
#include <NBAppFile.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>

class NBCustomActionsMenu : public QMenu {
	Q_OBJECT

	public:
		NBCustomActionsMenu( QList<QModelIndex>, QString, QWidget * );
		QStringList command();

	private:
		void buildCustomActionsMenu();
		QList<QModelIndex> selection;
		QString workingDir;

	private slots:
		void extract();
		void compress();
		void showCustomActionsDialog();
		void doCustomAction();

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

#endif
