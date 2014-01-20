/*
	*
	* NBTrashDelegate.hpp - NBTrashDelegate.cpp Header
	*
*/

#ifndef NBTRASHDELEGATE_HPP
#define NBTRASHDELEGATE_HPP

#include <Global.hpp>

class NBTrashDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	private:
		void paintExtraDetails( QPainter *painter, QList<QRect> &textRectList, const QModelIndex &index ) const;

		mutable int maxLines = 1;		// Max number of lines that can be written
		mutable int textLines = 1;		// Number of lines the filename occupies
};

#endif
