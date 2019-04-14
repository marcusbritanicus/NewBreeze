/*
	*
	* NBNBItemViewModel.hpp - NewBreeze NBItemViewModel Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBIconManager.hpp"
#include "NBItemViewNode.hpp"
#include "NBFileInfoGatherer.hpp"
#include "NBFileSystemWatcher.hpp"
#include "NBThumbnailer.hpp"
#include "NBProcessManager.hpp"

class NBItemViewModel : public QAbstractItemModel {
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

		NBItemViewModel( QObject *parent = 0 );
		~NBItemViewModel();

		/* Categorization Info */
		bool isCategorizationEnabled();
		void setCategorizationEnabled( bool );

		/* Children Info */
		int rowCount( const QModelIndex &parent = QModelIndex() ) const;
		int rowCount( const QString category ) const;
		int categoryCount() const;

		int columnCount( const QModelIndex &parent = QModelIndex() ) const;

		Qt::ItemFlags flags( const QModelIndex &) const;

		/* Display Info */
		QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
		QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
		bool setData( const QModelIndex &index, QVariant value, int role );

		/* Are we showing real or virtual data */
		bool isRealLocation() const;
		quint64 modelDataType() const;

		/* Data editing */
		bool insertNode( QString name );
		bool removeNode( QString name );
		void updateNode( QString name );

		/* Node Info */
		QModelIndex index( int row = 0, int column = 0, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex index( QString, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex parent( const QModelIndex &index = QModelIndex() ) const;

		QString category( const QModelIndex &index = QModelIndex() ) const;
		int categoryIndex( const QModelIndex &index = QModelIndex() ) const;
		QStringList categories() const;
		QPixmap pixmapForCategory( QString, bool folded = false ) const;

		int indexListCountForCategory( QString ) const;
		QModelIndexList indexListForCategory( QString ) const;
		int indexListCountForCategory( int ) const;
		QModelIndexList indexListForCategory( int ) const;
		int indexInCategory( QModelIndex ) const;
		QModelIndexList categorySiblings( QModelIndex ) const;

		bool showHidden() const;
		void setShowHidden( bool );

		/* Drag and Drop */
		Qt::DropActions supportedDropActions() const;
		Qt::DropActions supportedDragActions() const;
		Qt::ItemFlags flags( const QModelIndex index ) const;
		QStringList mimeTypes() const;

		/* Filters */
		bool filter( Filters filter );
		void setFilter( Filters filter, bool );
		QStringList nameFilters();
		void setNameFilters( QStringList );
		void clearNameFilters();

		void sort( int column, bool cs, bool categorized );
		void reload();

		/* FS Modification */
		bool rename( QString, QString );

		/* FS Navigation */
		QString nodeName( const QModelIndex ) const;
		QString nodePath( const QModelIndex ) const;
		QString nodePath( QString ) const;
		QFileInfo nodeInfo( const QModelIndex ) const;

		QString rootPath() const;
		void setRootPath( QString );

		bool canGoUp() const;
		bool canGoBack() const;
		bool canGoForward() const;

		QString currentDir() const;
		QString previousDir() const;
		QString nextDir() const;
		QString parentDir() const;
		QString lastOpenedFolder() const;

		/* Place to store the image thumbnails */
		static QHash<QString, QIcon> iconMap;

	public Q_SLOTS:
		void goUp();
		void goBack();
		void goForward();
		void goHome();

	private:
		/* Initiate setting up of model */
		void setupModelData();

		/* Model Data for SuperStart */
		void setupSuperStartData();

		/* Model Data for applications */
		void setupApplicationsData();

		/* Model Data for catalogs */
		void setupCatalogData();

		/* Model Data for real locations */
		void setupFileSystemData();

		/* New watch */
		void newWatch( QString );

		/* Category tools */
		QString getCategory( QVariantList );
		void recategorize();

		/* Thumbnailer */
		NBThumbnailer *thumbnailer;

		NBItemViewNode *rootNode;

		QString mRootPath;
		QStringList mChildNames;
		QStringList mChildPaths;

		QHash<QString, QList<int> > categoryRowMap;
		QHash<QString, bool> categoryVisibilityMap;
		QHash<QString, QString> mCategoryIconMap;

		bool mCategorizationEnabled;

		struct __prevSort {
			int column;
			bool cs;
			bool categorized;
		} prevSort;

		struct loadStatus {
			bool loading;
			bool stopLoading;
		} currentLoadStatus;

		QStringList mNameFilters;

		/* Showing virtual data */
		bool mVirtualData;

		/* Showing which data type */
		quint64 mModelDataType;

		/* History */
		QStringList oldRoots;
		long curIndex;
		QString prevFolder;

		/* To manage the rapidly changing nodes */
		mutable QStringList lastUpdatedNodes;
		mutable QList<QTime> lastUpdatedTimes;

		NBQuickFileInfoGatherer *quickDataGatherer;
		NBFileSystemWatcher *watcher;

	private slots:
		void handleNodeCreated( QString );
		void handleNodeChanged( QString );
		void handleNodeDeleted( QString );
		void handleNodeRenamed( QString, QString );
		void loadHome();

		void nodeUpdated( QString );

		void deleteComplete( NBProcess::Progress*, NBAbstractProcess* );

	Q_SIGNALS:
		void directoryLoading( QString );
		void directoryLoaded( QString );

		void runningHome( QString );
};
