#include "Epeg.h"
#include <stdio.h>

EAPI int saveJpegThumb( const char *path, const char *hashPath ) {

	int thumb_height = 128, thumb_width = 128;

	Epeg_Image *im = epeg_file_open( path );

	if ( !im )
		return false;

	int w, h;
	epeg_size_get( im, &w, &h );

	if ( w > h ) {
		thumb_width = 128;
		thumb_height = 128 * h / w;
	}

	else {
		thumb_height = 128;
		thumb_width = 128 * w / h;
	}

	epeg_decode_size_set( im, thumb_width, thumb_height );
	epeg_quality_set( im, 85 );
	epeg_thumbnail_comments_enable( im, 0 );
	epeg_file_output_set( im, hashPath );
	epeg_encode( im );
	epeg_close( im );

	return true;
};
