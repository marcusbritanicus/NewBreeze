/*
	*
	* NBContextMenu.hpp - NBContextMenu.cpp header
	*
*/

#ifndef NBCONTEXTMENU_HPP
#define NBCONTEXTMENU_HPP

#include <Global.hpp>
#include <NBAppEngine.hpp>
#include <NBAppFile.hpp>
#include <NBTools.hpp>

class NBCustomActionsMenu : public QMenu {
	Q_OBJECT

	public:
		NBCustomActionsMenu( QList<QModelIndex>, QString );
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
		NBAddToCatalogMenu( QString, QModelIndexList );

	private:
		QString workNode;
		QModelIndexList sNodes;

	private slots:
		void addToCatalog();

	Q_SIGNALS:
		void reloadCatalogs();
};

#endif
