/*
	*
	* NBFDItemDelegate.hpp - NewBreeze NBFDItemDelegate.cpp Header
	*
*/

#ifndef NBFDFILESYSTEMMODEL_HPP
#define NBFDFILESYSTEMMODEL_HPP

#include <Global.hpp>

class NBFDItemDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		QSize sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const;
};

#endif
