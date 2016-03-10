/*
	*
	* NBCatalogItem.cpp - NewBreeze NBCatalogItem Class
	*
*/

#include <NBCatalogItem.hpp>

QStringList NBCatalogItem::catalogList = QStringList();

bool catalogSorter( NBCatalogItem *first, NBCatalogItem *second ) {

	int firstIdx = NBCatalogItem::catalogList.indexOf( first->catalogName() );
	int secondIdx = NBCatalogItem::catalogList.indexOf( second->catalogName() );

	if ( firstIdx > secondIdx )
		return false;

	else if ( firstIdx < secondIdx )
		return true;

	else {
		if ( first->name() < second->name() )
			return true;

		else if ( first->name() == second->name() ) {
			if ( first->data( Qt::UserRole + 2 ).toString() < second->data( Qt::UserRole + 2 ).toString() )
				return true;

			else
				return false;
		}

		else
			return false;
	};
};

NBCatalogItem::NBCatalogItem() {

	for( int i = 0; i < 8; i++ )
		itemData << "";

	myCatalogName = "Uncategorized";
	myIcon = QIcon::fromTheme( "folder" );

	parentItem = 0;
};

NBCatalogItem::NBCatalogItem( QVariantList data, QString catalogName, NBCatalogItem *parent ) {

	itemData << data;
	parentItem = parent;

	myCatalogName = catalogName;
};

void NBCatalogItem::addChild( NBCatalogItem *item ) {

	myChildren << item;

	if ( not catalogList.contains( item->catalogName() ) )
		catalogList << item->catalogName();

	catalogList.sort();
	qSort( myChildren.begin(), myChildren.end(), catalogSorter );
};

void NBCatalogItem::removeChild( NBCatalogItem *item ) {

	myChildren.removeAll( item );
};

NBCatalogItem* NBCatalogItem::child( int row ) {

	return myChildren.at( row );
};

NBCatalogItem* NBCatalogItem::child( QString name ) {

	foreach( NBCatalogItem* node, myChildren )
		if ( node->data( 0 ) == name )
			return node;

	return new NBCatalogItem();
};

QList<NBCatalogItem*> NBCatalogItem::children() {

	return myChildren;
};

int NBCatalogItem::childCount() {

	return myChildren.count();
};

int NBCatalogItem::catalogCount() {

	return catalogList.count();
};

void NBCatalogItem::clearChildren() {

	return myChildren.clear();
};

QVariant NBCatalogItem::data( int column ) const {

	switch( column ) {
		case Qt::DisplayRole         : return itemData[ 0 ];
		case Qt::DecorationRole      : return myIcon;
		case Qt::UserRole + 1        : return itemData[ 0 ];
		case Qt::UserRole + 2        : return itemData[ 1 ];
		case Qt::UserRole + 3        : return itemData[ 2 ];
		default: return QVariant();
	};
};

QString NBCatalogItem::name() {

	return itemData[ 0 ].toString();
};

QIcon NBCatalogItem::icon() {

	return myIcon;
};

void NBCatalogItem::setIcon( QIcon icon ) {

	myIcon = QIcon( icon );
};

QString NBCatalogItem::catalogName() {

	return myCatalogName;
};

int NBCatalogItem::catalogIndex() {

	return catalogList.indexOf( myCatalogName );
};

QString NBCatalogItem::toolTip() {

	return itemData[ 1 ].toString();
};

NBCatalogItem* NBCatalogItem::parent() {

	return parentItem;
};

int NBCatalogItem::row() {

	if ( parentItem )
		return parentItem->myChildren.indexOf( this );

	return 0;
};
