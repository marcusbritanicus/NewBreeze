/*
	*
	* NBSelectionArea.hpp - NewBreeze Icon Viewer Class Header
	*
*/

#pragma once
#ifndef NBSELECTIONAREA_HPP
#define NBSELECTIONAREA_HPP

#include <Global.hpp>

class NBSelection {

	public:
		/* @rect is the selection rectangle to be drawn
		 * The first qint64 is the visual row of the index
		 * The second quit64 is the visual column of the index */
		NBSelection( QRect, qint64, qint64 );

		/* To access row and column */
		qint64 row, col;
		QRect rect;
};

class NBSelectionArea {

	public:
		NBSelectionArea( qint64, qint64 );
		void addRect( QRect, int, int );

		qint64 rows;
		qint64 cols;

		QPainterPath toPath();

		void operator<<( NBSelection other );

	private:
		QList<QList<QRect> > selection;
};

#endif
