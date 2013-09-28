/*
	*
	* NBItemDelegate.hpp - NewBreeze NBItemDelegate.cpp Header
	*
*/

#ifndef NBSPDEVICEDELEGATE_HPP
#define NBSPDEVICEDELEGATE_HPP

#include <Global.hpp>

class NBSPDeviceDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		QSize sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const;
};

#endif
