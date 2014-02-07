/*
	*
	* NBTrashNode.hpp - NewBreeze NBTrashNode Class Header
	*
*/

#pragma once
#ifndef NBTRASHNODE_HPP
#define NBTRASHNODE_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBIconProvider.hpp>

class NBTrashNode {

	public:
		NBTrashNode();
		NBTrashNode( QStringList, QString, NBTrashNode *parent = 0 );

		int childCount();
		int categoryCount();

		void clearChildren();

		void addChild( NBTrashNode* );
		void removeChild( NBTrashNode* );

		NBTrashNode* child( int );
		NBTrashNode* child( QString );
		QList<NBTrashNode*> children();

		QVariant data( int column ) const;
		QStringList allData();
		bool setData( int column, QString data );

		/* Info functions */
		QString name();
		QIcon icon();
		QString size();
		QDateTime deletionDate();
		QString originalPath();
		QString trashPath();
		QString trashInfoPath();
		QString type();

		QString category();
		void setCategory( QString );

		int categoryIndex();

		QStringList categoryList();

		NBTrashNode *parent();
		int row();

		void sort();

		void updateCategories();
		QStringList mCategoryList;

	private:
		QList<NBTrashNode*> childNodes;
		QStringList nodeData;
		NBTrashNode *parentNode;

		QString myCategory;
		QString mType;
		QString mSize;
		QDateTime mDate;
		QIcon mIcon;
};

bool columnSort( NBTrashNode *first, NBTrashNode *second );
QStringList sortDateList( QStringList& );

#endif
