/*
	*
	* NBIconDelegate.hpp - NBIconDelegate.cpp Header
	*
*/

#ifndef NBICONDELEGATE_HPP
#define NBICONDELEGATE_HPP

#include <Global.hpp>

class NBIconDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		QSize sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const;
};

#endif
