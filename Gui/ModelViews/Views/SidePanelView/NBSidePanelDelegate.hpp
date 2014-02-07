/*
	*
	* NBSidePanelDelegate.hpp - NBSidePanelDelegate.cpp Header
	*
*/

#pragma once
#ifndef NBSidePanelDelegate_HPP
#define NBSidePanelDelegate_HPP

#include <Global.hpp>

class NBSidePanelDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
};

#endif
