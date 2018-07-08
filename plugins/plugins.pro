TEMPLATE = subdirs

SUBDIRS += AVPlugin Default TextEdit TermPlugin Thumbnailer

isEqual( QT_MAJOR_VERSION, 5 ) {
	lessThan( QT_MINOR_VERSION, 5 ) {
		SUBDIRS += MarkDown4
	}
	isEqual( QT_MINOR_VERSION, 5 ) | greaterThan( QT_MINOR_VERSION, 5 ) {
		SUBDIRS += MarkDown
	}
}

isEqual( QT_MAJOR_VERSION, 4 ) {
	SUBDIRS += MarkDown4
}
