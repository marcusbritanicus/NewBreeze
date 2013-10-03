/*
	*
	* NBSidePanelItem.cpp - NewBreeze NBSidePanelItem Class
	*
*/

#include <NBSidePanelItem.hpp>

NBSidePanelItem::NBSidePanelItem() {

	nodeData << "" << "" << "";
	parentNode = 0;
};

NBSidePanelItem::NBSidePanelItem( QVariantList data, NBSidePanelItem *parent ) {

	nodeData << data;
	parentNode = parent;
};

void NBSidePanelItem::appendChild( NBSidePanelItem *node ) {

	childNodes << node;
};

void NBSidePanelItem::insertChildAt( int row, NBSidePanelItem *node ) {

	childNodes.insert( row, node );
};

void NBSidePanelItem::removeChildAt( int row ) {

	delete childNodes.takeAt( row );
};

NBSidePanelItem* NBSidePanelItem::child( int row ) {

	return childNodes.at( row );
};

NBSidePanelItem* NBSidePanelItem::child( QString name ) {

	foreach( NBSidePanelItem* node, childNodes )
		if ( node->data( 0 ) == name )
			return node;

	return new NBSidePanelItem( QVariantList() );
};

int NBSidePanelItem::childCount() {

	return childNodes.count();
};

void NBSidePanelItem::clearChildren() {

	return childNodes.clear();
};

int NBSidePanelItem::columnCount() {

	// We have only one columns: Label

	return 1;
};

QVariant NBSidePanelItem::data( int column ) const {

	switch( column ) {
		case Qt::DisplayRole     : return nodeData.at( 0 );
		case Qt::DecorationRole  : return nodeData.at( 1 );
		case Qt::UserRole + 1    : return nodeData.at( 2 );
		default                  : return QVariant();
	};

	return QVariant();
};

NBSidePanelItem* NBSidePanelItem::parent() {

	return parentNode;
};

int NBSidePanelItem::row() {

	if ( parentNode )
		return parentNode->childNodes.indexOf( this );

	return 0;
};
