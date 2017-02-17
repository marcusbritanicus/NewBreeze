/*
	*
	* NBMimeTypeNode.hpp - NewBreeze NBMimeTypeNode Class Header
	*
*/

#pragma once

#include "Global.hpp"

class NBMimeTypeNode {

	public:
		NBMimeTypeNode( QMimeType mimeType = QMimeType(), NBMimeTypeNode *parent = 0 );

		int childCount();

		void clearChildren();

		void addChild( NBMimeTypeNode* );
		void removeChild( NBMimeTypeNode* );

		NBMimeTypeNode* child( int );
		NBMimeTypeNode* child( QString );
		QList<NBMimeTypeNode*> children();

		NBMimeTypeNode *parent();
		int row();

		void sort();

		QString name();
		QString category();

		QIcon icon();
		void setIcon( QIcon );

	private:
		QList<NBMimeTypeNode*> childNodes;
		NBMimeTypeNode *parentNode;

		QString mCategory;
		QString mName;
		QIcon mIcon;
};

bool mimeTypeNodeSort( NBMimeTypeNode *first, NBMimeTypeNode *second );
