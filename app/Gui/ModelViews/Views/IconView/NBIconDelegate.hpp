/*
	*
	* NBIconDelegate.hpp - NBIconDelegate.cpp Header
	*
*/

#pragma once

#include "Global.hpp"

class NBIconDelegate : public QItemDelegate {
    Q_OBJECT

	public:
		void paintIcons( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		void paintTiles( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
		void paintDetails( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	private:
		void paintIconTextDetails( QPainter *painter, QRect &textRect, const QModelIndex &index ) const;
		void paintTileTextDetails( QPainter *painter, QRect &textRect, const QModelIndex &index ) const;
		void paintExtraDetails( QPainter *painter, QList<QRect> &textRectList, const QModelIndex &index ) const;

		mutable int maxLines;		// Max number of lines that can be written
		mutable int textLines;		// Number of lines the filename occupies
};
