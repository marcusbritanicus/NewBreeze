TEMPLATE = subdirs

SUBDIRS += Crypt Default TextEdit

isEqual( QT_MAJOR_VERSION, 5 ) {
	SUBDIRS += MarkDown
}

isEqual( QT_MAJOR_VERSION, 4 ) {
	SUBDIRS += MarkDown4
}
