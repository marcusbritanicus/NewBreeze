/*
	*
	* NBBookmarksModel.cpp - BookMark Display Widget Class
	*
*/

#include <NBBookmarksModel.hpp>
#include <NBTools.hpp>

NBBookmarksModel::NBBookmarksModel() : QStandardItemModel() {

	itemWidth = 0;

	updateModelData();
	connect( this, SIGNAL( itemChanged( QStandardItem * ) ), this, SLOT( saveChange( QStandardItem * ) ) );
};

NBBookmarksModel::~NBBookmarksModel() {

	delete &bookmarkList;
}

QVariant NBBookmarksModel::data( const QModelIndex &index, int role ) const {

	int row = index.row();
	switch( role ) {
		case Qt::DisplayRole :
			return bookmarkList.at( row ).first;

		case Qt::DecorationRole :
			return QIcon::fromTheme( "bookmarks", QIcon( ":/icons/bookmarks.png" ) );

		case Qt::ToolTipRole :
			return bookmarkList.at( row ).second;

		case Qt::WhatsThisRole :
			return QString(
					"Click this to open the location %1"
				).arg( bookmarkList.at( row ).second );

		default:
			return QStandardItemModel::data( index, role );
	}
};

QModelIndex NBBookmarksModel::index( int row, int column, const QModelIndex &parent ) const {

	Q_UNUSED(parent );
	return item( row, column )->index();
};

int NBBookmarksModel::rowCount( const QModelIndex &parent ) const {

	Q_UNUSED( parent );
	return bookmarkList.count();
};

int NBBookmarksModel::maxItemWidth() const {

	return itemWidth;
};

QList<BookMark> NBBookmarksModel::bookmarks() {

	return bookmarkList;
};

QString NBBookmarksModel::bookmarkPath( int row ) {

	return bookmarkList.at( row ).second;
};

void NBBookmarksModel::updateModelData() {

	clear();
	bookmarkList.clear();

	QFontMetrics fm( qApp->font() );

	foreach( QString bookmark, bookmarkSettings.value( "Order" ).toStringList() ) {
		QString label = bookmarkSettings.value( QUrl::toPercentEncoding( bookmark ) ).toString();

		QStandardItem *item = new QStandardItem();
		item->setText( label );
		item->setIcon( QIcon::fromTheme( "bookmarks", QIcon( ":/icons/bookmarks.png" ) ) );
		item->setEditable( false );

		appendRow( item );
		bookmarkList << BookMark( label, bookmark );

		if ( itemWidth < fm.width( label ) + 30 )
			itemWidth = fm.width( label ) + 30;
	}
};

void NBBookmarksModel::saveChange( QStandardItem *item ) {

	Q_UNUSED( item );
};
