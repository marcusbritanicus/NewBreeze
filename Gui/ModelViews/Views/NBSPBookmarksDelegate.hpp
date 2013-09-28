/*
	*
	* NBItemDelegate.hpp - NewBreeze NBItemDelegate.cpp Header
	*
*/

#ifndef NBSPBOOKMARKSDELEGATE_HPP
#define NBSPBOOKMARKSDELEGATE_HPP

#include <Global.hpp>

class NBSPBookmarksDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		QSize sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const;
};

#endif
