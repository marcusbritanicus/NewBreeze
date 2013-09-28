/*
	*
	* NBDeviceModel.cpp - BookMark Display Widget Class
	*
*/

#include <NBDeviceModel.hpp>

NBDeviceModel::NBDeviceModel() : QStandardItemModel() {

	itemWidth = 0;
	updateModelData();

	int mountsFD = open( "/proc/self/mounts", O_RDONLY, 0 );
	watcher = new QSocketNotifier( mountsFD, QSocketNotifier::Write );
	connect( watcher, SIGNAL( activated( int ) ), this, SLOT( updateModelData() ) );
};

NBDeviceModel::~NBDeviceModel() {

	delete &deviceInfos;
}

QVariant NBDeviceModel::data( const QModelIndex &index, int role ) const {

	int row = index.row();
	switch( role ) {
		case Qt::DisplayRole :
			return deviceInfos.at( row ).driveLabel();

		case Qt::DecorationRole :
			return QIcon( ":/icons/" + deviceInfos.at( row ).driveType() + ".png" );

		case Qt::ToolTipRole :
			return deviceInfos.at( row ).mountPoint();

		case Qt::WhatsThisRole :
			return QString(
					"Click this to open the partition %1 mounted on %2"
				).arg( deviceInfos.at( row ).driveName() ).arg( deviceInfos.at( row ).mountPoint() );

		default:
			return QStandardItemModel::data( index, role );
	}
};

QModelIndex NBDeviceModel::index( int row, int column, const QModelIndex &parent ) const {

	Q_UNUSED(parent );
	return item( row, column )->index();
};

int NBDeviceModel::rowCount( const QModelIndex &parent ) const {

	Q_UNUSED( parent );
	return deviceInfos.count();
};

int NBDeviceModel::maxItemWidth() const {

	return itemWidth;
};

QList<NBDeviceInfo> NBDeviceModel::deviceData() {

	return deviceInfos;
};

NBDeviceInfo NBDeviceModel::deviceData( int row ) {

	return deviceInfos.at( row );
};

void NBDeviceModel::updateModelData() {

	clear();

	QFontMetrics fm( qApp->font() );

	NBDeviceManager dm;
	deviceInfos = dm.allDevices();

	foreach( NBDeviceInfo devInfo, deviceInfos ) {

		QStandardItem *item = new QStandardItem();
		item->setText( devInfo.driveLabel() );
		item->setIcon( QIcon( ":/icons/" + devInfo.driveType() + ".png" ) );
		item->setDropEnabled( true );
		item->setEditable( false );

		appendRow( item );

		if ( itemWidth < fm.width( devInfo.driveLabel() ) + 30 )
			itemWidth = fm.width( devInfo.driveLabel() ) + 30;
	}
};
