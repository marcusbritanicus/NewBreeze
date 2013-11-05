/*
	*
	* NBApplicationItem.cpp - NewBreeze NBApplicationItem Class
	*
*/

#include <NBApplicationItem.hpp>

QStringList NBApplicationItem::categoryList = QStringList();

bool categorySorter( NBApplicationItem *first, NBApplicationItem *second ) {

	int firstIdx = NBApplicationItem::categoryList.indexOf( first->category() );
	int secondIdx = NBApplicationItem::categoryList.indexOf( second->category() );

	if ( firstIdx > secondIdx )
		return false;

	else if ( firstIdx < secondIdx )
		return true;

	else {
		if ( first->name() < second->name() )
			return true;

		else
			return false;
	};
};

NBApplicationItem::NBApplicationItem() {

	for( int i = 0; i < 8; i++ )
		itemData << "";

	myCategory = "Uncategorized";
	myIcon = QIcon::fromTheme( "unknown" );

	parentItem = 0;
};

NBApplicationItem::NBApplicationItem( QVariantList data, QString category, NBApplicationItem *parent ) {

	itemData << data;
	parentItem = parent;

	myCategory = category;
};

void NBApplicationItem::addChild( NBApplicationItem *item ) {

	myChildren << item;

	if ( not categoryList.contains( item->category() ) )
		categoryList << item->category();

	categoryList.sort();
	qSort( myChildren.begin(), myChildren.end(), categorySorter );
};

void NBApplicationItem::removeChild( NBApplicationItem *item ) {

	myChildren.removeAll( item );
};

NBApplicationItem* NBApplicationItem::child( int row ) {

	return myChildren.at( row );
};

NBApplicationItem* NBApplicationItem::child( QString name ) {

	foreach( NBApplicationItem* node, myChildren )
		if ( node->data( 0 ) == name )
			return node;

	return new NBApplicationItem();
};

QList<NBApplicationItem*> NBApplicationItem::children() {

	return myChildren;
};

int NBApplicationItem::childCount() {

	return myChildren.count();
};

int NBApplicationItem::categoryCount() {

	return categoryList.count();
};

void NBApplicationItem::clearChildren() {

	return myChildren.clear();
};

QVariant NBApplicationItem::data( int column ) const {

	switch( column ) {
		case Qt::DisplayRole         : return itemData[ 0 ];
		case Qt::DecorationRole      : return myIcon;
		case Qt::UserRole + 1        : return itemData[ 0 ];
		case Qt::UserRole + 2        : return itemData[ 1 ];
		case Qt::UserRole + 3        : return itemData[ 2 ];
		case Qt::UserRole + 4        : return itemData[ 3 ];
		case Qt::UserRole + 5        : return itemData[ 4 ];
		case Qt::UserRole + 6        : return itemData[ 5 ];
		case Qt::UserRole + 7        : return itemData[ 6 ];
		case Qt::UserRole + 8        : return itemData[ 7 ];
		default: return QVariant();
	};
};

QString NBApplicationItem::name() {

	return itemData[ 0 ].toString();
};

QIcon NBApplicationItem::icon() {

	return myIcon;
};

void NBApplicationItem::setIcon( QIcon icon ) {

	myIcon = QIcon( icon );
};

QString NBApplicationItem::category() {

	return myCategory;
};

int NBApplicationItem::categoryIndex() {

	return categoryList.indexOf( myCategory );
};

QString NBApplicationItem::toolTip() {

	return itemData[ 0 ].toString();
};

NBApplicationItem* NBApplicationItem::parent() {

	return parentItem;
};

int NBApplicationItem::row() {

	if ( parentItem )
		return parentItem->myChildren.indexOf( this );

	return 0;
};
