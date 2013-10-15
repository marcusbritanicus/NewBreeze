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

	delete &deviceInfos;
}

QVariant NBSidePanelModel::data( const QModelIndex &index, int role ) const {

	if ( not index.isValid() )
		return QVariant();

	if ( index.column() != 0 )
		return QVariant();

	NBSidePanelItem *item = static_cast<NBSidePanelItem*>( index.internalPointer() );
	switch( role ) {
		case Qt::ForegroundRole : {
			if ( ( Settings.General.Style == QString( "TransLight" ) ) or ( Settings.General.Style == QString( "LightGray" ) ) )
				return Qt::black;

			else
				return Qt::white;
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
		parentItem = (NBSidePanelItem *)parent.internalPointer();

	NBSidePanelItem *childItem = parentItem->child( row );
	if ( childItem )
		return createIndex( row, column, childItem );

	else
		return QModelIndex();
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

	if ( not parent.isValid() )
		return 0;

	NBSidePanelItem *item = ( NBSidePanelItem * )parent.internalPointer();
	return item->childCount();
};

int NBSidePanelModel::columnCount( const QModelIndex &parent ) const {

	if ( parent == index( 0, 0, QModelIndex() ) )
		return 1;

	if ( not parent.isValid() )
		return 0;

	return 1;
};

QList<NBDeviceInfo> NBSidePanelModel::deviceData() {

	return deviceInfos;
};

void NBSidePanelModel::prepareRootItems() {

	rootItem = new NBSidePanelItem( QVariantList() );
	devRootItem = new NBSidePanelItem( QVariantList() << "Devices" << QIcon() << "", rootItem );
	bmkRootItem = new NBSidePanelItem( QVariantList() << "Bookmarks" << QIcon() << "", rootItem );

	rootItem->appendChild( devRootItem );
	rootItem->appendChild( bmkRootItem );
};

void NBSidePanelModel::updateDeviceData() {

	devList.clear();
	devRootItem->clearChildren();

	NBDeviceManager dm;
	deviceInfos = dm.allDevices();

	foreach( NBDeviceInfo devInfo, deviceInfos ) {

		QVariantList data;
		QVariant var;
		var.setValue<NBDeviceInfo>( devInfo );

		data << devInfo.driveLabel();
		data << QIcon( ":/icons/" + devInfo.driveType() + ".png" );
		data << var;

		deviceInfos << devInfo;
		devRootItem->appendChild( new NBSidePanelItem( data, devRootItem ) );
	}
};

void NBSidePanelModel::updateBookmarkData() {

	bmkList.clear();
	bmkRootItem->clearChildren();

	foreach( QString bookmark, bookmarkSettings.value( "Order" ).toStringList() ) {
		QString label = bookmarkSettings.value( QUrl::toPercentEncoding( bookmark ) ).toString();

		QVariantList data;
		data << label;
		data << QIcon::fromTheme( "bookmarks", QIcon( ":/icons/bookmarks.png" ) );
		data << bookmark;

		bookmarkList << BookMark( label, bookmark );
		bmkRootItem->appendChild( new NBSidePanelItem( data, bmkRootItem ) );
	}
};

void NBSidePanelModel::updateModelData() {

	updateDeviceData();
	updateBookmarkData();
};
