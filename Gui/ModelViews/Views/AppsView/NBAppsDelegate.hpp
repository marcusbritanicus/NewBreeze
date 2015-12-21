/*
	*
	* NBIconDelegate.hpp - NBIconDelegate.cpp Header
	*
*/

#pragma once
#ifndef NBAPPSDELEGATE_HPP
#define NBAPPSDELEGATE_HPP

#include <Global.hpp>

class NBAppsDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
};

#endif
