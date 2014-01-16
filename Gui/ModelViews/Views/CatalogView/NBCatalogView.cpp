/*
	*
	* NBCatalogView.cpp - NewBreeze Catalog Viewer Class
	*
*/

#include <NBCatalogView.hpp>

NBCatalogView::NBCatalogView() : QAbstractItemView() {

	// Set the Apps Delegate
	setItemDelegate( new NBCatalogDelegate() );

	// Applications Model
	catalogModel = new NBCatalogModel();
	setModel( catalogModel );

	// Grid Size
	setGridSize( QSize( 120, 80 ) );

	// Icon Size
	setIconSize( QSize( 48, 48 ) );

	// Selection
	setSelectionMode( QAbstractItemView::SingleSelection );
	setSelectionBehavior( QAbstractItemView::SelectRows );

	// Internal Object Name
	setObjectName( "mainList" );

	// Minimum Size
	setMinimumWidth( 640 );

	// Focus Policy
	setFocusPolicy( Qt::WheelFocus );

	// Font
	setFont( qApp->font() );

	// Mouse tracking
	setMouseTracking( true );

	// No Horizontal ScrollBar
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	horizontalScrollBar()->hide();

	// Always how vertical scrollbar
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
	verticalScrollBar()->show();
	verticalScrollBar()->setRange( 0, 0 );

	// Context Menu
	setContextMenuPolicy( Qt::CustomContextMenu );
	connect( this, SIGNAL( customContextMenuRequested( QPoint ) ), this, SLOT( showContextMenu( QPoint ) ) );

	// Handle Enter Press
	connect( this, SIGNAL( activated( const QModelIndex& ) ), this, SLOT( openCatalogItem( const QModelIndex& ) ) );
};

void NBCatalogView::setModel( QAbstractItemModel *model ) {

	QAbstractItemView::setModel( model );
	hashIsDirty = true;
};

int NBCatalogView::categoryHeight() const {

	return myCategoryHeight;
};

void NBCatalogView::setCategoryHeight( int newCategoryHeight ) {

	myCategoryHeight = newCategoryHeight;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QSize NBCatalogView::gridSize() const {

	return myGridSizeMin;
};

void NBCatalogView::setGridSize( QSize newGridSize ) {

	myGridSizeMin = newGridSize;

	computeRowsAndColumns();

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBCatalogView::setGridSize( int nWidth, int nHeight ) {

	setGridSize( QSize( nWidth, nHeight ) );
};

QSize NBCatalogView::iconSize() const {

	return myIconSize;
};

void NBCatalogView::setIconSize( QSize newIconSize ) {

	myIconSize = newIconSize;
	viewport()->update();
};

void NBCatalogView::setIconSize( int nWidth, int nHeight ) {

	setIconSize( QSize( nWidth, nHeight ) );
};

QMargins NBCatalogView::contentsMargins() const {

	return myContentsMargins;
};

void NBCatalogView::setContentsMargins( QMargins newContentsMargins ) {

	myContentsMargins = newContentsMargins;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBCatalogView::setContentsMargins( int nLeft, int nTop, int nRight, int nBottom ) {

	setContentsMargins( QMargins( nLeft, nTop, nRight, nBottom ) );
};

QMargins NBCatalogView::inlayMargins() const {

	return myInlayMargins;
};

void NBCatalogView::setInlayMargins( QMargins newInlayMargins ) {

	myInlayMargins = newInlayMargins;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBCatalogView::setInlayMargins( int nLeft, int nTop, int nRight, int nBottom ) {

	setInlayMargins( QMargins( nLeft, nTop, nRight, nBottom ) );

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

int NBCatalogView::categorySpacing() const {

	return myCategorySpacing;
};

void NBCatalogView::setCategorySpacing( int newCategorySpacing ) {

	myCategorySpacing = newCategorySpacing;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBCatalogView::calculateRectsIfNecessary() const {

	if ( not hashIsDirty )
		return;

	computeRowsAndColumns();

	rectForCategory.clear();
	rectForRow.clear();

	int x = 0, y = 0, prevRows = 0, totalRows = 0;
	QStringList categoryList = catalogModel->catalogs();

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = catalogModel->indexListForCatalog( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		int minX = myContentsMargins.left();
		int minY = myContentsMargins.top() + catIdx * myCategoryHeight + catIdx * myCategorySpacing + totalRows * myGridSize.height();

		int categoryWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
		rectForCategory[ catIdx ] = QRect( minX, minY, categoryWidth, 24 );

		if ( not catalogModel->isCatalogVisible( catalogModel->catalogName( mList.value( 0 ) ) ) )
			continue;

		// Mimimum X and Y for indexes
		minX += myInlayMargins.left();
		minY += myCategoryHeight;

		prevRows = mList.count() / itemsPerRow;
		if ( mList.count() % itemsPerRow )
			prevRows++;

		totalRows += prevRows;

		for( int lrow = 0; lrow < mList.count(); lrow++ ) {
			int row = lrow / itemsPerRow;
			int col = lrow % itemsPerRow;

			x = minX + col * myGridSize.width() + 5;
			y = minY + row * myGridSize.height() + 5;

			rectForRow[ mList[ lrow ].row() ] = QPoint( x, y );
		}

		idealHeight = minY + myCategorySpacing + myContentsMargins.bottom() + prevRows * myGridSize.height();
	}

	hashIsDirty = false;
	viewport()->update();
};

void NBCatalogView::computeRowsAndColumns() const {

	int vWidth = viewport()->width() - contentsMargins().left() - contentsMargins().right();
	vWidth = vWidth - myInlayMargins.left() - myInlayMargins.right();

	itemsPerRow = qMax( 1, vWidth / myGridSizeMin.width() );

	numberOfRows = ( int )( catalogModel->rowCount() / itemsPerRow );
	if ( ( catalogModel->rowCount() % itemsPerRow ) > 0 )
		numberOfRows++;

	int newGridWidth = ( int )( vWidth / itemsPerRow );
	myGridSize = QSize( newGridWidth, myGridSizeMin.height() );
	myItemSize = myGridSize - QSize( 10, 10 );
};

QRect NBCatalogView::visualRect( const QModelIndex &index ) const {

	QRect rect;
	if ( index.isValid() )
		rect = viewportRectForRow( index.row() );

	return rect;
};

QRect NBCatalogView::viewportRectForRow( int row ) const {

	calculateRectsIfNecessary();
	QPoint pt = rectForRow.value( row );
	if ( !QRect( pt, myGridSize ).isValid() )
		return QRect();

	return QRect( pt.x(), pt.y() - verticalScrollBar()->value(), myItemSize.width(), myItemSize.height() );
};

QRect NBCatalogView::categoryRect( int catalogIndex ) const {

	calculateRectsIfNecessary();
	QRect rect = rectForCategory.value( catalogIndex );
	if ( not rect.isValid() )
		return QRect();

	return QRect( rect.x(), rect.y() - verticalScrollBar()->value(), rect.width(), rect.height() );
};

void NBCatalogView::scrollTo( const QModelIndex &index, QAbstractItemView::ScrollHint ) {

	QRect viewRect = viewport()->rect();
	QRect itemRect = visualRect( index );

	if ( itemRect.top() < viewRect.top() ) {
		verticalScrollBar()->setValue( verticalScrollBar()->value() + itemRect.top() - viewRect.top() );
	}

	else if ( itemRect.bottom() > viewRect.bottom() ) {
		verticalScrollBar()->setValue(
			verticalScrollBar()->value() + qMin( itemRect.bottom() - viewRect.bottom(), itemRect.top() - viewRect.top() )
		);
	}

	viewport()->update();
};

QModelIndex NBCatalogView::indexAt( const QPoint &point_ ) const {

	QPoint point( point_ );
	point.ry() += verticalScrollBar()->value();
	calculateRectsIfNecessary();
	QHashIterator<int, QPoint> i( rectForRow );
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( i.value(), myItemSize ).contains( point ) )
			return catalogModel->index( i.key(), 0, rootIndex() );
	}
	return QModelIndex();
};

QString NBCatalogView::categoryAt( const QPoint &point_ ) const {

	QPoint point( point_ );
	point.ry() += verticalScrollBar()->value();
	calculateRectsIfNecessary();
	QHashIterator<int, QRect> i( rectForCategory );
	while ( i.hasNext() ) {

		i.next();
		if ( i.value().contains( point ) )
			return catalogModel->catalogs().value( i.key() );
	}

	return QString();
};

void NBCatalogView::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight ) {

	hashIsDirty = true;
	QAbstractItemView::dataChanged( topLeft, bottomRight );
};

void NBCatalogView::rowsInserted( const QModelIndex &parent, int start, int end ) {

	hashIsDirty = true;
	QAbstractItemView::rowsInserted( parent, start, end );
};

void NBCatalogView::rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end ) {

	hashIsDirty = true;
	QAbstractItemView::rowsAboutToBeRemoved( parent, start, end );
};

QModelIndex NBCatalogView::moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers ) {

	QModelIndex idx = currentIndex();
	QStringList categoryList = catalogModel->catalogs();

	/* If there exists a current index */
	if ( idx.isValid() ) {

		int thisCategoryIdx = catalogModel->catalogIndex( idx );
		/* If we are in the first category go to the last one */
		int prevCategoryIdx = ( thisCategoryIdx == 0 ? catalogModel->catalogCount() - 1 : thisCategoryIdx - 1 );
		/* If we are in the last category go to the first one */
		int nextCategoryIdx = ( ( thisCategoryIdx == catalogModel->catalogCount() - 1 ) ? 0 : thisCategoryIdx + 1 );

		QString prevCategory = categoryList.value( prevCategoryIdx );
		QString thisCategory = catalogModel->catalogName( idx );
		QString nextCategory = categoryList.value( nextCategoryIdx );

		QModelIndexList prevcatalogIndexes = catalogModel->indexListForCatalog( prevCategory );
		QModelIndexList thiscatalogIndexes = catalogModel->indexListForCatalog( thisCategory );
		QModelIndexList nextcatalogIndexes = catalogModel->indexListForCatalog( nextCategory );

		// Calculate the visual row of this index
		int rowInCategory = thiscatalogIndexes.indexOf( idx );
		int vrow = rowInCategory / itemsPerRow;

		switch( cursorAction ) {
			case QAbstractItemView::MoveNext:
			case QAbstractItemView::MoveRight: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < catalogModel->rowCount() - 1 ) {
					/* If this is the visual index of the catgory, then next persistentVCol will be zero */
					if ( idx == thiscatalogIndexes.last() )
						persistentVCol = 0;

					else
						persistentVCol = ( rowInCategory + 1 ) % itemsPerRow;

					return catalogModel->index( idx.row() + 1, 0, rootIndex() );
				}

				/* Current index is the last visible index */
				else {
					persistentVCol = 0;
					return catalogModel->index( 0, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MovePrevious:
			case QAbstractItemView::MoveLeft: {
				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < catalogModel->rowCount() ) {
					if ( idx == thiscatalogIndexes.first() )
						persistentVCol = ( prevcatalogIndexes.count() - 1 ) % itemsPerRow;

					else
						persistentVCol = ( rowInCategory - 1 ) % itemsPerRow;
					return catalogModel->index( idx.row() - 1, 0, rootIndex() );
				}

				/* The current index is the first one */
				else {
					// #warning "FIXME: This implementation is buggy"
					persistentVCol = ( prevcatalogIndexes.count() - 1 ) % itemsPerRow;
					return catalogModel->index( catalogModel->rowCount() - 1, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveDown: {
				if ( Settings->General.FolderView == "DetailsView" ) {
					if ( idx.row() == catalogModel->rowCount() - 1 ) {
						return catalogModel->index( 0, 0, idx.parent() );
					}
					else {
						return catalogModel->index( idx.row() + 1, 0, idx.parent() );
					}
				}

				int newVRow = vrow + 1;

				if ( ( newVRow * itemsPerRow + persistentVCol ) < thiscatalogIndexes.count() ) {
					// We have an idx below this idx from the same category
					return thiscatalogIndexes.value( newVRow * itemsPerRow + persistentVCol );
				}

				else if ( newVRow * itemsPerRow < thiscatalogIndexes.count() ) {
					// We have indexes with row greater than this one, not below this one
					return thiscatalogIndexes.last();
				}

				else {
					// We need to return the idx from the next category same or lower column
					if ( nextcatalogIndexes.count() ) {
						if ( nextcatalogIndexes.count() <= persistentVCol )
							return nextcatalogIndexes.last();

						else
							return nextcatalogIndexes.value( persistentVCol );
					}

					else {

						return nextcatalogIndexes.last();
					};
				}

			}

			case QAbstractItemView::MoveUp: {

				if ( Settings->General.FolderView == "DetailsView" ) {
					if ( idx.row() == 0 ) {
						return catalogModel->index( catalogModel->rowCount() - 1, 0, idx.parent() );
					}
					else {
						return catalogModel->index( idx.row() - 1, 0, idx.parent() );
					}
				}

				int newVRow = vrow - 1;

				if ( newVRow >= 0 ) {

					// This means that there is a visual row before this one
					return thiscatalogIndexes.value( newVRow * itemsPerRow + persistentVCol );
				}

				else {
					if ( prevcatalogIndexes.count() ) {
						int nrow = ( int )ceil( 1.0 * prevcatalogIndexes.count() / itemsPerRow );
						if ( prevcatalogIndexes.count() > ( nrow - 1 ) * itemsPerRow + persistentVCol )
							return prevcatalogIndexes.value( ( nrow - 1 ) * itemsPerRow + persistentVCol );

						else
							return prevcatalogIndexes.last();
					}

					else {

						// This is the first category
						return thiscatalogIndexes.first();
					}
				}
			}

			case QAbstractItemView::MoveHome: {

				verticalScrollBar()->setValue( 0 );
				persistentVCol = 0;
				return catalogModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				persistentVCol = ( catalogModel->indexListCountForCatalog( categoryList.last() ) - 1 ) % itemsPerRow;
				return catalogModel->index( catalogModel->rowCount() - 1, 0, idx.parent() );
			}

			default: {
				return QModelIndex();
			}
		}
	}

	/* If there is no current index */
	else {
		switch( cursorAction ) {
			case QAbstractItemView::MoveHome: {

				persistentVCol = 0;
				verticalScrollBar()->setValue( 0 );
				return catalogModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				persistentVCol = ( catalogModel->indexListCountForCatalog( categoryList.last() ) - 1 ) % itemsPerRow;
				return catalogModel->index( catalogModel->rowCount() - 1, 0, idx.parent() );
			}

			default: {

				return catalogModel->index( 0, 0, rootIndex() );
			}
		}
	}

	return idx;
};

int NBCatalogView::horizontalOffset() const {

	return horizontalScrollBar()->value();
};

int NBCatalogView::verticalOffset() const {

	return verticalScrollBar()->value();
};

void NBCatalogView::scrollContentsBy( int dx, int dy ) {

	scrollDirtyRegion( dx, dy );
	viewport()->scroll( dx, dy );
};

void NBCatalogView::setSelection( const QRect &rect, QFlags<QItemSelectionModel::SelectionFlag> flags ) {

	QRect rectangle = rect.translated( horizontalScrollBar()->value(), verticalScrollBar()->value() ).normalized();

	calculateRectsIfNecessary();
	QHashIterator<int, QPoint> i( rectForRow );
	int firstRow = catalogModel->rowCount();
	int lastRow = -1;
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( i.value(), myItemSize ).intersects( rectangle ) ) {
			firstRow = firstRow < i.key() ? firstRow : i.key();
			lastRow = lastRow > i.key() ? lastRow : i.key();
		}
	}

	if ( firstRow != catalogModel->rowCount() && lastRow != -1 ) {
		QItemSelection selection( catalogModel->index( firstRow, 0, rootIndex() ), catalogModel->index( lastRow, 0, rootIndex() ) );
		selectionModel()->select( selection, flags );
	}

	else {
		QModelIndex invalid;
		QItemSelection selection( invalid, invalid );
		selectionModel()->select( selection, flags );
	}
};

void NBCatalogView::paintEvent( QPaintEvent* event ) {

	QPainter painter( viewport() );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	for ( int catIdx = 0; catIdx < catalogModel->catalogs().count(); catIdx++ ) {

		QRect rect = categoryRect( catIdx );
		if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
			continue;

		paintCategory( &painter, rect, catalogModel->catalogs().at( catIdx ) );
	}

	for ( int row = 0; row < catalogModel->rowCount( rootIndex() ); row++ ) {
		QModelIndex index = catalogModel->index( row, 0, rootIndex() );

		if ( not catalogModel->isCatalogVisible( catalogModel->catalogName( index ) ) )
			continue;

		QRect rect = viewportRectForRow( row );
		if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
			continue;

		QStyleOptionViewItem option = viewOptions();
		option.rect = rect;

		if ( selectionModel()->isSelected( index ) )
			option.state |= QStyle::State_Selected;

		if ( currentIndex() == index )
			option.state |= QStyle::State_HasFocus;

		QPoint mousePos = mapFromGlobal( cursor().pos() );
		if ( rect.contains( mousePos ) )
			option.state |= QStyle::State_MouseOver;

		option.decorationSize = myIconSize;

		itemDelegate()->paint( &painter, option, index );
	}

	event->accept();
};

void NBCatalogView::paintCategory( QPainter *painter, const QRect &rectangle, const QString &text ) const {

	painter->save();

	QColor textColor = painter->pen().color();

	QLinearGradient hLine( rectangle.topLeft(), rectangle.topRight() );
	hLine.setColorAt( 0, textColor );
	hLine.setColorAt( 1, Qt::transparent );

	QBrush brush( hLine );
	QPen hPen( brush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );

	painter->setPen( hPen );
	painter->drawLine( rectangle.topLeft(), rectangle.topRight() );

	painter->setPen( QPen( textColor ) );

	QPixmap pix = pixmapForCategory( text );
	QPoint topLeft = rectangle.topLeft();

	painter->drawPixmap( topLeft.x() + 10, topLeft.y() + 4, 16, 16, pix );
	QFont categoryFont = qApp->font();
	categoryFont.setBold( true );
	painter->setFont( categoryFont );
	painter->drawText( topLeft.x() + 30, topLeft.y(), rectangle.width() - 48, rectangle.height(), Qt::AlignVCenter, text );

	painter->restore();
};

QPixmap NBCatalogView::pixmapForCategory( QString categoryText ) const {

	if ( categoryText == "Documents" )
		return QIcon( ":/icons/documents.png" ).pixmap( 16, 16 );

	else if ( categoryText == "Music" )
		return QIcon::fromTheme( "folder-sound" ).pixmap( 16, 16 );

	else if ( categoryText == "Pictures" )
		return QIcon::fromTheme( "folder-image" ).pixmap( 16, 16 );

	else if ( categoryText == "Videos" )
		return QIcon::fromTheme( "folder-video" ).pixmap( 16, 16 );

	else
		return QIcon( ":/icons/catalogs-alt.png" ).pixmap( 16, 16 );
};

QRegion NBCatalogView::visualRegionForSelection( const QItemSelection &selection ) const {

	QRegion region;
	foreach ( const QItemSelectionRange &range, selection ) {
		for ( int row = range.top(); row <= range.bottom(); ++row ) {
			for ( int column = range.left(); column < range.right(); ++column ) {
				QModelIndex index = catalogModel->index( row, column, rootIndex() );
				region += visualRect( index );
			}
		}
	}
	return region;
};

void NBCatalogView::resizeEvent( QResizeEvent* ) {

	hashIsDirty = true;

	calculateRectsIfNecessary();
	updateGeometries();
};

void NBCatalogView::updateGeometries() {

	computeRowsAndColumns();

	verticalScrollBar()->setSingleStep( myGridSize.height() );
	verticalScrollBar()->setPageStep( viewport()->height() );
	verticalScrollBar()->setRange( 0, qMax( 0, idealHeight - viewport()->height() ) );
};

void NBCatalogView::mousePressEvent( QMouseEvent *event ) {

	QAbstractItemView::mousePressEvent( event );
	setCurrentIndex( indexAt( event->pos() ) );
};

void NBCatalogView::reload() {

	catalogModel->reload();

	hashIsDirty = true;
	persistentVCol = 0;
	calculateRectsIfNecessary();
};

void NBCatalogView::mouseDoubleClickEvent( QMouseEvent *event ) {

	/*
		*
		* QString mCatalog = categoryAt( event->pos() );
		* if ( ( not mCatalog.isNull() ) or ( not mCatalog.isEmpty() ) ) {
		* 	if ( catalogModel->isCatalogVisible( mCatalog ) )
		* 		catalogModel->hideCategory( mCatalog );
		*
		* 	else
		* 		catalogModel->showCategory( mCatalog );
		*
		* 	hashIsDirty = true;
		* 	calculateRectsIfNecessary();
		* 	updateGeometries();
		*
		* 	event->accept();
		*
		* 	return;
		* }
		*
	*/

	QModelIndex index = indexAt( event->pos() );
	if ( index.isValid() ) {
		QString target = index.data( NBCatalogModel::Target ).toString();
		if ( index.data( NBCatalogModel::Exists ).toBool() )
			emit openLocation( target );

		else {
			QString text = QString(
				"You are trying to access this location:<p><tt><b>%1</b></tt></p>"
				"However this location does not exist. If this folder "
				"has been deleted, you may want to delete this catalog entry."
			).arg( target );
			NBMessageDialog::error( "Location Unavailable", text );
		}
	}

	event->accept();
};

void NBCatalogView::openCatalogItem(const QModelIndex &index ) {

	QString target = index.data( NBCatalogModel::Target ).toString();
	if ( index.data( NBCatalogModel::Exists ).toBool() )
		emit openLocation( target );

	else {
		QString text = QString(
			"You are trying to access this location:<p><tt><b>%1</b></tt></p>"
			"However this location does not exist. If this folder "
			"has been deleted, you may want to delete this catalog entry."
		).arg( target );
		NBMessageDialog::error( "Location Unavailable", text );
	}
};

void NBCatalogView::showContextMenu( const QPoint &point ) {

	NBMenu *menu = new NBMenu();

	QAction *openAct = new QAction( QIcon(), "&Open Location", this );
	connect( openAct, SIGNAL( triggered() ), this, SLOT( openCatalogItem() ) );
	menu->addAction( openAct );

	QAction *deleteAct = new QAction( QIcon( ":/icons/close.png" ), "&Remove Location", this );
	connect( deleteAct, SIGNAL( triggered() ), this, SLOT( deleteCatalogItem() ) );
	menu->addAction( deleteAct );

	if ( not indexAt( point ).isValid() ) {
		openAct->setDisabled( true );
		deleteAct->setDisabled( true );
	}

	menu->exec( mapToGlobal( point ) );
};

void NBCatalogView::openCatalogItem() {

	QModelIndex index = currentIndex();
	QString target = index.data( NBCatalogModel::Target ).toString();
	if ( index.data( NBCatalogModel::Exists ).toBool() )
		emit openLocation( target );

	else {
		QString text = QString(
			"You are trying to access this location:<p><tt><b>%1</b></tt></p>"
			"However this location does not exist. If this folder "
			"has been deleted, you may want to delete this catalog entry."
		).arg( target );
		NBMessageDialog::error( "Location Unavailable", text );
	}
};

void NBCatalogView::deleteCatalogItem() {

	QModelIndexList sList = selectionModel()->selectedRows();
	QSettings catalogConf( "NewBreeze", "Catalogs" );

	foreach( QModelIndex idx, sList ) {
		QString catalog = catalogModel->catalogName( idx );
		QString target = sList.at( 0 ).data( NBCatalogModel::Target ).toString();

		QStringList existing = catalogConf.value( catalog ).toStringList();
		existing.removeOne( target );

		catalogConf.setValue( catalog, existing );
		catalogConf.sync();
	}

	reload();
};
