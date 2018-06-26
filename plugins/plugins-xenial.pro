TEMPLATE = subdirs

SUBDIRS += AVPlugin Default TextEdit TermPlugin VideoThumbs

Default.file = Default-xenial.pro

isEqual( QT_MAJOR_VERSION, 5 ) {
	lessThan( QT_MINOR_VERSION, 9 ) {
		SUBDIRS += MarkDown4
	}
	isEqual( QT_MINOR_VERSION, 9 ) | greaterThan( QT_MINOR_VERSION, 9 ) {
		SUBDIRS += MarkDown
	}
}

isEqual( QT_MAJOR_VERSION, 4 ) {
	SUBDIRS += MarkDown4
}
