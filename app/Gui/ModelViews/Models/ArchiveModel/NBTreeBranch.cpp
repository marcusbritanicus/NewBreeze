/*
	*
	* NBTreeBranch.cpp - NewBreeze NBTreeBranch Class
	*
*/

#include "NBTreeBranch.hpp"

NBTreeBranch::NBTreeBranch() : QObject() {

	__showHidden = false;

	mPath = QString();
	mIcon = QIcon();

	parentNode = 0;
};

NBTreeBranch::NBTreeBranch( QString name, QIcon icon, NBTreeBranch *parent ) : QObject()  {

	__showHidden = false;

	mPath = QString( name );
	mIcon = icon;

	parentNode = parent;
};

int NBTreeBranch::branchCount() {

	return mBranches.count();
};

void NBTreeBranch::clearBranches() {

	mBranches.clear();
};

void NBTreeBranch::addBranch( NBTreeBranch *newNode ) {

	Q_FOREACH( NBTreeBranch* node, mBranches )
		if ( node->name() == newNode->name() )
			return;

	mBranches << newNode;
};

void NBTreeBranch::removeBranch( NBTreeBranch *node ) {

	delete mBranches.takeAt( node->row() );
};

NBTreeBranch* NBTreeBranch::branch( int row ) {

	return mBranches.at( row );
};

NBTreeBranch* NBTreeBranch::branch( QString name ) {

	Q_FOREACH( NBTreeBranch* node, mBranches )
		if ( node->name() == name )
			return node;

	return new NBTreeBranch();
};

QList<NBTreeBranch*> NBTreeBranch::branches() {

	return mBranches;
};

QStringList NBTreeBranch::nameFilters() {

	return __nameFilters;
};

void NBTreeBranch::setNameFilters( QStringList filters ) {

	__nameFilters.clear();
	__nameFilters << filters;
};

void NBTreeBranch::clearNameFilters() {

	__nameFilters.clear();
};

bool NBTreeBranch::showHidden() const {

	return __showHidden;
};

void NBTreeBranch::setShowHidden( bool shown ) {

	__showHidden = shown;
};

QString NBTreeBranch::name() {

	return mPath;
}

QIcon NBTreeBranch::icon() {

	return mIcon;
};

QVariant NBTreeBranch::data( int role ) const {

	switch( role ) {
		case Qt::DisplayRole:
			return mPath;

		case Qt::DecorationRole:
			return mIcon;

		case Qt::UserRole + 1:
			return mPath;

		default:
			return QVariant();
	}
};

bool NBTreeBranch::setData( int, QVariant ) {

	return true;
};

NBTreeBranch* NBTreeBranch::parent() {

	return parentNode;
};

int NBTreeBranch::row() {

	/* If mPath is not defined */
	if ( not mPath.count() )
		return -1;

	if ( parentNode )
		return parentNode->mBranches.indexOf( this );

	return 0;
};

void NBTreeBranch::sort() {

	qSort( mBranches.begin(), mBranches.end(), caseInsensitiveNameSort );
};

bool caseInsensitiveNameSort( NBTreeBranch *first, NBTreeBranch *second )  {

	QString name1 = first->name().toLower();
	QString name2 = second->name().toLower();

	return name1 < name2;
};

bool matchesFilter( QStringList filters, QString text ) {

	Q_FOREACH( QString filter, filters )
		if ( text.contains( QRegExp( filter, Qt::CaseInsensitive, QRegExp::Wildcard ) ) )
			return true;

	return false;
};
