/*
	*
	* NBStyleOptionViewItem.cpp - NewBreeze IconView StyleOption Class Header
	*
*/

#pragma once

#include "Global.hpp"

class NBStyleOptionViewItem : public QStyleOptionViewItem {
	public:
		enum NodeType {
			Normal = 0x69BA3A,
			SymLink,
			Executable,
		};

		NBStyleOptionViewItem() {

			nodeType = Normal;
		};

		NBStyleOptionViewItem( QStyleOptionViewItem other ) : QStyleOptionViewItem( other ) {

			nodeType = Normal;
		};

		int nodeType;
};
