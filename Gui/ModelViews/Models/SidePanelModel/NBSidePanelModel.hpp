/*
	*
	* NBSidePanelModel.hpp - NBSidePanelModel.cpp Header
	*
*/

#pragma once
#ifndef NBSIDEPANELMODEL_HPP
#define NBSIDEPANELMODEL_HPP

#include <Global.hpp>
#include <NBTools.hpp>
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

	private:
		void prepareRootItems();

		NBSidePanelItem *rootItem;
		QStringList bmkList, devList;

		bool showingDevices = true;

	public slots:
		void updateDeviceData();
		void updateBookmarkData();
};

#endif
