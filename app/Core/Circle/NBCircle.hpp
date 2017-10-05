/*
	*
	* NBCircle.hpp - Circle class header
	*
*/

#pragma once

#include <QtCore>
#include <cmath>

class Circle {

	public:
		/* Initialization */
		Circle( QPoint, qint64 );

		/* Center */
		QPoint center();

		/* Radius */
		qint64 radius();

		/* Is the point contained in this circle */
		bool contains( QPoint );
		bool contains( int, int );

		/* Does this circle intersect the other */
		bool intersects( Circle );

	private:
		QPoint m_center;
		qint64 m_radius;
};
