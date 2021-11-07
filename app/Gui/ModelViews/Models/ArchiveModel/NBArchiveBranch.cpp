/*
	*
	* NBArchiveBranch.cpp - NewBreeze NBArchiveBranch Class
	*
*/

#include "NBArchiveBranch.hpp"

NBArchiveBranch::NBArchiveBranch() : QObject() {

	__showHidden = false;

	mPath = QString();
	mIcon = QIcon();

	parentNode = 0;
};

NBArchiveBranch::NBArchiveBranch( QString name, QIcon icon, NBArchiveBranch *parent ) : QObject()  {

	__showHidden = false;

	mPath = QString( name );
	mIcon = icon;

	parentNode = parent;
};

int NBArchiveBranch::branchCount() {

	return mBranches.count();
};

void NBArchiveBranch::clearBranches() {

	mBranches.clear();
};

void NBArchiveBranch::addBranch( NBArchiveBranch *newNode ) {

	Q_FOREACH( NBArchiveBranch* node, mBranches )
		if ( node->name() == newNode->name() )
			return;

	mBranches << newNode;
};

void NBArchiveBranch::removeBranch( NBArchiveBranch *node ) {

	delete mBranches.takeAt( node->row() );
};

NBArchiveBranch* NBArchiveBranch::branch( int row ) {

	return mBranches.at( row );
};

NBArchiveBranch* NBArchiveBranch::branch( QString name ) {

	Q_FOREACH( NBArchiveBranch* node, mBranches )
		if ( node->name() == name )
			return node;

	return new NBArchiveBranch();
};

QList<NBArchiveBranch*> NBArchiveBranch::branches() {

	return mBranches;
};

QStringList NBArchiveBranch::nameFilters() {

	return __nameFilters;
};

void NBArchiveBranch::setNameFilters( QStringList filters ) {

	__nameFilters.clear();
	__nameFilters << filters;
};

void NBArchiveBranch::clearNameFilters() {

	__nameFilters.clear();
};

bool NBArchiveBranch::showHidden() const {

	return __showHidden;
};

void NBArchiveBranch::setShowHidden( bool shown ) {

	__showHidden = shown;
};

QString NBArchiveBranch::name() {

	return mPath;
}

QIcon NBArchiveBranch::icon() {

	return mIcon;
};

QVariant NBArchiveBranch::data( int role ) const {

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

bool NBArchiveBranch::setData( int, QVariant ) {

	return true;
};

NBArchiveBranch* NBArchiveBranch::parent() {

	return parentNode;
};

int NBArchiveBranch::row() {

	/* If mPath is not defined */
	if ( not mPath.count() )
		return -1;

	if ( parentNode )
		return parentNode->mBranches.indexOf( this );

	return 0;
};

void NBArchiveBranch::sort() {

	std::sort( mBranches.begin(), mBranches.end(), caseInsensitiveNameSort );
};

bool caseInsensitiveNameSort( NBArchiveBranch *first, NBArchiveBranch *second )  {

	QString name1 = first->name().toLower();
	QString name2 = second->name().toLower();

	return name1 < name2;
};
