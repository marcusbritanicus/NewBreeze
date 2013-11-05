/*
	*
	* NBApplicationsModel.cpp - NewBreeze NBApplicationsModel Class
	*
*/

#include <NBApplicationsModel.hpp>

inline bool matchesFilter( QStringList filters, QString text ) {

	foreach( QString filter, filters )
		if ( text.contains( QRegExp( filter, Qt::CaseInsensitive, QRegExp::Wildcard ) ) )
			return true;

	return false;
};

NBApplicationsModel::NBApplicationsModel() : QAbstractItemModel() {

	appEngine = NBAppEngine::instance();
	rootItem = new NBApplicationItem();

	setupModelData();
};

int NBApplicationsModel::rowCount( const QModelIndex &parent ) const {

	if ( not parent.isValid() )
		return rootItem->childCount();

	else
		return static_cast<NBApplicationItem*>( parent.internalPointer() )->childCount();

	return 0;
};

int NBApplicationsModel::rowCount( QString mCategory ) const {

	if ( not rootItem->categoryList.contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return 0;

	return categoryRowMap[ mCategory ].count();
};

int NBApplicationsModel::categoryCount() const {

	return rootItem->categoryCount();
};

int NBApplicationsModel::columnCount( const QModelIndex & parent ) const {

	Q_UNUSED( parent );

	return 1;
};

Qt::ItemFlags NBApplicationsModel::flags( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
};

QVariant NBApplicationsModel::data( const QModelIndex &index, int role ) const {

	NBApplicationItem *item = static_cast<NBApplicationItem*>( index.internalPointer() );
	switch( role ) {
		case Qt::DecorationRole :	return item->icon();
		case Qt::ToolTipRole    :	return item->toolTip();
		default                 :	return item->data( role );
	};
};

QModelIndex NBApplicationsModel::index( int row, int, const QModelIndex &parent ) const {

	if ( not hasIndex( row, 0, parent ) )
		return QModelIndex();

	NBApplicationItem *parentItem;
	if ( not parent.isValid() )
		parentItem = rootItem;

	else
		parentItem = (NBApplicationItem *)parent.internalPointer();

	NBApplicationItem *childItem = parentItem->child( row );
	if ( childItem )
		return createIndex( row, 0, childItem );

	else
		return QModelIndex();
};

QModelIndex NBApplicationsModel::index( QString name, const QModelIndex &parent ) const {

	NBApplicationItem *parentItem;
	if ( not parent.isValid() )
		parentItem = rootItem;

	else
		parentItem = (NBApplicationItem *)parent.internalPointer();

	int row = parentItem->child( name )->row();
	NBApplicationItem *childItem = parentItem->child( name );

	if ( childItem )
		return createIndex( row, 0, childItem );

	return QModelIndex();
};

QModelIndex NBApplicationsModel::parent( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QModelIndex();

	NBApplicationItem *childItem = ( NBApplicationItem * )index.internalPointer();
	NBApplicationItem *parentItem = childItem->parent();

	if ( parentItem == rootItem )
		return QModelIndex();

	return createIndex( parentItem->row(), 0, parentItem );
};

QString NBApplicationsModel::category( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return QString();

	NBApplicationItem *childItem = ( NBApplicationItem * )index.internalPointer();
	return childItem->category();
};

int NBApplicationsModel::categoryIndex( const QModelIndex &index ) const {

	if ( not index.isValid() )
		return -1;

	NBApplicationItem *childItem = ( NBApplicationItem * )index.internalPointer();
	return childItem->categoryIndex();
};

QStringList NBApplicationsModel::categories() const {

	return rootItem->categoryList;
};

void NBApplicationsModel::hideCategory( QString mCategory ) {

	if ( rootItem->categoryList.contains( mCategory ) )
		categoryVisibilityMap[ mCategory ] = false;
};

void NBApplicationsModel::showCategory( QString mCategory ) {

	if ( rootItem->categoryList.contains( mCategory ) )
		categoryVisibilityMap[ mCategory ] = true;
};

bool NBApplicationsModel::isCategoryVisible( QString mCategory ) const {

	if ( rootItem->categoryList.contains( mCategory ) )
		return categoryVisibilityMap[ mCategory ];

	return false;
};

QModelIndexList NBApplicationsModel::indexListForCategory( QString mCategory ) const {

	QModelIndexList mList;

	if ( not rootItem->categoryList.contains( mCategory ) or mCategory.isEmpty() or mCategory.isNull() )
		return mList;

	foreach( int row, categoryRowMap[ mCategory ] ) {
		mList << index( row, 0, QModelIndex() );
	}

	return mList;
};

QStringList NBApplicationsModel::nameFilters() const {

	return QStringList();
};

void NBApplicationsModel::setNameFilters( QStringList filters ) {

	Q_UNUSED( filters );
};

void NBApplicationsModel::clearNameFilters() {
};

void NBApplicationsModel::reload() {

	setupModelData();
};

void NBApplicationsModel::setupModelData() {

	emit modelLoading();

	beginResetModel();
	foreach( NBAppFile app, appEngine->allDesktops().toQList() ) {
		if ( ( app.value( NBAppFile::Type ) != "Application" ) or ( app.value( NBAppFile::NoDisplay ).toBool() ) )
			continue;

		QVariantList data;

		data << app.value( NBAppFile::Name ).toString();
		data << app.value( NBAppFile::Comment ).toString();
		data << app.execArgs();
		data << app.value( NBAppFile::Icon ).toString();
		data << app.value( NBAppFile::WorkPath ).toString();
		data << app.value( NBAppFile::MimeTypes ).toStringList();
		data << app.value( NBAppFile::TerminalMode ).toBool();
		data << app.value( NBAppFile::Categories ).toStringList();

		rootItem->addChild( new NBApplicationItem( data, app.category(), rootItem ) );
	}
	endResetModel();

	emit modelLoaded();
	QTimer::singleShot( 0, this, SLOT( loadIcons() ) );

	// Create a map of categoryIdndex versus rows
	foreach( NBApplicationItem *item, rootItem->children() )
		categoryRowMap[ item->category() ] << item->row();

	foreach( QString mCategory, rootItem->categoryList )
		categoryVisibilityMap[ mCategory ] = true;
};

void NBApplicationsModel::loadIcons() {

	foreach( NBApplicationItem *item, rootItem->children() ) {
		QString icoStr = item->data( NBApplicationsModel::Icon ).toString();
		QIcon icon;
		if ( NBIconProvider::hasIcon( icoStr ) ) {
			icon = QIcon::fromTheme( icoStr, QIcon( icoStr ) );
		}

		else {
			icon = QIcon( NBIconProvider::pixmapIcon( icoStr ) );
			if ( icon.isNull() )
				icon = QIcon::fromTheme( "application-x-desktop", QIcon::fromTheme( "application-x-executable", QIcon( ":/icons/exec.png" ) ) );
		}

		item->setIcon( icon );

		QModelIndex idx = index( item->row(), 0, QModelIndex() );
		emit dataChanged( idx, idx );
	}
};
