/*
	*
	* NBIconDelegate.hpp - NBIconDelegate.cpp Header
	*
*/

#pragma once
#ifndef NBOpenWithDelegate_HPP
#define NBOpenWithDelegate_HPP

#include <Global.hpp>

class NBOpenWithDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
};

#endif
