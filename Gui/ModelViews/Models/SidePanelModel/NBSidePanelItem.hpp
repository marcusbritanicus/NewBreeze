/*
	*
	* NBNBSidePanelItem.hpp - NewBreeze NBSidePanelItem Class Header
	*
*/

#ifndef NBSIDEPANELITEM_HPP
#define NBSIDEPANELITEM_HPP

#include <Global.hpp>

class NBSidePanelItem {
	public:
		NBSidePanelItem();
		NBSidePanelItem( QVariantList, NBSidePanelItem *parent = 0 );

		int childCount();
		int columnCount();

		void clearChildren();

		void appendChild( NBSidePanelItem* );
		void insertChildAt( int, NBSidePanelItem* );
		void removeChildAt( int );

		NBSidePanelItem* child( int );
		NBSidePanelItem* child( QString );

		QVariant data( int column ) const;

		NBSidePanelItem *parent();
		int row();

	private:
		QList<NBSidePanelItem*> childNodes;
		QVariantList nodeData;
		NBSidePanelItem *parentNode;
};

#endif
