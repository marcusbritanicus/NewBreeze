/*
	*
	* NBSidePanelModel.hpp - NBSidePanelModel.cpp Header
	*
*/

#ifndef NBSIDEPANELMODEL_HPP
#define NBSIDEPANELMODEL_HPP

#include <Global.hpp>
#include <NBDeviceInfo.hpp>
#include <NBSidePanelItem.hpp>

class NBSidePanelModel : public QAbstractItemModel {
	Q_OBJECT

	public:
		NBSidePanelModel();
		~NBSidePanelModel();

		QVariant data( const QModelIndex &index, int role ) const;

		QModelIndex index( int row = 0, int column = 0, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex rootIndex() const;
		QModelIndex parent( const QModelIndex &index = QModelIndex() ) const;

		Qt::ItemFlags flags( const QModelIndex &) const;

		int rowCount( const QModelIndex &parent = QModelIndex() ) const;
		int columnCount( const QModelIndex &parent = QModelIndex() ) const;

		QList<NBDeviceInfo> deviceData();
		QList<BookMark> bookmarks();

	private:
		void prepareRootItems();

		QList<NBDeviceInfo> deviceInfos;
		QList<BookMark> bookmarkList;

		NBSidePanelItem  *rootItem, *devRootItem, *bmkRootItem;
		QStringList bmkList, devList;

	public slots:
		void updateDeviceData();
		void updateBookmarkData();

		void updateModelData();
};

#endif
