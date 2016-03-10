/*
	*
	* NBCatalogItem.hpp - NewBreeze NBCatalogItem Class Header
	*
*/

#pragma once
#ifndef NBCATALOGITEM_HPP
#define NBCATALOGITEM_HPP

#include <Global.hpp>

class NBCatalogItem {
	public:
		NBCatalogItem();
		NBCatalogItem( QVariantList, QString, NBCatalogItem *parent = 0 );

		int childCount();
		int catalogCount();

		void clearChildren();

		void addChild( NBCatalogItem* );
		void removeChild( NBCatalogItem* );

		NBCatalogItem* child( int );
		NBCatalogItem* child( QString );
		QList<NBCatalogItem*> children();

		QVariant data( int column ) const;

		QString name();

		QIcon icon();
		void setIcon( QIcon );

		QString catalogName();
		int catalogIndex();

		QString toolTip();

		NBCatalogItem *parent();
		int row();

		void sort();

		bool operator<( NBCatalogItem *other );

		static QStringList catalogList;

	private:
		NBCatalogItem *parentItem;
		QVariantList itemData;

		QList<NBCatalogItem*> myChildren;

		QString myCatalogName;
		QIcon myIcon;
};

#endif
