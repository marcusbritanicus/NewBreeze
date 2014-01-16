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
		// QSize sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const;

	private:
		void paintTiles( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		void paintIcons( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		void paintDetails( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

		void paintIconTextDetails( QPainter *painter, QRect &textRect, const QModelIndex &index ) const;
		void paintTileTextDetails( QPainter *painter, QRect &textRect, const QModelIndex &index ) const;
		void paintExtraDetails( QPainter *painter, QList<QRect> &textRectList, const QModelIndex &index ) const;

		mutable int maxLines = 1;		// Max number of lines that can be written
		mutable int textLines = 1;		// Number of lines the filename occupies
};

#endif
