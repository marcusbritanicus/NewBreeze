/*
	*
	* NBIconDelegate.hpp - NBIconDelegate.cpp Header
	*
*/

#ifndef NBAPPSDELEGATE_HPP
#define NBAPPSDELEGATE_HPP

#include <Global.hpp>

class NBAppsDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		QSize sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const;
};

#endif
