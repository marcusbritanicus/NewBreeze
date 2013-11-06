/*
	*
	* NBCatalogModel.cpp - NewBreeze NBCatalogModel Class
	*
*/

#include <NBCatalogModel.hpp>

inline bool matchesFilter( QStringList filters, QString text ) {

	foreach( QString filter, filters )
		if ( text.contains( QRegExp( filter, Qt::CaseInsensitive, QRegExp::Wildcard ) ) )
			return true;

	return false;
};

NBCatalogModel::NBCatalogModel() : QAbstractItemModel() {

	rootItem = new NBCatalogItem();
	setupModelData();
};

int NBCatalogModel::rowCount( const QModelIndex &parent ) const {

	if ( not parent.isValid() )
		return rootItem->childCount();

	else
		return static_cast<NBCatalogItem*>( parent.internalPointer() )->childCount();

	return 0;
};

int NBCatalogModel::rowCount( QString mCatalogName ) const {

	if ( not rootItem->catalogList.contains( mCatalogName ) or mCatalogName.isEmpty() or mCatalogName.isNull() )
		return 0;

	return catalogRowMap[ mCatalogName ].count();
};

int NBCatalogModel::catalogCount() const {

	return rootItem->catalogCount();
};

int NBCatalogModel::columnCount( const QModelIndex & parent ) const {

	Q_UNUSED( parent );

	return 1;
};

Qt::ItemFlags NBCatalogModel::flags( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return Qt::NoItemFlags;

	if ( index.data( NBCatalogModel::Exists ).toBool() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	else
		return Qt::NoItemFlags;
};

QVariant NBCatalogModel::data( const QModelIndex &index, int role ) const {

	NBCatalogItem *item = static_cast<NBCatalogItem*>( index.internalPointer() );
	switch( role ) {
		case Qt::DecorationRole :	return item->icon();
		case Qt::ToolTipRole    :	return item->toolTip();
		default                 :	return item->data( role );
	};
};

QModelIndex NBCatalogModel::index( int row, int, const QModelIndex &parent ) const {

	if ( not hasIndex( row, 0, parent ) )
		return QModelIndex();

	NBCatalogItem *parentItem;
	if ( not parent.isValid() )
		parentItem = rootItem;

	else
		parentItem = (NBCatalogItem *)parent.internalPointer();

	NBCatalogItem *childItem = parentItem->child( row );
	if ( childItem )
		return createIndex( row, 0, childItem );

	else
		return QModelIndex();
};

QModelIndex NBCatalogModel::index( QString name, const QModelIndex &parent ) const {

	NBCatalogItem *parentItem;
	if ( not parent.isValid() )
		parentItem = rootItem;

	else
		parentItem = (NBCatalogItem *)parent.internalPointer();

	int row = parentItem->child( name )->row();
	NBCatalogItem *childItem = parentItem->child( name );

	if ( childItem )
		return createIndex( row, 0, childItem );

	return QModelIndex();
};

QModelIndex NBCatalogModel::parent( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QModelIndex();

	NBCatalogItem *childItem = ( NBCatalogItem * )index.internalPointer();
	NBCatalogItem *parentItem = childItem->parent();

	if ( parentItem == rootItem )
		return QModelIndex();

	return createIndex( parentItem->row(), 0, parentItem );
};

QString NBCatalogModel::catalogName( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QString();

	NBCatalogItem *childItem = ( NBCatalogItem * )index.internalPointer();
	return childItem->catalogName();
};

int NBCatalogModel::catalogIndex( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return -1;

	NBCatalogItem *childItem = ( NBCatalogItem * )index.internalPointer();
	return childItem->catalogIndex();
};

QStringList NBCatalogModel::catalogs() const {

	return rootItem->catalogList;
};

void NBCatalogModel::hideCatalog( QString mCatalogName ) {

	if ( rootItem->catalogList.contains( mCatalogName ) )
		catalogVisibilityMap[ mCatalogName ] = false;
};

void NBCatalogModel::showCatalog( QString mCatalogName ) {

	if ( rootItem->catalogList.contains( mCatalogName ) )
		catalogVisibilityMap[ mCatalogName ] = true;
};

bool NBCatalogModel::isCatalogVisible( QString mCatalogName ) const {

	if ( rootItem->catalogList.contains( mCatalogName ) )
		return catalogVisibilityMap[ mCatalogName ];

	return false;
};

QModelIndexList NBCatalogModel::indexListForCatalog( QString mCatalogName ) const {

	QModelIndexList mList;

	if ( not rootItem->catalogList.contains( mCatalogName ) or mCatalogName.isEmpty() or mCatalogName.isNull() )
		return mList;

	foreach( int row, catalogRowMap[ mCatalogName ] ) {
		mList << index( row, 0, QModelIndex() );
	}

	return mList;
};

QStringList NBCatalogModel::nameFilters() const {

	return QStringList();
};

void NBCatalogModel::setNameFilters( QStringList filters ) {

	Q_UNUSED( filters );
};

void NBCatalogModel::clearNameFilters() {
};

void NBCatalogModel::reload() {

	setupModelData();
};

void NBCatalogModel::setupModelData() {

	emit modelLoading();

	beginResetModel();
	rootItem->clearChildren();

	QSettings catalogsSettings( "NewBreeze", "Catalogs" );
	foreach( QString ctlg, catalogsSettings.allKeys() ) {
		QStringList locations = catalogsSettings.value( ctlg ).toStringList();
		foreach( QString location, locations ) {
			QVariantList data;

			// Name
			data << baseName( location );

			// Target
			data << location;

			// Target exists
			data << exists( location );

			rootItem->addChild( new NBCatalogItem( data, ctlg, rootItem ) );
		}
	}
	endResetModel();

	emit modelLoaded();
	QTimer::singleShot( 0, this, SLOT( loadIcons() ) );

	catalogRowMap.clear();
	catalogVisibilityMap.clear();

	// Create a map of catalogIdndex versus rows
	foreach( NBCatalogItem *item, rootItem->children() )
		catalogRowMap[ item->catalogName() ] << item->row();

	foreach( QString mCatalogName, rootItem->catalogList )
		catalogVisibilityMap[ mCatalogName ] = true;
};

void NBCatalogModel::loadIcons() {

	QMimeType dirType = mimeDb.mimeTypeForFile( "/" );
	foreach( NBCatalogItem *item, rootItem->children() ) {
		QString icoStr = NBIconProvider::icon( item->data( NBCatalogModel::Target ).toString(), dirType );
		item->setIcon( QIcon::fromTheme( icoStr, QIcon( icoStr ) ) );

		QModelIndex idx = index( item->row(), 0, QModelIndex() );
		emit dataChanged( idx, idx );
	}
};
