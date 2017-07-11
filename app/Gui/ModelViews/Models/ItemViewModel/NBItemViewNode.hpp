/*
	*
	* NBItemViewNode.hpp - NewBreeze NBItemViewNode Class Header
	*
*/

#pragma once

#include "Global.hpp"

class NBItemViewNode {

	public:
		NBItemViewNode();
		NBItemViewNode( QVariantList, QString, NBItemViewNode *parent = 0 );

		~NBItemViewNode();

		int childCount();
		int categoryCount();

		void clearChildren();

		void addChild( NBItemViewNode* );
		void removeChild( NBItemViewNode* );

		NBItemViewNode* child( int );
		NBItemViewNode* child( QString );
		QList<NBItemViewNode*> children();

		QVariant data( int column, bool special = false ) const;
		QVariantList allData();
		bool setData( int column, QVariant data, bool special = false );

		QString category();
		void setCategory( QString );

		int categoryIndex();

		QStringList categoryList();

		NBItemViewNode *parent();
		int row();

		void sort( int column = 0, bool cs = false, bool categorized = false );

		void updateIcon();
		void updateCategories();

		QIcon icon();

	private:
		QList<NBItemViewNode*> childNodes;
		QVariantList nodeData;
		NBItemViewNode *parentNode;

		QStringList mCategoryList;
		QString myCategory;
		QIcon mIcon;

		QFuture<void> future;
};

bool columnSort2( NBItemViewNode *first, NBItemViewNode *second );
QStringList sortCategoryList( QStringList& );
