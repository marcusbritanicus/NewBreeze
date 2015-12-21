/*
	*
	* NBCatalogModel.hpp - NewBreeze NBCatalogModel Class Header
	*
*/

#pragma once
#ifndef NBCATALOGMODEL_HPP
#define NBCATALOGMODEL_HPP

#include <Global.hpp>
#include <NBCatalogItem.hpp>
#include <NBIconProvider.hpp>
#include <NBTools.hpp>

class NBCatalogModel : public QAbstractItemModel {
    Q_OBJECT

	public:
		enum Lookup {
			Name = Qt::UserRole + 1,
			Target,
			Exists
		};
		NBCatalogModel();

		// Children Info
		int rowCount( const QModelIndex &parent = QModelIndex() ) const;
		int rowCount( const QString mCatalogName ) const;
		int catalogCount() const;

		int columnCount( const QModelIndex &parent = QModelIndex() ) const;

		Qt::ItemFlags flags( const QModelIndex & ) const;

		// Display Info
		QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

		// Node Info
		QModelIndex index( int row = 0, int column = 0, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex index( QString, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex parent( const QModelIndex &index = QModelIndex() ) const;

		QString catalogName( const QModelIndex &index = QModelIndex() ) const;
		int catalogIndex( const QModelIndex &index = QModelIndex() ) const;
		QStringList catalogs() const;

		// Show Hide Categories
		void hideCatalog( QString mCatalogName );
		void showCatalog( QString mCatalogName );
		bool isCatalogVisible( QString mCatalogName ) const;

		int indexListCountForCatalog( QString ) const;
		QModelIndexList indexListForCatalog( QString ) const;

		// Filters
		QStringList nameFilters() const;
		void setNameFilters( QStringList );
		void clearNameFilters();

		void reload();

	private:
		void setupModelData();

		NBCatalogItem *rootItem;

		QHash<QString, QList<int> > catalogRowMap;
		QHash<QString, bool> catalogVisibilityMap;

	private slots:
		void loadIcons();

	Q_SIGNALS:
		void modelLoading();
		void modelLoaded();
};

#endif
