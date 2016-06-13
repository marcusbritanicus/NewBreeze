/*
	*
	* NBNBTrashModel.hpp - NewBreeze NBTrashModel Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBTrashNode.hpp"

class NBTrashModel : public QAbstractItemModel {
    Q_OBJECT

	public:
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

		NBTrashModel();
		~NBTrashModel();

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
		bool setData( const QModelIndex &index, QString value, int role );

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

		void reload();

		// Restore/Delete
		void restore( QModelIndexList );
		void removeFromDisk( QModelIndexList );

	private:
		void setupModelData();
		QString getCategory( QString );
		void recategorize();

		NBTrashNode *rootNode;

		QString __rootPath;
		QStringList __childNames;

		QHash<QString, QList<int> > categoryRowMap;
		QHash<QString, bool> categoryVisibilityMap;

		bool mCategorizationEnabled;

		struct loadStatus {
			bool loading;
			bool stopLoading;
		} currentLoadStatus;

		QList<Filters> __currentFilters;
		QStringList __nameFilters;

		// History
		QStringList oldRoots;
		long curIndex;
		mutable int updatedNodes;

	public slots:
		void sort();

	private slots:
		void handleNodeCreated( QString );
		void handleNodeChanged( QString );
		void handleNodeDeleted( QString );

		/* Perform the sorting again on a signal */
		/* Just does sort( prevSort.column, prevSort.cs, prevSort.categorized ) */

	Q_SIGNALS:
		void trashLoading();
		void trashLoaded();

		void restored( QModelIndexList );
		void deleted( QModelIndexList );
};
