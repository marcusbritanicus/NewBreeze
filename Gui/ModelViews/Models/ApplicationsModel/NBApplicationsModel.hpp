/*
	*
	* NBApplicationsModel.hpp - NewBreeze NBApplicationsModel Class Header
	*
*/

#ifndef NBAPPLICATIONSMODEL_HPP
#define NBAPPLICATIONSMODEL_HPP

#include <Global.hpp>
#include <NBApplicationItem.hpp>
#include <NBIconProvider.hpp>
#include <NBAppEngine.hpp>

class NBApplicationsModel : public QAbstractItemModel {
    Q_OBJECT

	public:
		enum Lookup {
			Name = Qt::UserRole + 1,
			Comment,
			ExecList,
			Icon,
			WorkPath,
			MimeList,
			Terminal,
			Categories
		};
		NBApplicationsModel();

		// Children Info
		int rowCount( const QModelIndex &parent = QModelIndex() ) const;
		int rowCount( const QString mCategory ) const;
		int categoryCount() const;

		int columnCount( const QModelIndex &parent = QModelIndex() ) const;

		Qt::ItemFlags flags( const QModelIndex & ) const;

		// Display Info
		QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

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

		QModelIndexList indexListForCategory( QString ) const;

		// Filters
		QStringList nameFilters() const;
		void setNameFilters( QStringList );
		void clearNameFilters();

		void reload();

	private:
		void setupModelData();

		NBAppEngine *appEngine;
		NBApplicationItem *rootItem;

		QHash<QString, QList<int>> categoryRowMap;
		QHash<QString, bool> categoryVisibilityMap;

		struct loadStatus {
			bool loading = false;
			bool stopLoading = false;
		} curentLoadStatus;

	private slots:
		void loadIcons();

	Q_SIGNALS:
		void modelLoading();
		void modelLoaded();
};

#endif
