/*
	*
	* NBCircle.cpp - Circle class
	*
*/

#include <NBCircle.hpp>

Circle::Circle( QPoint center, qint64 radius ) {

	m_center = center;
	m_radius = radius;
};

QPoint Circle::center() {

	return m_center;
};


qint64 Circle::radius() {

	return m_radius;
};

bool Circle::contains( QPoint point ) {

	qint64 d2 = pow( point.x() - m_center.x(), 2 ) + pow( point.y() - m_center.y(), 2 );

	return ( d2 <= m_radius * m_radius );
};

bool Circle::contains( int x, int y ) {

	return contains( QPoint( x, y ) );
};

bool Circle::intersects( Circle circle2 ) {

	QPoint c1 = m_center;
	QPoint c2 = circle2.center();

	qint64 dsqr = pow( c1.x() - c2.x(), 2 ) + pow( c1.y() - c2.y(), 2 );

	return dsqr <= pow( m_radius + circle2.radius(), 2 );
};
