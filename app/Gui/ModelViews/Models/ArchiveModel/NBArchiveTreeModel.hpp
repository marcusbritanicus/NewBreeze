/*
	*
	* NBArchiveTreeModel.hpp - NewBreeze NBArchiveTreeModel Class Header
	*
*/

#pragma once

#include "NBArchiveBranch.hpp"
#include <libarchiveqt.h>

class NBArchiveTreeModel : public QAbstractItemModel {
    Q_OBJECT

	public:
		enum LocationType {
			SuperStart				= 0x6AF97E,			/* SuperStart */
			Applications,								/* Applications */
			Catalogs,									/* Catalogs */
			FileSystem									/* Real Locations */
		};

		enum Filters {
			Dirs,
			Files,
			NoSymLinks,
			AllEntries,
			Readable,
			Writable,
			Executable,
			Modified,
			Hidden,
			System,
		};

		NBArchiveTreeModel( QString path );
		~NBArchiveTreeModel();

		/* Children Info */
		int rowCount( const QModelIndex &parent = QModelIndex() ) const;
		int columnCount( const QModelIndex &parent = QModelIndex() ) const;

		Qt::ItemFlags flags( const QModelIndex & ) const;

		/* Display Info */
		QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
		QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

		/* Node Info */
		QModelIndex index( int row = 0, int column = 0, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex index( QString, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex parent( const QModelIndex &index = QModelIndex() ) const;

		/* Children, fetchMore */
		bool hasBranches( const QModelIndex &index = QModelIndex() ) const;

		Qt::ItemFlags flags( const QModelIndex index ) const;

		/* Filters */
		QStringList nameFilters();
		void setNameFilters( QStringList );
		void clearNameFilters();

		/* Model reload */
		void reload();

		/* FS Navigation */
		QString nodeName( const QModelIndex ) const;
		QString nodePath( const QModelIndex ) const;
		QString nodePath( QString ) const;
		QFileInfo nodeInfo( const QModelIndex ) const;

		bool extract( QString ) const;
		bool extractAll() const;

		/* To halt the Info Gathering */
		void terminateInfoGathering();

	private:
		/* Grow the tree */
		void growTree();

		NBArchiveBranch *tree;

		QString archiveName;
		LibArchiveQt *archive;

		mutable struct loadStatus {
			bool loading;
			bool stopLoading;
		} currentLoadStatus;

		QStringList __nameFilters;

		/* Info Gatherer kill switch */
		bool __terminate;

	Q_SIGNALS:
		/* Updated Node */
		void nodeUpdated( QString );

		void directoryLoading( QString );
		void directoryLoaded( QString );
		void runningHome( QString );
};
