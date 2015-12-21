/*
	*
	* NBFileSystemNode.cpp - NewBreeze NBFileSystemNode Class
	*
*/

#include <NBFileSystemNode.hpp>

static int __sortColumn = Settings->General.SortColumn;
static bool __sortCase = Settings->General.SortCase;
static bool __sortCategory = Settings->General.Grouping;

NBFileSystemNode::NBFileSystemNode() {

	for( int i = 0; i < 10; i++ )
		nodeData << "";

	mCategoryList.clear();
	myCategory = "Uncategorized";
	parentNode = 0;
};

NBFileSystemNode::NBFileSystemNode( QVariantList data, QString category, NBFileSystemNode *parent ) {

	nodeData << data;
	myCategory = category;

	parentNode = parent;
};

void NBFileSystemNode::addChild( NBFileSystemNode *node ) {

	if ( not mCategoryList.contains( node->category() ) )
		mCategoryList << node->category();

	childNodes << node;
};

void NBFileSystemNode::removeChild( NBFileSystemNode *node ) {

	delete childNodes.takeAt( node->row() );
};

NBFileSystemNode* NBFileSystemNode::child( int row ) {

	return childNodes.at( row );
};

NBFileSystemNode* NBFileSystemNode::child( QString name ) {

	foreach( NBFileSystemNode* node, childNodes )
		if ( node->data( 0 ) == name )
			return node;

	return new NBFileSystemNode();
};

QList<NBFileSystemNode*> NBFileSystemNode::children() {

	return childNodes;
};

int NBFileSystemNode::childCount() {

	return childNodes.count();
};

int NBFileSystemNode::categoryCount() {

	return mCategoryList.count();
};

void NBFileSystemNode::clearChildren() {

	childNodes.clear();
	mCategoryList.clear();
};

QString NBFileSystemNode::category() {

	return myCategory;
};

void NBFileSystemNode::setCategory( QString newCategory ) {

	myCategory = newCategory;
};

int NBFileSystemNode::categoryIndex() {

	return parentNode->mCategoryList.indexOf( myCategory );
};

QStringList NBFileSystemNode::categoryList() {

	return mCategoryList;
};

QVariant NBFileSystemNode::data( int column, bool special ) const {

	/*
		*
		* If we want special data, i.e., we can choose column 0, 1, or 2
		* If we want normal data, then we can choose columns 0 to 6, internally 3 to 9
		*
		* [
		* 	dir/file/system
		*   rawsize
		*   icon ( string used with QIcon::fromTheme )
		*   name = 0
		*   size = 1
		*   type = 2
		*   mime = 3
		*   time = 4
		*   perm = 5
		*   ownr = 6
		* ]
		*
	*/

	if ( special ) {
		if ( ( column < 0 ) or ( column > 2 ) )
			return QVariant();

		return nodeData.at( column );
	}

	else {
		if ( ( column < 0 ) or ( column > 7 ) )
			return QVariant();

		return nodeData.at( 3 + column );
	}
};

QVariantList NBFileSystemNode::allData() {

	return nodeData;
}

bool NBFileSystemNode::setData( int column, QVariant data, bool special ) {

	/*
		*
		* If we want to set special data, i.e., we can choose column 0, 1, or 2
		* If we want to set normal data, then we can choose columns 0 to 6, internally 3 to 9
		*
	*/

	if ( special ) {
		if ( ( column < 0 ) or ( column > 2 ) )
			return false;

		nodeData.replace( column, data );
	}

	else {
		if ( ( column < 0 ) or ( column > 7 ) )
			return false;

		nodeData.replace( 3 + column, data );
	}

	return true;
};

NBFileSystemNode* NBFileSystemNode::parent() {

	return parentNode;
};

int NBFileSystemNode::row() {

	if ( parentNode )
		return parentNode->childNodes.indexOf( this );

	return 0;
};

void NBFileSystemNode::sort( int column, bool cs, bool categorized ) {

	__sortColumn = column;
	__sortCase = cs;
	__sortCategory = categorized;

	if ( categorized )
		mCategoryList = sortCategoryList( mCategoryList );

	qSort( childNodes.begin(), childNodes.end(), columnSort2 );
};

void NBFileSystemNode::updateCategories() {

	NBFileSystemNode::mCategoryList.clear();
	foreach( NBFileSystemNode *cNode, childNodes ) {
		QString newCategory = cNode->category();
		if ( not mCategoryList.contains( newCategory ) )
			mCategoryList << newCategory;
	}

	if ( __sortCategory )
		mCategoryList = sortCategoryList( mCategoryList );
}

bool columnSort2( NBFileSystemNode *first, NBFileSystemNode *second )  {

	int firstIdx = first->categoryIndex();
	int secondIdx = second->categoryIndex();

	/* If its not a categorized sorting, then both have equal categorization weights */
	if ( not __sortCategory )
		firstIdx = secondIdx;

	if ( firstIdx == secondIdx ) {
		if ( ( first->data( 0, true ) == "dir" ) and ( second->data( 0, true ) == "dir" ) ) {
			switch( __sortColumn ) {
				case 0: {
					if ( __sortCase )
						return first->data( 0 ).toString() < second->data( 0 ).toString();

					else
						return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseInsensitive ) < 0;
				}

				case 1: {
					return first->data( 1, true ).toLongLong() < second->data( 1, true ).toLongLong();
				}

				default: {
					if ( first->data( __sortColumn ).toString() < second->data( __sortColumn ).toString() )
						return true;

					else if ( first->data( __sortColumn ).toString() == second->data( __sortColumn ).toString() )
						if ( __sortCase )
							return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseSensitive ) < 0;

						else
							return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseInsensitive ) < 0;

					else
						return false;
				}
			}
		}

		else if ( ( first->data( 0, true ) == "dir" ) and ( second->data( 0, true ) == "file" ) ) {

			return true;
		}

		else if ( ( first->data( 0, true ) == "dir" ) and ( second->data( 0, true ) == "system" ) ) {

			return true;
		}

		else if ( ( first->data( 0, true ) == "file" ) and ( second->data( 0, true ) == "dir" ) ) {

			return false;
		}

		else if ( ( first->data( 0, true ) == "file" ) and ( second->data( 0, true ) == "file" ) ) {

			switch( __sortColumn ) {
				case 0: {
					if ( __sortCase )
						return first->data( 0 ).toString() < second->data( 0 ).toString();

					else
						return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseInsensitive ) < 0;
				}

				case 1: {
					return first->data( 1, true ).toLongLong() < second->data( 1, true ).toLongLong();
				}

				default: {
					if ( first->data( __sortColumn ).toString() < second->data( __sortColumn ).toString() )
						return true;

					else if ( first->data( __sortColumn ).toString() == second->data( __sortColumn ).toString() )
						if ( __sortCase )
							return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseSensitive ) < 0;

						else
							return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseInsensitive ) < 0;

					else
						return false;
				}
			}
		}

		else if ( ( first->data( 0, true ) == "file" ) and ( second->data( 0, true ) == "system" ) ) {

			return true;
		}

		else if ( ( first->data( 0, true ) == "system" ) and ( second->data( 0, true ) == "dir" ) ) {

			return false;
		}

		else if ( ( first->data( 0, true ) == "system" ) and ( second->data( 0, true ) == "file" ) ) {

			return false;
		}

		else if ( ( first->data( 0, true ) == "system" ) and ( second->data( 0, true ) == "system" ) ) {

			switch( __sortColumn ) {
				case 0: {
					if ( __sortCase )
						return first->data( 0 ).toString() < second->data( 0 ).toString();

					else
						return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseInsensitive ) < 0;
				}

				case 1: {
					return first->data( 1, true ).toLongLong() < second->data( 1, true ).toLongLong();
				}

				default: {
					if ( first->data( __sortColumn ).toString() < second->data( __sortColumn ).toString() )
						return true;

					else if ( first->data( __sortColumn ).toString() == second->data( __sortColumn ).toString() )
						if ( __sortCase )
							return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseSensitive ) < 0;

						else
							return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseInsensitive ) < 0;

					else
						return false;
				}
			}
		}
	}

	else if ( firstIdx < secondIdx ) {
		/* Means first item is in a category earlier than the second category */
		return true;
	}

	else {
		/* Means first item is in a category later than the second category */
		return false;
	}

	return false;
};

/* function: less than */
inline bool listLessThanA( QString a, QString b ) {

	if ( a.count() != b.count() )
		return ( a.count() < b.count() );
	else
		return ( a < b );
};

inline bool listLessThanB( QString a, QString b ) {

	return a.toLower() < b.toLower();
};

QStringList sortCategoryList( QStringList& cList ) {

	switch( __sortColumn ) {
		/* Name sort */
		case 0: {
			qSort( cList.begin(), cList.end(), listLessThanA );
			return cList;
		};

		/* Size sort */
		/* The order should be: Folders, Tiny, Small, Medium, Large, Massive */
		case 1: {
			QStringList nList;
			if ( cList.contains( "Folders" ) )
				nList << "Folders";

			if ( cList.contains( "Tiny" ) )
				nList << "Tiny";

			if ( cList.contains( "Small" ) )
				nList << "Small";

			if ( cList.contains( "Medium" ) )
				nList << "Medium";

			if ( cList.contains( "Large" ) )
				nList << "Large";

			if ( cList.contains( "Massive" ) )
				nList << "Massive";

			return nList;
		};

		/* Type sort */
		/* Folder must be the first item Then come the files */
		case 2: {
			if ( cList.contains( "Folders" ) ) {
				cList.removeAll( "Folders" );
				qSort( cList.begin(), cList.end(), listLessThanB );
				cList.insert( 0, "Folders" );
			}

			else {
				qSort( cList.begin(), cList.end(), listLessThanB );
			}

			return cList;
		};

		/* Date sort */
		/* We convert all the String dates to QDates, sort them and then convert them back to String */
		case 4: {
			QStringList alphaDates = QStringList() << "Today" <<"This Week" << "Last Week" << "This Month" << "Last Month";

			QList<QDate> dList;
			QStringList nList;
			Q_FOREACH( QString date, cList ) {
				if ( not alphaDates.contains( date ) )
					dList << QDate::fromString( date, "MMMM yyyy" );
			}

			qSort( dList.begin(), dList.end() );
			Q_FOREACH( QDate date, dList )
				nList << date.toString( "MMMM yyyy" );

			/* This ensures that the user sees the Alphabetic dates at first */
			if ( cList.contains( "Last Month" ) )
				nList.insert( 0, "Last Month" );

			if ( cList.contains( "This Month" ) )
				nList.insert( 0, "This Month" );

			if ( cList.contains( "Last Week" ) )
				nList.insert( 0, "Last Week" );

			if ( cList.contains( "This Week" ) )
				nList.insert( 0, "This Week" );

			if ( cList.contains( "Today" ) )
				nList.insert( 0, "Today" );

			return nList;
		};

		default: {
			return cList;
		}
	};
};
