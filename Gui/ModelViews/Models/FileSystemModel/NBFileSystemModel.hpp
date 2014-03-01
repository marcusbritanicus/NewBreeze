/*
	*
	* NBNBFileSystemModel.hpp - NewBreeze NBFileSystemModel Class Header
	*
*/

#pragma once
#ifndef NBFILESYSTEMMODEL_HPP
#define NBFILESYSTEMMODEL_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBFileSystemNode.hpp>
#include <NBFileInfoGatherer.hpp>
#include <NBFileSystemWatcher.hpp>

class NBFileSystemModel : public QAbstractItemModel {
    Q_OBJECT

	public:
		enum Sections {
			NAME = 3,
			SIZE = 4,
			TYPE = 5,
			MIME = 6,
			TIME = 7,
			PERM = 8,
			OWNR = 9
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

		NBFileSystemModel();
		~NBFileSystemModel();

		// Categorization Info
		bool isCategorizationEnabled();
		void setCategorizationEnabled( bool );

		// Children Info
		int rowCount( const QModelIndex &parent = QModelIndex() ) const;
		int rowCount( const QString mCategory ) const;
		int categoryCount() const;

		int columnCount( const QModelIndex &parent = QModelIndex() ) const;

		Qt::ItemFlags flags( const QModelIndex &) const;

		// Display Info
		QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
		QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
		bool setData( const QModelIndex &index, QVariant value, int role );

		// Data editing
		bool insertNode( QString name );
		bool removeNode( QString name );
		void updateNode( QString name );

		// Node Info
		QModelIndex index( int row = 0, int column = 0, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex index( QString, const QModelIndex &parent = QModelIndex() ) const;
		QModelIndex parent( const QModelIndex &index = QModelIndex() ) const;

		QString category( const QModelIndex &index = QModelIndex() ) const;
		int categoryIndex( const QModelIndex &index = QModelIndex() ) const;
		QStringList categories() const;

		// Show Hide Categories
		void hideCategory( QString category );
		void showCategory( QString category );
		bool isCategoryVisible( QString mCategory ) const;

		int indexListCountForCategory( QString ) const;
		QModelIndexList indexListForCategory( QString ) const;

		bool showHidden() const;
		void setShowHidden( bool );

		bool readOnly() const;
		void setReadOnly( bool );

		// Drag and Drop
		Qt::DropActions supportedDropActions() const;
		Qt::DropActions supportedDragActions() const;
		Qt::ItemFlags flags( const QModelIndex index ) const;
		QStringList mimeTypes() const;

		// Filters
		bool filter( Filters filter );
		void setFilter( Filters filter, bool );
		QStringList nameFilters();
		void setNameFilters( QStringList );
		void clearNameFilters();

		void sort( int column, bool cs, bool categorized );
		int sortColumn() const;
		bool sortCaseSensitivity() const;
		bool sortCategorized() const;
		void reload();

		// FS Modification
		bool mkdir( QString );
		bool remove( QString );
		bool copy( QString, QString );
		bool move( QString, QString );
		bool rename( QString, QString );
		bool chmod( QString, int );

		// FS Navigation
		QString nodeName( const QModelIndex ) const;
		QString nodePath( const QModelIndex ) const;
		QString nodePath( QString ) const;
		QFileInfo nodeInfo( const QModelIndex ) const;

		QString rootPath() const;
		void setRootPath( QString );

		void goUp();
		void goBack();
		void goForward();
		void goHome();

		bool canGoUp() const;
		bool canGoBack() const;
		bool canGoForward() const;

		QString currentDir() const;
		QString previousDir() const;
		QString nextDir() const;
		QString parentDir() const;

	private:
		void setupModelData();

		QString getCategory( QVariantList );
		void recategorize();

		NBFileSystemNode *rootNode;

		QString __rootPath;
		QStringList __childNames;

		QHash<QString, QList<int> > categoryRowMap;
		QHash<QString, bool> categoryVisibilityMap;

		bool mCategorizationEnabled = false;

		struct __prevSort {
			int column = Settings->Session.SortColumn;
			bool cs = Settings->Session.SortCase;
			bool categorized = Settings->Session.SortCategory;
		} prevSort;

		struct loadStatus {
			bool loading = false;
			bool stopLoading = false;
		} curentLoadStatus;

		bool __showHidden;
		bool __readOnly;

		QList<Filters> __currentFilters;
		QStringList __nameFilters;

		// History
		QStringList oldRoots;
		long curIndex;
		mutable int updatedNodes = 0;

		NBQuickFileInfoGatherer *quickDataGatherer;
		NBFileSystemWatcher *watcher;

	private slots:
		void gatherFileInfo();
		void saveInfo( QString, QString, QStringList );

		void handleNodeCreated( QString );
		void handleNodeChanged( QString );
		void handleNodeDeleted( QString );
		void loadHome();

		/* Perform the sorting again on a signal */
		/* Just does sort( prevSort.column, prevSort.cs, prevSort.categorized ) */
		void sort();

	Q_SIGNALS:
		void loadFileInfo();
		void dirLoading( QString );
		void directoryLoaded( QString );
		void runningHome( QString );
		void updatedAllNodes();

		// Experimental
		void loadApplications();
		void loadCatalogs();
		void loadFolders();
};

#endif
