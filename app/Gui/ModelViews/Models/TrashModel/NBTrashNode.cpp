/*
	*
	* NBTrashNode.cpp - NewBreeze NBTrashNode Class
	*
*/

#include "NBTrashNode.hpp"

NBTrashNode::NBTrashNode() {

	for( int i = 0; i < 10; i++ )
		nodeData << "";

	mCategoryList.clear();
	myCategory = "Uncategorized";
	parentNode = 0;
};

NBTrashNode::NBTrashNode( QStringList data, QString category, NBTrashNode *parent ) {

	nodeData << data;
	myCategory = category;

	if ( isDir( data.last() ) )
		mType = "dir";

	else if ( isFile( data.last() ) )
		mType = "file";

	else
		mType = "system";

	mSize = formatSize( getSize( data.last() ) );
	mDate = QDateTime::fromString( data.at( 2 ), "yyyyMMddThh:mm:ss" );

	QString icoStr = NBIconProvider::icon( data.last() );
	if ( icoStr.contains( dirName( data.at( 1 ) ) ) )
		icoStr.replace( dirName( data.at( 1 ) ), dirName( data.at( 3 ) ) );

	mIcon = QIcon::fromTheme( icoStr, QIcon( icoStr ) );

	parentNode = parent;
};

void NBTrashNode::addChild( NBTrashNode *node ) {

	if ( not mCategoryList.contains( node->category() ) )
		mCategoryList << node->category();

	childNodes << node;
};

void NBTrashNode::removeChild( NBTrashNode *node ) {

	delete childNodes.takeAt( node->row() );
};

NBTrashNode* NBTrashNode::child( int row ) {

	return childNodes.at( row );
};

NBTrashNode* NBTrashNode::child( QString name ) {

	foreach( NBTrashNode* node, childNodes )
		if ( node->data( 0 ) == name )
			return node;

	return new NBTrashNode();
};

QList<NBTrashNode*> NBTrashNode::children() {

	return childNodes;
};

int NBTrashNode::childCount() {

	return childNodes.count();
};

int NBTrashNode::categoryCount() {

	return mCategoryList.count();
};

void NBTrashNode::clearChildren() {

	childNodes.clear();
	mCategoryList.clear();
};

QString NBTrashNode::category() {

	return myCategory;
};

void NBTrashNode::setCategory( QString newCategory ) {

	myCategory = newCategory;
};

int NBTrashNode::categoryIndex() {

	return parentNode->mCategoryList.indexOf( myCategory );
};

QStringList NBTrashNode::categoryList() {

	return mCategoryList;
};

QVariant NBTrashNode::data( int column ) const {

	/*
		*
		* If we want special data, i.e., we can choose column 0, 1, or 2
		* If we want normal data, then we can choose columns 0 to 6, internally 3 to 9
		*
	*/

	if ( ( column < 0 ) or ( column > 7 ) )
		return QVariant();

	return nodeData.at( column );
};

QStringList NBTrashNode::allData() {

	return nodeData;
}

bool NBTrashNode::setData( int column, QString data ) {

	/*
		*
		* If we want to set special data, i.e., we can choose column 0, 1, or 2
		* If we want to set normal data, then we can choose columns 0 to 6, internally 3 to 9
		*
	*/

	if ( ( column < 0 ) or ( column > 7 ) )
		return false;

	nodeData.replace( column, data );

	return true;
};

QString NBTrashNode::name() {

	return nodeData.at( 0 );
};

QString NBTrashNode::type() {

	return mType;
};

QDateTime NBTrashNode::deletionDate() {

	return mDate;
};

QString NBTrashNode::originalPath() {

	return nodeData.at( 1 );
};

QString NBTrashNode::trashPath() {

	return nodeData.last();
};

QString NBTrashNode::trashInfoPath() {

	return nodeData.last().replace( "/files/", "/info/" ) + ".trashinfo";
};

QString NBTrashNode::size() {

	return mSize;
};

QIcon NBTrashNode::icon() {

	return mIcon;
};

NBTrashNode* NBTrashNode::parent() {

	return parentNode;
};

int NBTrashNode::row() {

	if ( parentNode )
		return parentNode->childNodes.indexOf( this );

	return 0;
};

void NBTrashNode::sort() {

	mCategoryList = sortDateList( mCategoryList );
	qSort( childNodes.begin(), childNodes.end(), columnSort );
};

void NBTrashNode::updateCategories() {

	NBTrashNode::mCategoryList.clear();
	foreach( NBTrashNode *cNode, childNodes ) {
		QString newCategory = cNode->category();
		if ( not mCategoryList.contains( newCategory ) )
			mCategoryList << newCategory;
	}

	mCategoryList = sortDateList( mCategoryList );
}

bool columnSort( NBTrashNode *first, NBTrashNode *second )  {

	int firstIdx = first->categoryIndex();
	int secondIdx = second->categoryIndex();

	if ( firstIdx == secondIdx ) {
		if ( ( first->type() == "dir" ) and ( second->type() == "dir" ) ) {

			if ( first->deletionDate() == second->deletionDate() )
				return first->name().compare( second->name(), Qt::CaseInsensitive ) < 0;

			else
				return first->deletionDate() < second->deletionDate();
		}

		else if ( ( first->type() == "dir" ) and ( second->type() == "file" ) ) {

			return true;
		}

		else if ( ( first->type() == "dir" ) and ( second->type() == "system" ) ) {

			return true;
		}

		else if ( ( first->type() == "file" ) and ( second->type() == "dir" ) ) {

			return false;
		}

		else if ( ( first->type() == "file" ) and ( second->type() == "file" ) ) {

			if ( first->deletionDate() == second->deletionDate() )
				return first->name().compare( second->name(), Qt::CaseInsensitive ) < 0;

			else
				return first->deletionDate() < second->deletionDate();
		}

		else if ( ( first->type() == "file" ) and ( second->type() == "system" ) ) {

			return true;
		}

		else if ( ( first->type() == "system" ) and ( second->type() == "dir" ) ) {

			return false;
		}

		else if ( ( first->type() == "system" ) and ( second->type() == "file" ) ) {

			return false;
		}

		else if ( ( first->type() == "system" ) and ( second->type() == "system" ) ) {

			if ( first->deletionDate() == second->deletionDate() )
				return first->name().compare( second->name(), Qt::CaseInsensitive ) < 0;

			else
				return first->deletionDate() < second->deletionDate();
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


QStringList sortDateList( QStringList& cList ) {

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
