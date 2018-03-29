/*
	*
	* NBItemViewNode.cpp - NewBreeze NBItemViewNode Class
	*
*/

#include "NBItemViewNode.hpp"
#include "NBIconManager.hpp"

inline static QIcon getIcon( QString name, QString type ) {

	QIcon icon;
	if ( type.isEmpty() ) {
		if ( exists( name ) )
			icon.addFile( name );

		else {
			Q_FOREACH( QString icoStr, NBIconManager::instance()->icon( name ) ) {
				QRegExp rx( "[0-9]+" );
				rx.indexIn( icoStr );
				int num = rx.cap( 0 ).toInt();
				icon.addFile( icoStr, QSize( num, num ) );
			}
		}
	}

	else {
		char *newPath[ PATH_MAX ] = { 0 };

		QString path = QString::fromLocal8Bit( realpath( name.toLocal8Bit().constData(), NULL ) );
		Q_FOREACH( QString icoStr, NBIconManager::instance()->iconsForFile( type, path ) ) {
			QRegExp rx( "[0-9]+" );
			rx.indexIn( icoStr );
			int num = rx.cap( 0 ).toInt();
			icon.addFile( icoStr, QSize( num, num ) );
		}
	}

	return icon;
};

NBItemViewNode::NBItemViewNode() {

	for( int i = 0; i < 10; i++ )
		nodeData << "";

	mIcon = QIcon( ":/icons/unknown.png" );

	mCategoryList.clear();
	myCategory = "Uncategorized";
	parentNode = 0;
};

NBItemViewNode::NBItemViewNode( QVariantList data, QString category, NBItemViewNode *parent ) {

	nodeData << data;
	myCategory = category;

	if ( data.at( 0 ).toString() == "Application" )
		mIcon = getIcon( data.at( 7 ).toString(), QString() );

	else if ( data.at( 0 ).toString() == "SuperStart" )
		mIcon = QIcon( data.at( 2 ).toString() );

	else if ( data.at( 0 ).toString() == "dirss" )
		mIcon = getIcon( data.at( 10 ).toString(), QString( "inode/directory" ) );

	else
		mIcon = getIcon( data.at( 3 ).toString(), data.at( 6 ).toString() );

	parentNode = parent;
};

NBItemViewNode::~NBItemViewNode() {

	// delete &future;
};

void NBItemViewNode::addChild( NBItemViewNode *node ) {

	if ( not mCategoryList.contains( node->category() ) )
		mCategoryList << node->category();

	childNodes << node;
};

void NBItemViewNode::removeChild( NBItemViewNode *node ) {

	delete childNodes.takeAt( node->row() );
};

NBItemViewNode* NBItemViewNode::child( int row ) {

	return childNodes.at( row );
};

NBItemViewNode* NBItemViewNode::child( QString name ) {

	foreach( NBItemViewNode* node, childNodes )
		if ( node->data( 0 ) == name )
			return node;

	return new NBItemViewNode();
};

QList<NBItemViewNode*> NBItemViewNode::children() {

	return childNodes;
};

int NBItemViewNode::childCount() {

	return childNodes.count();
};

int NBItemViewNode::categoryCount() {

	return mCategoryList.count();
};

void NBItemViewNode::clearChildren() {

	childNodes.clear();
	mCategoryList.clear();
};

QIcon NBItemViewNode::icon() {

	return mIcon;
};

QString NBItemViewNode::category() {

	return myCategory;
};

void NBItemViewNode::setCategory( QString newCategory ) {

	myCategory = newCategory;
};

int NBItemViewNode::categoryIndex() {

	return parentNode->mCategoryList.indexOf( myCategory );
};

QStringList NBItemViewNode::categoryList() {

	return mCategoryList;
};

QVariant NBItemViewNode::data( int column, bool special ) const {

	/*
		*
		* If we want special data, i.e., we can choose column 0, 1, or 2
		* If we want normal data, then we can choose columns 0 to 6, internally 3 onwards
		*
		* [
		* 	dir/file/system			0
		*   rawsize					1
		*   iconstr					2
		*   name = 0				3
		*   size = 1				4
		*   type = 2				5
		*   mime = 3				6
		*   time = 4				7
		*   perm = 5				8
		*   ownr = 6				9
		*   mtpt = 7				10
		* ]
		*
	*/

	if ( special )
		return nodeData.at( column );

	else
		return nodeData.value( 3 + column, QString() );
};

QVariantList NBItemViewNode::allData() {

	return nodeData;
}

bool NBItemViewNode::setData( int column, QVariant data, bool special ) {

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
		if ( column == 2 ) {
			// Icon String
			mIcon = QIcon::fromTheme( data.toString(), QIcon( data.toString() ) );

			if ( mIcon.isNull() )
				mIcon = QIcon( ":/icons/unknown.png" );
		}
	}

	else {
		if ( ( column < 0 ) or ( column > 7 ) )
			return false;

		nodeData.replace( 3 + column, data );
	}

	return true;
};

NBItemViewNode* NBItemViewNode::parent() {

	return parentNode;
};

int NBItemViewNode::row() {

	if ( parentNode )
		return parentNode->childNodes.indexOf( this );

	return 0;
};

void NBItemViewNode::sort( int column, bool cs, bool categorized ) {

	if ( categorized )
		mCategoryList = sortCategoryList( mCategoryList );

	qSort( childNodes.begin(), childNodes.end(), columnSort2 );
};

void NBItemViewNode::updateIcon() {

	mIcon = getIcon( nodeData.at( 3 ).toString(), nodeData.at( 6 ).toString() );
};

void NBItemViewNode::updateCategories() {

	NBItemViewNode::mCategoryList.clear();
	foreach( NBItemViewNode *cNode, childNodes ) {
		QString newCategory = cNode->category();
		if ( not mCategoryList.contains( newCategory ) )
			mCategoryList << newCategory;
	}

	if ( Settings->General.Grouping )
		mCategoryList = sortCategoryList( mCategoryList );
};

bool columnSort2( NBItemViewNode *first, NBItemViewNode *second )  {

	int firstIdx = first->categoryIndex();
	int secondIdx = second->categoryIndex();

	/* If its not a categorized sorting, then both have equal categorization weights */
	if ( not Settings->General.Grouping )
		firstIdx = secondIdx;

	if ( firstIdx == secondIdx ) {
		if ( ( first->data( 0, true ) == "dir" ) and ( second->data( 0, true ) == "dir" ) ) {
			switch( Settings->General.SortColumn ) {
				case 0: {
					if ( Settings->General.SortCase )
						return first->data( 0 ).toString() < second->data( 0 ).toString();

					else
						return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseInsensitive ) < 0;
				}

				case 1: {
					return first->data( 1, true ).toLongLong() < second->data( 1, true ).toLongLong();
				}

				default: {
					if ( first->data( Settings->General.SortColumn ).toString() < second->data( Settings->General.SortColumn ).toString() )
						return true;

					else if ( first->data( Settings->General.SortColumn ).toString() == second->data( Settings->General.SortColumn ).toString() )
						if ( Settings->General.SortCase )
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

			switch( Settings->General.SortColumn ) {
				case 0: {
					if ( Settings->General.SortCase )
						return first->data( 0 ).toString() < second->data( 0 ).toString();

					else
						return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseInsensitive ) < 0;
				}

				case 1: {
					return first->data( 1, true ).toLongLong() < second->data( 1, true ).toLongLong();
				}

				default: {
					if ( first->data( Settings->General.SortColumn ).toString() < second->data( Settings->General.SortColumn ).toString() )
						return true;

					else if ( first->data( Settings->General.SortColumn ).toString() == second->data( Settings->General.SortColumn ).toString() )
						if ( Settings->General.SortCase )
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

			switch( Settings->General.SortColumn ) {
				case 0: {
					if ( Settings->General.SortCase )
						return first->data( 0 ).toString() < second->data( 0 ).toString();

					else
						return first->data( 0 ).toString().compare( second->data( 0 ).toString(), Qt::CaseInsensitive ) < 0;
				}

				case 1: {
					return first->data( 1, true ).toLongLong() < second->data( 1, true ).toLongLong();
				}

				default: {
					if ( first->data( Settings->General.SortColumn ).toString() < second->data( Settings->General.SortColumn ).toString() )
						return true;

					else if ( first->data( Settings->General.SortColumn ).toString() == second->data( Settings->General.SortColumn ).toString() )
						if ( Settings->General.SortCase )
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

	switch( Settings->General.SortColumn ) {
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
				if ( cList.contains( "Computer  " ) ) {
					QStringList catList;
					catList << "Computer  ";
					if ( cList.contains( "Places  " ) )
						catList << "Places  ";
					if ( cList.contains( "Files  " ) )
						catList << "Files  ";

					return catList;
				}
				qSort( cList.begin(), cList.end(), listLessThanB );
			}

			return cList;
		};

		/* Date sort */
		/* We convert all the String dates to QDates, sort them and then convert them back to String */
		case 4: {
			QStringList alphaDates = QStringList() << "Today" << "This Week" << "Last Week" << "This Month" << "Last Month";

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
