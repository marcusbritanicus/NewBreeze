/*
	*
	* NBFileSystemNode.cpp - NewBreeze NBFileSystemNode Class
	*
*/

#include <NBFileSystemNode.hpp>

static int __sortColumn = Settings->Session.SortColumn;
static bool __sortCase = Settings->Session.SortCase;
static bool __sortCategory = Settings->Session.SortCategory;

QStringList NBFileSystemNode::categoryList = QStringList();

NBFileSystemNode::NBFileSystemNode() {

	for( int i = 0; i < 10; i++ )
		nodeData << "";

	myCategory = "Uncategorized";
	parentNode = 0;
};

NBFileSystemNode::NBFileSystemNode( QVariantList data, QString category, NBFileSystemNode *parent ) {

	nodeData << data;
	myCategory = category;

	parentNode = parent;
};

void NBFileSystemNode::addChild( NBFileSystemNode *node ) {

	if ( not categoryList.contains( node->category() ) )
		categoryList << node->category();

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

	return categoryList.count();
};

void NBFileSystemNode::clearChildren() {

	childNodes.clear();
	categoryList.clear();
};

QString NBFileSystemNode::category() {

	return myCategory;
};

void NBFileSystemNode::setCategory( QString newCategory ) {

	myCategory = newCategory;
};

int NBFileSystemNode::categoryIndex() {

	return categoryList.indexOf( myCategory );
};

QVariant NBFileSystemNode::data( int column, bool special ) const {

	/*
		*
		* If we want special data, i.e., we can choose column 0, 1, or 2
		* If we want normal data, then we can choose columns 0 to 6, internally 3 to 9
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
		NBFileSystemNode::categoryList = sortCategoryList( NBFileSystemNode::categoryList );

	qSort( childNodes.begin(), childNodes.end(), columnSort2 );
};

void NBFileSystemNode::updateCategories() {

	NBFileSystemNode::categoryList.clear();
	foreach( NBFileSystemNode *cNode, childNodes ) {
		QString newCategory = cNode->category();
		if ( not NBFileSystemNode::categoryList.contains( newCategory ) )
			NBFileSystemNode::categoryList << newCategory;
	}

	if ( __sortCategory )
		NBFileSystemNode::categoryList = sortCategoryList( NBFileSystemNode::categoryList );
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


QStringList sortCategoryList( QStringList& cList ) {

	switch( __sortColumn ) {
		/* Name sort */
		case 0: {
			cList.sort();
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
				qSort( cList.begin(), cList.end(), []( QString a, QString b ) { return a.toLower() < b.toLower(); } );
				cList.insert( 0, "Folders" );
			}

			else {
				qSort( cList.begin(), cList.end(), []( QString a, QString b ) { return a.toLower() < b.toLower(); } );
			}

			return cList;
		};

		/* Date sort */
		/* We convert all the String dates to QDates, sort them and then convert them back to String */
		case 4: {
			QList<QDate> dList;
			QStringList nList;
			Q_FOREACH( QString date, cList ) {
				dList << QDate::fromString( date, "MMMM yyyy" );
			}

			qSort( dList.begin(), dList.end() );
			Q_FOREACH( QDate date, dList )
				nList << date.toString( "MMMM yyyy" );

			return nList;
		};

		default: {
			return cList;
		}
	};
};
