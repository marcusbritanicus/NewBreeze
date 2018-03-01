TEMPLATE = subdirs

SUBDIRS += Crypt Default TextEdit

isEqual( QT_MAJOR_VERSION, 5 ) {
	lessThan(QT_MINOR_VERSION, 5) {
		SUBDIRS += MarkDown4
	}
	isEqual( QT_MINOR_VERSION, 5 ) | greaterThan( QT_MINOR_VERSION, 5 ) {
		SUBDIRS += MarkDown
	}
}

isEqual( QT_MAJOR_VERSION, 4 ) {
	SUBDIRS += MarkDown4
}
