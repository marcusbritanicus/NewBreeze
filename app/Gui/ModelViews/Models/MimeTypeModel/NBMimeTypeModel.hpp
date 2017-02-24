/*
	*
	* NBNBMimeTypeModel.hpp - NewBreeze NBMimeTypeModel Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBIconManager.hpp"
#include "NBMimeTypeNode.hpp"
#include "NBFileInfoGatherer.hpp"
#include "NBFileSystemWatcher.hpp"

class NBMimeIconUpdater : public QThread {
	Q_OBJECT

		public:
		NBMimeIconUpdater( QString, QStringList, bool *term );
		~NBMimeIconUpdater();

	private:
		void run();

		QStringList entryList;
		QString rootPath;

		bool *mTerminate;

	signals:
		void updated( QString, QString, QStringList );
};

class NBMimeTypeModel : public QAbstractItemModel {
    Q_OBJECT

	public:
		NBMimeTypeModel();
		~NBMimeTypeModel();

		/* Categorization Info */
		bool isCategorizationEnabled();

		/* Children Info */
		int rowCount( const QModelIndex &parent = QModelIndex() ) const;
		int rowCount( const QString category ) const;
		int categoryCount() const;

		int columnCount( const QModelIndex &parent = QModelIndex() ) const;

		Qt::ItemFlags flags( const QModelIndex &) const;

		/* Display Info */
		QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

		/* Node Info */
		QModelIndex index( int row = 0, int column = 0, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex index( QString, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex parent( const QModelIndex &index = QModelIndex() ) const;

		QString category( const QModelIndex &index = QModelIndex() ) const;
		int categoryIndex( const QModelIndex &index = QModelIndex() ) const;
		QStringList categories() const;
		QPixmap pixmapForCategory( QString ) const;

		void foldCategory( QString );
		void openCategory( QString );
		bool isCategoryOpen( QString ) const;

		int indexListCountForCategory( QString ) const;
		QModelIndexList indexListForCategory( QString ) const;
		QModelIndexList categorySiblings( QModelIndex ) const;

		/* Filters */
		QStringList nameFilters();
		void setNameFilters( QStringList );
		void clearNameFilters();

		void sort();
		void reload();

		/* To halt the Info Gathering */
		void terminateInfoGathering();

	private:
		/* Initiate setting up of model */
		void setupModelData();

		NBMimeTypeNode *rootNode;
		QStringList mChildNames;
		QStringList mCategoryList;

		QHash<QString, QList<int> > categoryRowMap;
		QHash<QString, bool> categoryVisibilityMap;

		struct loadStatus {
			bool loading;
			bool stopLoading;
		} currentLoadStatus;

		QStringList __nameFilters;

		/* Info Gatherer kill switch */
		bool mTerminate;

	private slots:
		void updateAllNodes();
		void saveInfo( QString, QString, QStringList );

	Q_SIGNALS:
		void modelLoaded();
};
