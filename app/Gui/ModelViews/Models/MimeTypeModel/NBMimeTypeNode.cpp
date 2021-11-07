/*
	*
	* NBMimeTypeNode.cpp - NewBreeze NBMimeTypeNode Class
	*
*/

#include "NBMimeTypeNode.hpp"

NBMimeTypeNode::NBMimeTypeNode( QMimeType mimeType, NBMimeTypeNode *parent ) {

	mCategory = mimeType.name().split( "/" ).value( 0 );
	mName = mimeType.name().split( "/" ).value( 1 );
	mIcon = QIcon::fromTheme( "application-octet-stream" );

	parentNode = parent;
};

void NBMimeTypeNode::addChild( NBMimeTypeNode *node ) {

	childNodes << node;
};

void NBMimeTypeNode::removeChild( NBMimeTypeNode *node ) {

	delete childNodes.takeAt( node->row() );
};

NBMimeTypeNode* NBMimeTypeNode::child( int row ) {

	return childNodes.at( row );
};

NBMimeTypeNode* NBMimeTypeNode::child( QString name ) {

	foreach( NBMimeTypeNode* node, childNodes )
		if ( node->name() == name )
			return node;

	return new NBMimeTypeNode();
};

QList<NBMimeTypeNode*> NBMimeTypeNode::children() {

	return childNodes;
};

int NBMimeTypeNode::childCount() {

	return childNodes.count();
};

void NBMimeTypeNode::clearChildren() {

	childNodes.clear();
};

QString NBMimeTypeNode::name() {

	return mName;
}

QString NBMimeTypeNode::category() {

	return mCategory;
};

QIcon NBMimeTypeNode::icon() {

	return mIcon;
}

void NBMimeTypeNode::setIcon( QIcon newIcon ) {

	mIcon = newIcon;
}

NBMimeTypeNode* NBMimeTypeNode::parent() {

	return parentNode;
};

int NBMimeTypeNode::row() {

	if ( parentNode )
		return parentNode->childNodes.indexOf( this );

	return 0;
};

void NBMimeTypeNode::sort() {

	std::sort( childNodes.begin(), childNodes.end(), mimeTypeNodeSort );
};

bool mimeTypeNodeSort( NBMimeTypeNode *first, NBMimeTypeNode *second ) {

	if ( first->category() == second->category() ) {

		return first->name().toLower() < second->name().toLower();
	}

	else {

		return first->category() < second->category();
	}
};
