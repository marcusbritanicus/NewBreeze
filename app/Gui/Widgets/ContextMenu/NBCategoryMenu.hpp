/*
	*
	* NBCategoryMenu.hpp - NBCategoryMenu.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBGuiWidgets.hpp"

class NBCategoryMenu : public QMenu {
	Q_OBJECT

	public:
		NBCategoryMenu( QString catName, QPixmap icon, QString wNode, QModelIndexList nodeList, QWidget *parent );

	private:
		QString mCategory;
		QString mWorkNode;
		QModelIndexList sNodes;

		void populateMenu();

		QAction *showAct, *foldAct, *hideAct;
		QAction *selAct, *uselAct;
		QAction *propAct;

	private Q_SLOTS:
		void showCategory();
		void foldCategory();
		void hideCategory();

		void manageSelection();
		void showProperties();

	Q_SIGNALS:
		void showCategory( QString );
		void foldCategory( QString );
		void hideCategory( QString );

		void selection( QString, bool );
};
