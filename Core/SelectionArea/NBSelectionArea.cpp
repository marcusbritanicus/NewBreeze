/*
	*
	* NBSelectionArea.cpp - NewBreeze IconView Class
	*
*/

#include <NBSelectionArea.hpp>

NBSelection::NBSelection( QRect mrct, qint64 mrw, qint64 mcl ) {

	rect = mrct;
	row = mrw;
	col = mcl;
};

NBSelectionArea::NBSelectionArea( qint64 mrws, qint64 mcls ) {

	rows = mrws;
	cols = mcls;

	for( int r = 0; r < rows; r++ ) {
		QList<QRect> newRectList;
		for( int c = 0; c < cols; c++ )
			newRectList << QRect( 0, 0, 0, 0 );

		selection << newRectList;
	}
};

void NBSelectionArea::addRect( QRect rect, int r, int c ) {

	selection[ r ][ c ] = rect;
};

QPainterPath NBSelectionArea::toPath() {

	/*
		*
		* [ ] [x] [x] [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [x] [ ] [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [x] [x] [x] [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [x] [x] [x] [x] [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [x] [ ] [ ] [x] [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [x] [x] [x] [x] [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [x] [x] [x] [x] [x] [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [x] [ ] [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [x] [x] [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		* [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
		*
	*/

	QList<QRect> nodes;
	qint64 r = 0, c = 0;

	QPainterPath path;

	QPolygon poly;
	QRect first;

	/* Start */
	while ( true ) {
		if ( not selection[ r ][ c ].isEmpty() ) {
			first = selection[ r ][ c ];
			poly << first.topLeft() << first.topRight();
			break;
		}

		else {
			/* Increment the column */
			c++;

			/* If the column is geater than the visual columns, increment the row and reset column to 0 */
			if ( c >= cols ) {
				c = 0;
				r++;
			}

			/* If the last row increment results in over shooting the visual rows, it means we did not get our */
			if ( r >= rows ) {
				return QPainterPath();
			}
		}
	}

	/* Now that we have started, lets go right till be hit the bottom */
	while ( r < rows ) {
		/* Increment c, and make sure it stays below cols limit */
		c++;
		if ( c >= cols ) {
			c--;
			r++;

			/* While incrementing r, if we hit the bottom rightmost valid rectangle, we start going left and top */
			if ( r > rows ) {
				r--;
				break;
			}
		}

		/* Check right and if it is valid capture it and continue right */
		QRect nRect = selection[ r ][ c ];
		if ( not nRect.isEmpty() ) {
			poly << nRect.topLeft() << nRect.topRight();
			continue; 			// Goes back to c++
		}
		/* Since its not valid, we go down and we check right */
		else {
			down:				// A down going bock of code
			c--;				// We did a c++ and that [ r, c ] was empty, so we do a c--
			r++;

			/* While incrementing r, if we hit the bottom rightmost valid rectangle, we start going left and top */
			if ( r > rows ) {
				r--;
				break;
			}

			nRect = selection[ r ][ c ];
			if ( not nRect.isEmpty() ) {
				/* When we go down, we just take the top-tight corner. The top-left corner comes in later */
				poly << nRect.topRight();
				continue;		// Goes back to c++
			}
			else {
				goto down;
			}
		}
	}

	return path;
};

int main( int argc, char ** argv ) {

	QCoreApplication app( argc, argv );
	NBSelectionArea area( 10, 10 );

	area.addRect( QRect( 10, 0, 10, 10 ), 1, 1 );

	qDebug() << area.toPath();

	return 0;
};
