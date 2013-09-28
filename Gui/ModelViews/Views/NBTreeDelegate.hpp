/*
	*
	* NBTreeDelegate.hpp - NBTreeDelegate.cpp Header
	*
*/

#ifndef NBTreeDelegate_HPP
#define NBTreeDelegate_HPP

#include <Global.hpp>

class NBTreeDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		QSize sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const;
};

#endif
