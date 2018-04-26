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

int NBTreeBranch::childCount() {

	return childNodes.count();
};

void NBTreeBranch::explore() {

	DIR *dir;
	struct dirent *ent;
	dir = opendir( mPath.toLocal8Bit().data() );

	clearChildren();

	if ( dir != NULL ) {
		while ( ( ent = readdir( dir ) ) != NULL) {

			/* We need only directories */
			if ( ent->d_type != DT_DIR )
				continue;

			/* Do not show . and .. */
			QString nodeName = QString::fromLocal8Bit( ent->d_name );
			if ( ( nodeName.compare( "." ) == 0 ) or ( nodeName.compare( ".." ) == 0 ) )
				continue;

			/* Show Hidden */
			if ( __showHidden ) {
				/* If we have set name filters */
				if ( __nameFilters.count() and matchesFilter( __nameFilters, nodeName ) )
					addChild( new NBTreeBranch( mPath + nodeName + "/", QIcon::fromTheme( "folder" ), this ) );

				else
					addChild( new NBTreeBranch( mPath + nodeName + "/", QIcon::fromTheme( "folder" ), this ) );
			}

			/* Hide Hidden */
			else {
				if ( not nodeName.startsWith( "." ) ) {
					/* We want to filter folders too */
					if ( __nameFilters.count() and matchesFilter( __nameFilters, nodeName ) )
						addChild( new NBTreeBranch( mPath + nodeName + "/", QIcon::fromTheme( "folder" ), this ) );

					else
						addChild( new NBTreeBranch( mPath + nodeName + "/", QIcon::fromTheme( "folder" ), this ) );
				}
			}
		}

		closedir( dir );
		sort();
	}
};

void NBTreeBranch::clearChildren() {

	childNodes.clear();
};

void NBTreeBranch::addChild( NBTreeBranch *node ) {

	childNodes << node;
};

void NBTreeBranch::removeChild( NBTreeBranch *node ) {

	delete childNodes.takeAt( node->row() );
};

NBTreeBranch* NBTreeBranch::child( int row ) {

	return childNodes.at( row );
};

NBTreeBranch* NBTreeBranch::child( QString name ) {

	foreach( NBTreeBranch* node, childNodes )
		if ( node->name() == name )
			return node;

	return new NBTreeBranch();
};

QList<NBTreeBranch*> NBTreeBranch::children() {

	return childNodes;
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

	if ( mPath == "/" )
		return "/";

	else
		return baseName( mPath );
}

QIcon NBTreeBranch::icon() {

	return mIcon;
};

QVariant NBTreeBranch::data( int role ) const {

	switch( role ) {
		case Qt::DisplayRole:
			return baseName( mPath );

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
		return parentNode->childNodes.indexOf( this );

	return 0;
};

void NBTreeBranch::sort() {

	qSort( childNodes.begin(), childNodes.end(), caseInsensitiveNameSort );
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
