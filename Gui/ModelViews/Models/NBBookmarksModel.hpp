/*
	*
	* NBBookmarksModel.hpp - NBBookmarksModel.cpp Header
	*
*/

#ifndef NBBOOKMARKSMODEL_HPP
#define NBBOOKMARKSMODEL_HPP

#include <Global.hpp>

class NBBookmarksModel : public QStandardItemModel {
	Q_OBJECT

	public:
		NBBookmarksModel();
		~NBBookmarksModel();

		QVariant data( const QModelIndex &index, int role ) const;
		QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
		int rowCount( const QModelIndex &parent = QModelIndex() ) const;

		int maxItemWidth() const;

		QList<BookMark> bookmarks();
		QString bookmarkPath( int );

	private:
		QList<BookMark> bookmarkList;
		QSocketNotifier *watcher;

		int itemWidth;

	public slots:
		void updateModelData();

	private slots:
		void saveChange( QStandardItem * );
};

#endif
