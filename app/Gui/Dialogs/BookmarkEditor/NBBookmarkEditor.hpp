/*
	*
	* NBBookmarkEditor.hpp - NBBookmarkEditor.cpp class Header
	*
*/

#pragma once

#include "Global.hpp"

class NBBookmarkEditor : public NBDialog {
	Q_OBJECT

	public:
		NBBookmarkEditor( QWidget *parent );
};

class NBBookmarksModel : public QAbstractListModel {
	Q_OBJECT
};

// +---------------------------------------------------------------------------------------+
// | Bookmark Editor: Edit the bookmarks                                                   |
// |+---------------------------------------------------------------+                      |
// || Bookmark 1                                                    |     Move Up          |
// || Bookmakr 2                                                    |     Move Down        |
// || Bookmark 3                                                    |                      |
// || ...                                                           |     Remove           |
// || ...                                                           |     Rename           |
// || ...                                                           |                      |
// ||                                                               |                      |
// ||                                                               |                      |
// ||                                                               |                      |
// ||                                                               |                      |
// ||                                                               |                      |
// ||                                                               |                      |
// ||                                                               |                      |
// ||                                                               |                      |
// ||                                                               |           +----------+
// |+---------------------------------------------------------------+           | X  Close |
// +----------------------------------------------------------------------------+----------+
