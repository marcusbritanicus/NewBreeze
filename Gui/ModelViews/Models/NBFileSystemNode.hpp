/*
	*
	* NBNBFileSystemNode.hpp - NewBreeze NBFileSystemNode Class Header
	*
*/

#ifndef NBFILESYSTEMNODE_HPP
#define NBFILESYSTEMNODE_HPP

#include <Global.hpp>

class NBFileSystemNode {
	public:
		NBFileSystemNode();
		NBFileSystemNode( QVariantList, NBFileSystemNode *parent = 0 );

		int childCount();
		int columnCount();

		void clearChildren();

		void appendChild( NBFileSystemNode* );
		void insertChildAt( int, NBFileSystemNode* );
		void removeChildAt( int );

		NBFileSystemNode* child( int );
		NBFileSystemNode* child( QString );

		QVariant data( int column, bool special = false ) const;
		bool setData( int column, QVariant data, bool special = false );

		NBFileSystemNode *parent();
		int row();

		void sort( int column = 0, bool cs = false, bool dirsFirst = true );

	private:
		QList<NBFileSystemNode*> childNodes;
		QVariantList nodeData;
		NBFileSystemNode *parentNode;
};

bool columnSort( const NBFileSystemNode *first, const NBFileSystemNode *second );

#endif
