/*
	*
	* NBFileSystemNode.cpp - NewBreeze NBFileSystemNode Class
	*
*/

#include <NBFileSystemNode.hpp>

static int __sortColumn = 0;
static bool __sortCase = false;
static bool __sortDirs = true;

NBFileSystemNode::NBFileSystemNode() {

	for( int i = 0; i < 10; i++ )
		nodeData << "";

	parentNode = 0;
};

NBFileSystemNode::NBFileSystemNode( QVariantList data, NBFileSystemNode *parent ) {

	nodeData << data;
	parentNode = parent;
};

void NBFileSystemNode::appendChild( NBFileSystemNode *node ) {

	childNodes << node;
};

void NBFileSystemNode::insertChildAt( int row, NBFileSystemNode *node ) {

	childNodes.insert( row, node );
};

void NBFileSystemNode::removeChildAt( int row ) {

	delete childNodes.takeAt( row );
};

NBFileSystemNode* NBFileSystemNode::child( int row ) {

	return childNodes.at( row );
};

NBFileSystemNode* NBFileSystemNode::child( QString name ) {

	foreach( NBFileSystemNode* node, childNodes )
		if ( node->data( 0 ) == name )
			return node;

	return new NBFileSystemNode( QVariantList() );
};

int NBFileSystemNode::childCount() {

	return childNodes.count();
};

void NBFileSystemNode::clearChildren() {

	return childNodes.clear();
};

int NBFileSystemNode::columnCount() {

	/*
		*
		* We have three special columns which need to be discounted from the count
		*
	*/

	return nodeData.count() - 3;
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

void NBFileSystemNode::sort( int column, bool cs, bool dirsFirst ) {

	__sortColumn = column;
	__sortCase = cs;
	__sortDirs = dirsFirst;

	qSort( childNodes.begin(), childNodes.end(), columnSort );
};

bool columnSort( const NBFileSystemNode *first, const NBFileSystemNode *second )  {

	// dir/file/system, rawsize, icon
	// name, size, type, mime, time, perm, ownr

	if ( __sortDirs ) {

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

	return true;
};
