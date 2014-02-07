/*
	*
	* NBApplicationItem.hpp - NewBreeze NBApplicationItem Class Header
	*
*/

#pragma once
#ifndef NBAPPLICATIONITEM_HPP
#define NBAPPLICATIONITEM_HPP

#include <Global.hpp>

class NBApplicationItem {
	public:
		NBApplicationItem();
		NBApplicationItem( QVariantList, QString, NBApplicationItem *parent = 0 );

		int childCount();
		int categoryCount();

		void clearChildren();

		void addChild( NBApplicationItem* );
		void removeChild( NBApplicationItem* );

		NBApplicationItem* child( int );
		NBApplicationItem* child( QString );
		QList<NBApplicationItem*> children();

		QVariant data( int column ) const;

		QString name();

		QIcon icon();
		void setIcon( QIcon );

		QString category();
		int categoryIndex();

		QString toolTip();

		NBApplicationItem *parent();
		int row();

		void sort();

		bool operator<( NBApplicationItem *other );

		static QStringList categoryList;

	private:
		NBApplicationItem *parentItem;
		QVariantList itemData;

		QList<NBApplicationItem*> myChildren;

		QString myCategory;
		QIcon myIcon;
};

#endif
