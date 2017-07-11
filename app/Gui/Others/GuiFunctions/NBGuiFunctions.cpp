/*
	*
	* NBGuiFunctions.cpp - Some functions that requite the QtGui module
	*
*/

#include "NBGuiFunctions.hpp"

bool isBrightColor( QColor dst, QColor src ) {

	double sA = src.alpha() / 255;
	double dA = dst.alpha() / 255;

	int sR = src.red();
	int sG = src.green();
	int sB = src.blue();

	int dR = dst.red();
	int dG = dst.green();
	int dB = dst.blue();

	int oR = 0;
	int oG = 0;
	int oB = 0;

	double oA = sA + dA * ( 1 - sA );
	if ( oA == 1 ) {
		oR = 255;
		oG = 255;
		oB = 255;
	}

	else if ( ( int )( oA * 255 ) ) {
		oR = ( sR * sA + dR * dA * ( 1 - sA ) ) / oA;
		oG = ( sG * sA + dG * dA * ( 1 - sA ) ) / oA;
		oB = ( sB * sA + dB * dA * ( 1 - sA ) ) / oA;
	}

	if ( sqrt( 0.299 * oR * oR + 0.587 * oG * oG + 0.114 * oB * oB ) >= 130 )
		return true;

	else
		return false;
};

QIcon icon( QStringList icoStrs ) {

	QIcon icn;
	Q_FOREACH( QString icoStr, icoStrs )
		icn.addFile( icoStr );

	return icn;
}
