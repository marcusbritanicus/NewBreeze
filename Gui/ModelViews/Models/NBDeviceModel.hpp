/*
	*
	* NBDeviceModel.hpp - NBDeviceModel.cpp Header
	*
*/

#ifndef NBDEVICEMODEL_HPP
#define NBDEVICEMODEL_HPP

#include <Global.hpp>
#include <NBDeviceInfo.hpp>

class NBDeviceModel : public QStandardItemModel {
	Q_OBJECT

	public:
		NBDeviceModel();
		~NBDeviceModel();

		QVariant data( const QModelIndex &index, int role ) const;
		QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
		int rowCount( const QModelIndex &parent = QModelIndex() ) const;

		int maxItemWidth() const;

		QList<NBDeviceInfo> deviceData();
		NBDeviceInfo deviceData( int );

	private:
		QList<NBDeviceInfo> deviceInfos;
		QSocketNotifier *watcher;

		int itemWidth;

	private slots:
		void updateModelData();
};

#endif
