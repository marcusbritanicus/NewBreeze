/*
	*
	* NBSidePanelModel.cpp - BookMark Display Widget Class
	*
*/

#include <NBSidePanelModel.hpp>

NBSidePanelModel::NBSidePanelModel() : QAbstractItemModel() {

	prepareRootItems();
	updateModelData();
};

NBSidePanelModel::~NBSidePanelModel() {
}

QVariant NBSidePanelModel::data( const QModelIndex &index, int role ) const {

	if ( not index.isValid() )
		return QVariant();

	if ( index.column() != 0 )
		return QVariant();

	NBSidePanelItem *item = static_cast<NBSidePanelItem*>( index.internalPointer() );
	switch( role ) {
		case Qt::ForegroundRole : {
			if ( ( Settings->General.Style == QString( "TransLight" ) ) or ( Settings->General.Style == QString( "LightGray" ) ) )
				return QColor( Qt::black );

			else
				return QColor( Qt::white );
		}

		case Qt::ToolTipRole : {
			if ( index.parent().data() == "Devices" )
				return item->data( Qt::UserRole + 1 ).value<NBDeviceInfo>().mountPoint();

			else
				return item->data( Qt::UserRole + 1 );
		}

		case Qt::UserRole + 1 : {
			if ( index.parent().data() == "Devices" )
				return item->data( Qt::UserRole + 1 ).value<NBDeviceInfo>().mountPoint();

			else
				return item->data( Qt::UserRole + 1 );
		}

		default :
			return item->data( role );
	}
};

QModelIndex NBSidePanelModel::index( int row, int column, const QModelIndex &parent ) const {

	NBSidePanelItem *parentItem;
	if ( not parent.isValid() )
		parentItem = rootItem;

	else
		parentItem = ( NBSidePanelItem * )parent.internalPointer();

	NBSidePanelItem *childItem = parentItem->child( row );
	if ( childItem )
		return createIndex( row, column, childItem );

	else
		return QModelIndex();
};

Qt::ItemFlags NBSidePanelModel::flags( const QModelIndex & index ) const {

	if ( not index.isValid() )
		return Qt::NoItemFlags;

	if ( index.parent() == parent() )
		switch( index.row() ) {
			case 0:
			case 1:
				return Qt::NoItemFlags;

			case 2:
				return Qt::ItemIsEnabled;

			case 3:
				return Qt::ItemIsEnabled;
		}

	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;

	return Qt::NoItemFlags;
};

QModelIndex NBSidePanelModel::rootIndex() const {

	return createIndex( 0, 0, rootItem );
};

QModelIndex NBSidePanelModel::parent( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QModelIndex();

	NBSidePanelItem *childItem = ( NBSidePanelItem * )index.internalPointer();
	NBSidePanelItem *parentItem = childItem->parent();

	if ( parentItem == rootItem )
		return QModelIndex();

	return createIndex( parentItem->row(), 0, parentItem );
};

int NBSidePanelModel::rowCount( const QModelIndex &parent ) const {

	if ( parent == QModelIndex() ) {
		return rootItem->childCount();
	}

	NBSidePanelItem *item = ( NBSidePanelItem * )parent.internalPointer();
	return item->childCount();
};

int NBSidePanelModel::columnCount( const QModelIndex &parent ) const {

	if ( parent == QModelIndex() )
		return 1;

	if ( not parent.isValid() )
		return 0;

	return 1;
};

void NBSidePanelModel::prepareRootItems() {

	rootItem = new NBSidePanelItem( QVariantList() );

	folderRootItem = new NBSidePanelItem( QVariantList() << "Folders" << QIcon( ":/icons/folder.png" ) << "NB://FolderView", rootItem );
	appRootItem = new NBSidePanelItem( QVariantList() << "Applications" << QIcon( ":/icons/applications.png" ) << "NB://Applications", rootItem );
	catalogRootItem = new NBSidePanelItem( QVariantList() << "Catalogs" << QIcon( ":/icons/catalogs.png" ) << "NB://Catalogs", rootItem );

	devRootItem = new NBSidePanelItem( QVariantList() << "Devices" << QIcon( ":/icons/comp.png" ) << "", rootItem );
	bmkRootItem = new NBSidePanelItem( QVariantList() << "Favorites" << QIcon( ":/icons/bookmark.png" ) << "", rootItem );

	rootItem->appendChild( folderRootItem );
	rootItem->appendChild( appRootItem );
	rootItem->appendChild( catalogRootItem );

	rootItem->appendChild( devRootItem );
	rootItem->appendChild( bmkRootItem );
};

void NBSidePanelModel::updateDeviceData() {

	NBDeviceManager dm;
	QList<NBDeviceInfo> deviceInfos = dm.allDevices();

	beginResetModel();

	devList.clear();
	devRootItem->clearChildren();
	foreach( NBDeviceInfo devInfo, deviceInfos ) {

		QVariantList data;
		QVariant var;
		var.setValue<NBDeviceInfo>( devInfo );

		data << devInfo.driveLabel();
		data << QIcon( ":/icons/" + devInfo.driveType() + ".png" );
		data << var;

		devRootItem->appendChild( new NBSidePanelItem( data, devRootItem ) );
	}
	endResetModel();
};

void NBSidePanelModel::updateBookmarkData() {

	beginResetModel();

	bmkList.clear();
	bmkRootItem->clearChildren();
	foreach( QString bookmark, bookmarkSettings.value( "Order" ).toStringList() ) {
		QString label = bookmarkSettings.value( QUrl::toPercentEncoding( bookmark ) ).toString();

		QVariantList data;
		data << label;
		data << QIcon::fromTheme( "bookmarks", QIcon( ":/icons/bookmarks.png" ) );
		data << bookmark;

		bmkRootItem->appendChild( new NBSidePanelItem( data, bmkRootItem ) );
	}
	endResetModel();
};

void NBSidePanelModel::updateModelData() {

	updateDeviceData();
	updateBookmarkData();
};
