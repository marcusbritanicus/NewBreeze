 /*
	*
	* NBBookmarksInfo.cpp - BookmarksInfo class for NewBreeze
	*
*/

#include <NBBookmarksInfo.hpp>
#include <sys/statvfs.h>
#include <sys/stat.h>

QList<NBBookmarkInfo> NBBookmarksInfo::allBookmarks() {

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
