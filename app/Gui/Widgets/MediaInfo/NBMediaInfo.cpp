/*
	*
	* Copyright 2018 Britanicus <marcusbritanicus@gmail.com>
	*

	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 2 of the License, or
	* (at your option) any later version.
	*

	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.
	*

	*
	* You should have received a copy of the GNU General Public License
	* along with this program; if not, write to the Free Software
	* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	* MA 02110-1301, USA.
	*
*/

#ifdef USE_MEDIAINFO

#include <MediaInfo/MediaInfo.h>
#include <ZenLib/Ztring.h>
#include <ZenLib/ZtringListList.h>

#include "NBMediaInfo.hpp"

class NBMediaInfoNode {

	public:
		NBMediaInfoNode( QString key = QString(), QString value = QString(), NBMediaInfoNode *parent = 0 ) {

			mKey = key;
			mValue = value;

			parentNode = parent;
		};

		int childCount() {

			return childNodes.count();
		};

		void clearChildren() {

			childNodes.clear();
		};

		void addChild( NBMediaInfoNode *node ) {

			childNodes << node;
		};

		NBMediaInfoNode* child( int row ) {

			return childNodes.value( row );
		};

		QList<NBMediaInfoNode*> children() {

			return childNodes;
		};

		NBMediaInfoNode *parent() {

			return parentNode;
		};

		int row() {
			if ( parentNode ) {

				return parentNode->children().indexOf( this );
			}

			return 0;
		};

		inline QString data( int column ) {

			if ( column < 0 or column > 1 )
				return QString();

			return ( column == 0 ? mKey : mValue );
		};

	private:
		QList<NBMediaInfoNode*> childNodes;
		NBMediaInfoNode *parentNode;

		QString mKey;
		QString mValue;
};

NBMediaInfoModel::NBMediaInfoModel( QString fileName ) : QAbstractItemModel() {

	mFileName = QString( fileName );
	rootNode = new NBMediaInfoNode();

	setupModel();
};

QString NBMediaInfoModel::quickInfo() const {

	return mQuickInfo;
};

int NBMediaInfoModel::rowCount( const QModelIndex &par ) const {

	if ( not par.isValid() )
		return rootNode->childCount();

	else
		return static_cast<NBMediaInfoNode*>( par.internalPointer() )->childCount();
};

int NBMediaInfoModel::columnCount( const QModelIndex & ) const {

	return 2;
};

QVariant NBMediaInfoModel::data( const QModelIndex &idx, int role ) const {

	if ( not idx.isValid() )
		return QVariant();

	NBMediaInfoNode *node = static_cast<NBMediaInfoNode*>( idx.internalPointer() );
	if ( role == Qt::DisplayRole )
			return node->data( idx.column() );

	return QVariant();
};

QModelIndex NBMediaInfoModel::index( int row, int column, const QModelIndex &par ) const {

    if ( row < 0 || column < 0 )
        return QModelIndex();

    if ( ( row >= rowCount( par ) ) and ( column >= columnCount( par ) ) )
		return QModelIndex();

	NBMediaInfoNode *parentNode;
	if ( not par.isValid() )
		parentNode = rootNode;

	else
		parentNode = static_cast<NBMediaInfoNode*>( par.internalPointer() );

	NBMediaInfoNode *childNode = parentNode->child( row );
	if ( childNode )
		return createIndex( row, column, childNode );

	else
		return QModelIndex();
};

QModelIndex NBMediaInfoModel::parent( const QModelIndex &idx ) const {

	if ( not idx.isValid() )
		return QModelIndex();

	NBMediaInfoNode *childNode = static_cast<NBMediaInfoNode*>( idx.internalPointer() );
	NBMediaInfoNode *parentNode = childNode->parent();

	if ( parentNode == rootNode )
		return QModelIndex();

	return createIndex( parentNode->row(), 0, parentNode );
};

void NBMediaInfoModel::setupModel() {

	MediaInfo MI;
	MI.Open( FromQString( mFileName ) );

	QStringList keys;
	for( int s = (int)Stream_General; s < (int)Stream_Max; s++ ) {
		size_t streamNumMax = MI.Count_Get( (stream_t)s );
		if ( not streamNumMax )
			continue;

		for( size_t j = 0; j < streamNumMax; j++ ) {
			QString streamKindName = FromZString( MI.Get( (stream_t)s, j, ZString( "StreamKind/String" ), Info_Text ) );
			if ( streamNumMax > 1 )
				streamKindName += QString( " %1" ).arg( j );

			size_t infoCount = MI.Count_Get( (stream_t)s, j );

			if ( not infoCount )
				continue;

			NBMediaInfoNode *streamNode = new NBMediaInfoNode( streamKindName, QString(), rootNode );

			keys.clear();
			for( size_t i = 0; i < infoCount; i++ ) {
				QString name = FromZString( MI.Get( (stream_t)s, j, i, Info_Name_Text ) );
				QString value = FromZString( MI.Get( (stream_t)s, j, i, Info_Text ) );

				if ( name.contains( "Count", Qt::CaseInsensitive ) )
					continue;

				else if ( name.contains( "name", Qt::CaseInsensitive ) and not name.contains( "track", Qt::CaseInsensitive ) )
					continue;

				else if ( name.startsWith( "Kind of", Qt::CaseInsensitive ) )
					continue;

				else if ( name == "File size" )
					continue;

				else if ( name.contains( "last modification", Qt::CaseInsensitive ) )
					continue;

				else if ( name.contains( "Extensions usually", Qt::CaseInsensitive ) )
					continue;

				if ( value.size() )
					 value += FromZString( MI.Get( (stream_t)s, 0, i, Info_Measure_Text ) );

				if ( not name.size() or not value.size() )
					continue;

				if ( not keys.contains( name ) ) {
					keys << name;
					streamNode->addChild( new NBMediaInfoNode( name, value, streamNode ) );
				}
			}

			rootNode->addChild( streamNode );
		}
	}

	mQuickInfo = FromZString( MI.Inform() );
};

NBMediaInfoWidget::NBMediaInfoWidget( QString fileName, QWidget *parent ) : QTreeView( parent ) {

	setMinimumWidth( 250 );
	setSelectionMode( QAbstractItemView::NoSelection );
	setFocusPolicy( Qt::NoFocus );

	QPalette pltt = palette();
	pltt.setColor( QPalette::Base, pltt.color( QPalette::Window ) );

	setPalette( pltt );

	if ( exists( fileName ) ) {
		NBMediaInfoModel *model = new NBMediaInfoModel( fileName );
		setModel( model );

		expand( model->index( 0, 0, QModelIndex() ) );
		resizeColumnToContents( 0 );
	}
};

void NBMediaInfoWidget::setFileName( QString fileName ) {

	if ( exists( fileName ) ) {
		NBMediaInfoModel *model = new NBMediaInfoModel( fileName );
		setModel( model );

		expand( model->index( 0, 0, QModelIndex() ) );
		resizeColumnToContents( 0 );
	}
};

#ifdef MI_TEST

int main( int argc, char *argv[] ) {

	QApplication app( argc, argv );

	NBMediaInfoWidget *view = new NBMediaInfoWidget( argv[ 1 ] );
	view->show();

	view->setMinimumWidth( view->width() + 71 );

	return app.exec();
};

#endif

#endif
