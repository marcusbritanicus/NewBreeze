/*
	*
	* NBBookmarkInfo.cpp - Booksmarks Info class
	*
*/

#include <NBBookmarkInfo.hpp>

QList<NBBookmarkInfo> NBBookmarkInfo::allBookmarks() {

	QList<NBBookmarkInfo> bookmarkInfoList;

	foreach( QString bookmark, bookmarkSettings.value( "Order" ).toStringList() ) {
		if ( not exists( bookmark ) )
			continue;

		QString label = bookmarkSettings.value( QUrl::toPercentEncoding( bookmark ) ).toString();

		NBBookmarkInfo bmkInfo;
		bmkInfo.displayLabel = label;
		bmkInfo.displayIcon = "bookmarks";
		bmkInfo.mountPoint = bookmark;

		bookmarkInfoList << bmkInfo;
	}

	return bookmarkInfoList;
};
