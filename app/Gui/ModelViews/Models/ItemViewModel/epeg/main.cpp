#include "Epeg.h"
#include <QtCore>

int main( int argc, char **argv ) {

	return saveJpegThumb( QString( argv[ 1 ] ).toLocal8Bit().constData(), "/home/cosmos/thumb.jpg" );
}
