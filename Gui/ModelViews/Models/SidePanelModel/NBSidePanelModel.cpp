/*
	*
	* NBSidePanelModel.cpp - BookMark Display Widget Class
	*
*/

#include <NBSidePanelModel.hpp>

NBSidePanelModel::NBSidePanelModel() : QAbstractItemModel() {

	rootItem = new NBSidePanelItem( QVariantList() );
	updateDeviceData();
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
				return QColor( Settings->Colors.TextPenColor );

			else
				return QColor( Settings->Colors.TextPenColorAlt );
		}

		case Qt::ToolTipRole : {
			if ( showingDevices )
				return item->data( Qt::UserRole + 1 ).value<NBDeviceInfo>().mountPoint();

			else
				return item->data( Qt::UserRole + 1 );
		}

		case Qt::UserRole + 1 : {
			if ( showingDevices )
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

	return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
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

void NBSidePanelModel::updateDeviceData() {

	NBDeviceManager dm;
	QList<NBDeviceInfo> deviceInfos = dm.allDevices();

	showingDevices = true;
	beginResetModel();

	devList.clear();
	bmkList.clear();

	rootItem->clearChildren();
	foreach( NBDeviceInfo devInfo, deviceInfos ) {

		QVariantList data;
		QVariant var;
		var.setValue<NBDeviceInfo>( devInfo );

		data << devInfo.driveLabel();
		data << QIcon( ":/icons/" + devInfo.driveType() + ".png" );
		data << var;

		rootItem->appendChild( new NBSidePanelItem( data, rootItem ) );
	}
	endResetModel();
};

void NBSidePanelModel::updateBookmarkData() {

	showingDevices = false;
	beginResetModel();

	devList.clear();
	bmkList.clear();

	rootItem->clearChildren();
	foreach( QString bookmark, bookmarkSettings.value( "Order" ).toStringList() ) {
		if ( not exists( bookmark ) )
			continue;

		QString label = bookmarkSettings.value( QUrl::toPercentEncoding( bookmark ) ).toString();

		QVariantList data;
		data << label;
		data << QIcon::fromTheme( "bookmarks", QIcon( ":/icons/bookmarks.png" ) );
		data << bookmark;

		rootItem->appendChild( new NBSidePanelItem( data, rootItem ) );
	}
	endResetModel();
};
