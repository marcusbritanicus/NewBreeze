/*
	*
	* NBFileSystemNode.hpp - NewBreeze NBFileSystemNode Class Header
	*
*/

#ifndef NBFILESYSTEMNODE_HPP
#define NBFILESYSTEMNODE_HPP

#include <Global.hpp>

class NBFileSystemNode {
	public:
		NBFileSystemNode();
		NBFileSystemNode( QVariantList, QString, NBFileSystemNode *parent = 0 );

		int childCount();
		int categoryCount();

		void clearChildren();

		void addChild( NBFileSystemNode* );
		void removeChild( NBFileSystemNode* );

		NBFileSystemNode* child( int );
		NBFileSystemNode* child( QString );
		QList<NBFileSystemNode*> children();

		QVariant data( int column, bool special = false ) const;
		QVariantList allData();
		bool setData( int column, QVariant data, bool special = false );

		QString category();
		void setCategory( QString );

		int categoryIndex();

		QStringList categoryList();

		NBFileSystemNode *parent();
		int row();

		void sort( int column = 0, bool cs = false, bool categorized = false );

		void updateCategories();
		QStringList mCategoryList;

		QIcon icon();

	private:
		QList<NBFileSystemNode*> childNodes;
		QVariantList nodeData;
		NBFileSystemNode *parentNode;

		QString myCategory;
		QIcon mIcon;
};

bool columnSort2( NBFileSystemNode *first, NBFileSystemNode *second );
QStringList sortCategoryList( QStringList& );

#endif
