/*
	*
	* NBAppsView.cpp - NewBreeze FolderView Class
	*
*/

#include <NBAppsView.hpp>

NBApplicationsView::NBApplicationsView() : QAbstractItemView() {

	// Set the Apps Delegate
	setItemDelegate( new NBAppsDelegate() );

	// Applications Model
	appModel = new NBApplicationsModel();
	setModel( appModel );

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

	connect( this, SIGNAL( activated( const QModelIndex& ) ), this, SLOT( runApplication( const QModelIndex& ) ) );
};

void NBApplicationsView::setModel( QAbstractItemModel *model ) {

	QAbstractItemView::setModel( model );
	hashIsDirty = true;
};

int NBApplicationsView::categoryHeight() const {

	return myCategoryHeight;
};

void NBApplicationsView::setCategoryHeight( int newCategoryHeight ) {

	myCategoryHeight = newCategoryHeight;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QSize NBApplicationsView::gridSize() const {

	return myGridSizeMin;
};

void NBApplicationsView::setGridSize( QSize newGridSize ) {

	myGridSizeMin = newGridSize;

	computeRowsAndColumns();

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBApplicationsView::setGridSize( int nWidth, int nHeight ) {

	setGridSize( QSize( nWidth, nHeight ) );
};

QSize NBApplicationsView::iconSize() const {

	return myIconSize;
};

void NBApplicationsView::setIconSize( QSize newIconSize ) {

	myIconSize = newIconSize;
	viewport()->update();
};

void NBApplicationsView::setIconSize( int nWidth, int nHeight ) {

	setIconSize( QSize( nWidth, nHeight ) );
};

QMargins NBApplicationsView::contentsMargins() const {

	return myContentsMargins;
};

void NBApplicationsView::setContentsMargins( QMargins newContentsMargins ) {

	myContentsMargins = newContentsMargins;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBApplicationsView::setContentsMargins( int nLeft, int nTop, int nRight, int nBottom ) {

	setContentsMargins( QMargins( nLeft, nTop, nRight, nBottom ) );
};

QMargins NBApplicationsView::inlayMargins() const {

	return myInlayMargins;
};

void NBApplicationsView::setInlayMargins( QMargins newInlayMargins ) {

	myInlayMargins = newInlayMargins;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBApplicationsView::setInlayMargins( int nLeft, int nTop, int nRight, int nBottom ) {

	setInlayMargins( QMargins( nLeft, nTop, nRight, nBottom ) );

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

int NBApplicationsView::categorySpacing() const {

	return myCategorySpacing;
};

void NBApplicationsView::setCategorySpacing( int newCategorySpacing ) {

	myCategorySpacing = newCategorySpacing;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBApplicationsView::calculateRectsIfNecessary() const {

	if ( not hashIsDirty )
		return;

	computeRowsAndColumns();

	int x = 0, y = 0, prevRows = 0, totalRows = 0;
	QStringList categoryList = appModel->categories();

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = appModel->indexListForCategory( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		int minX = myContentsMargins.left();
		int minY = myContentsMargins.top() + catIdx * myCategoryHeight + catIdx * myCategorySpacing + totalRows * myGridSize.height();

		int categoryWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
		rectForCategory[ catIdx ] = QRect( minX, minY, categoryWidth, 24 );

		if ( not appModel->isCategoryVisible( appModel->category( mList.value( 0 ) ) ) )
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

void NBApplicationsView::computeRowsAndColumns() const {

	int vWidth = viewport()->width() - contentsMargins().left() - contentsMargins().right();
	vWidth = vWidth - myInlayMargins.left() - myInlayMargins.right();

	itemsPerRow = qMax( 1, vWidth / myGridSizeMin.width() );

	numberOfRows = ( int )( appModel->rowCount() / itemsPerRow );
	if ( ( appModel->rowCount() % itemsPerRow ) > 0 )
		numberOfRows++;

	int newGridWidth = ( int )( vWidth / itemsPerRow );
	myGridSize = QSize( newGridWidth, myGridSizeMin.height() );
	myItemSize = myGridSize - QSize( 10, 10 );
};

QRect NBApplicationsView::visualRect( const QModelIndex &index ) const {

	QRect rect;
	if ( index.isValid() )
		rect = viewportRectForRow( index.row() );

	return rect;
};

QRect NBApplicationsView::viewportRectForRow( int row ) const {

	calculateRectsIfNecessary();
	QPoint pt = rectForRow.value( row );
	if ( !QRect( pt, myGridSize ).isValid() )
		return QRect();

    return QRect( pt.x(), pt.y() - verticalScrollBar()->value(), myItemSize.width(), myItemSize.height() );
};

QRect NBApplicationsView::categoryRect( int categoryIndex ) const {

	calculateRectsIfNecessary();
	QRect rect = rectForCategory.value( categoryIndex );
	if ( not rect.isValid() )
		return QRect();

    return QRect( rect.x(), rect.y() - verticalScrollBar()->value(), rect.width(), rect.height() );
};

void NBApplicationsView::scrollTo( const QModelIndex &index, QAbstractItemView::ScrollHint ) {

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

QModelIndex NBApplicationsView::indexAt( const QPoint &point_ ) const {

	QPoint point( point_ );
	point.ry() += verticalScrollBar()->value();
	calculateRectsIfNecessary();
	QHashIterator<int, QPoint> i( rectForRow );
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( i.value(), myItemSize ).contains( point ) )
			return appModel->index( i.key(), 0, rootIndex() );
	}
	return QModelIndex();
};

QString NBApplicationsView::categoryAt( const QPoint &point_ ) const {

	QPoint point( point_ );
	point.ry() += verticalScrollBar()->value();
	calculateRectsIfNecessary();
	QHashIterator<int, QRect> i( rectForCategory );
	while ( i.hasNext() ) {

		i.next();
		if ( i.value().contains( point ) )
			return appModel->categories().value( i.key() );
	}

	return QString();
};

void NBApplicationsView::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight ) {

	hashIsDirty = true;
	QAbstractItemView::dataChanged( topLeft, bottomRight );
};

void NBApplicationsView::rowsInserted( const QModelIndex &parent, int start, int end ) {

	hashIsDirty = true;
	QAbstractItemView::rowsInserted( parent, start, end );
};

void NBApplicationsView::rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end ) {

	hashIsDirty = true;
	QAbstractItemView::rowsAboutToBeRemoved( parent, start, end );
};

QModelIndex NBApplicationsView::moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers ) {

	QModelIndex index = currentIndex();
	if ( index.isValid() ) {

		QStringList categoryList = appModel->categories();
		switch( cursorAction ) {
			case QAbstractItemView::MoveRight: {
				if ( index.row() >= 0 and index.row() < appModel->rowCount() ) {
					QModelIndex mIdx = appModel->index( index.row() + 1, 0, rootIndex() );
					if ( appModel->isCategoryVisible( appModel->category( mIdx ) ) )
						return mIdx;

					else {
						// We need to return the next visible index
						for ( int i = appModel->categoryIndex( index ) + 1; i < categoryList.count(); i++ ) {
							if ( appModel->isCategoryVisible( categoryList.value( i ) ) )
								return appModel->indexListForCategory( categoryList.value( i ) ).first();
						}
						return index;
					}
				}

				else {
					return appModel->index( appModel->rowCount() - 1, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveLeft: {
				if ( index.row() >= 0 and index.row() < appModel->rowCount() ) {
					QModelIndex mIdx = appModel->index( index.row() - 1, 0, rootIndex() );
					if ( appModel->isCategoryVisible( appModel->category( mIdx ) ) )
						return mIdx;

					else {
						// We need to return the previous visible index
						for ( int i = appModel->categoryIndex( index ) - 1; i < categoryList.count(); i-- ) {
							if ( appModel->isCategoryVisible( categoryList.value( i ) ) )
								return appModel->indexListForCategory( categoryList.value( i ) ).last();
						}
						return index;
					}
				}

				else {
					return appModel->index( 0, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveDown: {

				QString thisCategory = appModel->category( index );
				QString nextCategory;

				for ( int i = categoryList.indexOf( thisCategory ) + 1; i < categoryList.count(); i++ ) {
					if ( appModel->isCategoryVisible( categoryList.at( i ) ) ) {
						nextCategory = categoryList.value( i );
						break;
					}
				}

				QModelIndexList thisCategoryIndexes = appModel->indexListForCategory( thisCategory );
				QModelIndexList nextCategoryIndexes = appModel->indexListForCategory( nextCategory );

				// Calculate the visual row of this index
				int rowInCategory = thisCategoryIndexes.indexOf( index );
				int vrow = rowInCategory / itemsPerRow;
				int vcol = rowInCategory % itemsPerRow;
				int nrow = ( int )ceil( 1.0 * thisCategoryIndexes.count() / itemsPerRow );

				int newVRow = vrow + 1;

				if ( ( newVRow * itemsPerRow + vcol ) < thisCategoryIndexes.count() ) {
					// We have an index below this index from the same category
					return thisCategoryIndexes.value( newVRow * itemsPerRow + vcol );
				}

				else if ( newVRow < nrow ) {
					// We have indexes with row greater than this one, not below this one
					return thisCategoryIndexes.last();
				}

				else {
					// We need to return the index from the next category same or lower column
					if ( nextCategoryIndexes.count() ) {
						if ( nextCategoryIndexes.count() <= vcol )
							return nextCategoryIndexes.last();

						else
							return nextCategoryIndexes.value( vcol );
					}

					else {

						return thisCategoryIndexes.last();
					};
				}

			}

			case QAbstractItemView::MoveUp: {
				QString thisCategory = appModel->category( index );
				QString prevCategory;

				for( int i = categoryList.indexOf( thisCategory ) - 1; i >= 0; i-- ) {
					if ( appModel->isCategoryVisible( categoryList.at( i ) ) ) {
						prevCategory = categoryList.value( i );
						break;
					}
				}

				QModelIndexList thisCategoryIndexes = appModel->indexListForCategory( thisCategory );
				QModelIndexList prevCategoryIndexes = appModel->indexListForCategory( prevCategory );

				// Calculate the visual row of this index
				int rowInCategory = thisCategoryIndexes.indexOf( index );
				int vrow = rowInCategory / itemsPerRow;
				int vcol = rowInCategory % itemsPerRow;

				int newVRow = vrow - 1;

				if ( newVRow >= 0 ) {

					// This means that there is a visual row before this one
					return thisCategoryIndexes.value( newVRow * itemsPerRow + vcol );
				}
				else {
					if ( prevCategoryIndexes.count() ) {
						int nrow = ( int )ceil( 1.0 * prevCategoryIndexes.count() / itemsPerRow );
						if ( prevCategoryIndexes.count() > ( nrow - 1 ) * itemsPerRow + vcol )
							return prevCategoryIndexes.value( ( nrow - 1 ) * itemsPerRow + vcol );

						else
							return prevCategoryIndexes.last();
					}

					else {

						// This is the first category
						return thisCategoryIndexes.first();
					}
				}
			}

			case QAbstractItemView::MoveHome: {

				return appModel->index( 0, 0, index.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				return appModel->index( appModel->rowCount() - 1, 0, index.parent() );
			}

			case QAbstractItemView::MoveNext: {
				if ( index.row() >= 0 and index.row() < appModel->rowCount() ) {
					QModelIndex mIdx = appModel->index( index.row() + 1, 0, rootIndex() );
					if ( appModel->isCategoryVisible( appModel->category( mIdx ) ) )
						return mIdx;

					else {
						// We need to return the next visible index
						for ( int i = appModel->categoryIndex( index ) + 1; i < categoryList.count(); i++ ) {
							if ( appModel->isCategoryVisible( categoryList.value( i ) ) )
								return appModel->indexListForCategory( categoryList.value( i ) ).first();
						}
						return index;
					}
				}

				else {
					return appModel->index( appModel->rowCount() - 1, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MovePrevious: {

				if ( index.row() >= 0 and index.row() < appModel->rowCount() ) {
					QModelIndex mIdx = appModel->index( index.row() - 1, 0, rootIndex() );
					if ( appModel->isCategoryVisible( appModel->category( mIdx ) ) )
						return mIdx;

					else {
						// We need to return the previous visible index
						for ( int i = appModel->categoryIndex( index ) - 1; i < categoryList.count(); i-- ) {
							if ( appModel->isCategoryVisible( categoryList.value( i ) ) )
								return appModel->indexListForCategory( categoryList.value( i ) ).last();
						}
						return index;
					}
				}

				else {
					return appModel->index( 0, 0, rootIndex() );
				}
			}

			default: {
				return QModelIndex();
			}
		}
	}

	else {
		switch( cursorAction ) {
			case QAbstractItemView::MoveHome: {

				return appModel->index( 0, 0, index.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				return appModel->index( appModel->rowCount() - 1, 0, index.parent() );
			}

			default: {

				return appModel->index( 0, 0, rootIndex() );
			}
		}
	}

	return index;
};

int NBApplicationsView::horizontalOffset() const {

	return horizontalScrollBar()->value();
};

int NBApplicationsView::verticalOffset() const {

	return verticalScrollBar()->value();
};

void NBApplicationsView::scrollContentsBy( int dx, int dy ) {

	scrollDirtyRegion( dx, dy );
	viewport()->scroll( dx, dy );
};

void NBApplicationsView::setSelection( const QRect &rect, QFlags<QItemSelectionModel::SelectionFlag> flags ) {

	QRect rectangle = rect.translated( horizontalScrollBar()->value(), verticalScrollBar()->value() ).normalized();

	calculateRectsIfNecessary();
	QHashIterator<int, QPoint> i( rectForRow );
	int firstRow = appModel->rowCount();
	int lastRow = -1;
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( i.value(), myItemSize ).intersects( rectangle ) ) {
			firstRow = firstRow < i.key() ? firstRow : i.key();
			lastRow = lastRow > i.key() ? lastRow : i.key();
		}
	}

	if ( firstRow != appModel->rowCount() && lastRow != -1 ) {
		QItemSelection selection( appModel->index( firstRow, 0, rootIndex() ), appModel->index( lastRow, 0, rootIndex() ) );
		selectionModel()->select( selection, flags );
	}

	else {
		QModelIndex invalid;
		QItemSelection selection( invalid, invalid );
		selectionModel()->select( selection, flags );
	}
};

void NBApplicationsView::paintEvent( QPaintEvent* event ) {

	QPainter painter( viewport() );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	for ( int catIdx = 0; catIdx < appModel->categories().count(); catIdx++ ) {

		QRect rect = categoryRect( catIdx );
		if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
			continue;

		paintCategory( &painter, rect, appModel->categories().at( catIdx ) );
	}

	for ( int row = 0; row < appModel->rowCount( rootIndex() ); row++ ) {
		QModelIndex index = appModel->index( row, 0, rootIndex() );

		if ( not appModel->isCategoryVisible( appModel->category( index ) ) )
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

void NBApplicationsView::paintCategory( QPainter *painter, const QRect &rectangle, const QString &text ) const {

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

	painter->drawPixmap( topLeft.x() + 4, topLeft.y() + 4, 16, 16, pix );
	QFont categoryFont = qApp->font();
	categoryFont.setBold( true );
	painter->setFont( categoryFont );
	painter->drawText( topLeft.x() + 24, topLeft.y(), rectangle.width() - 48, rectangle.height(), Qt::AlignVCenter, text );

	painter->restore();
};

QPixmap NBApplicationsView::pixmapForCategory( QString categoryText ) const {

	if ( categoryText == "Accessories" )
		return QIcon::fromTheme( "applications-accessories" ).pixmap( 16, 16 );

	else if ( categoryText == "Development" )
		return QIcon::fromTheme( "applications-development" ).pixmap( 16, 16 );

	else if ( categoryText == "Education" )
		return QIcon::fromTheme( "applications-education" ).pixmap( 16, 16 );

	else if ( categoryText == "Games" )
		return QIcon::fromTheme( "applications-games" ).pixmap( 16, 16 );

	else if ( categoryText == "Graphics" )
		return QIcon::fromTheme( "applications-graphics" ).pixmap( 16, 16 );

	else if ( categoryText == "Internet" )
		return QIcon::fromTheme( "applications-internet" ).pixmap( 16, 16 );

	else if ( categoryText == "Multimedia" )
		return QIcon::fromTheme( "applications-multimedia" ).pixmap( 16, 16 );

	else if ( categoryText == "Office" )
		return QIcon::fromTheme( "applications-office" ).pixmap( 16, 16 );

	else if ( categoryText == "Science and Math" )
		return QIcon::fromTheme( "applications-science" ).pixmap( 16, 16 );

	else if ( categoryText == "Settings" )
		return QIcon::fromTheme( "applications-settings" ).pixmap( 16, 16 );

	else if ( categoryText == "System" )
		return QIcon::fromTheme( "applications-system" ).pixmap( 16, 16 );

	else
		return QIcon::fromTheme( "preferences-other" ).pixmap( 16, 16 );
};

QRegion NBApplicationsView::visualRegionForSelection( const QItemSelection &selection ) const {

	QRegion region;
	foreach ( const QItemSelectionRange &range, selection ) {
		for ( int row = range.top(); row <= range.bottom(); ++row ) {
			for ( int column = range.left(); column < range.right(); ++column ) {
				QModelIndex index = appModel->index( row, column, rootIndex() );
				region += visualRect( index );
			}
		}
	}
	return region;
};

void NBApplicationsView::resizeEvent( QResizeEvent* ) {

	hashIsDirty = true;

	calculateRectsIfNecessary();
	updateGeometries();
};

void NBApplicationsView::updateGeometries() {

	computeRowsAndColumns();

	verticalScrollBar()->setSingleStep( myGridSize.height() );
	verticalScrollBar()->setPageStep( viewport()->height() );
	verticalScrollBar()->setRange( 0, qMax( 0, idealHeight - viewport()->height() ) );
};

void NBApplicationsView::mousePressEvent( QMouseEvent *event ) {

	QAbstractItemView::mousePressEvent( event );
	setCurrentIndex( indexAt( event->pos() ) );
};

void NBApplicationsView::mouseDoubleClickEvent( QMouseEvent *event ) {

	/*
		*
		* QString mCategory = categoryAt( event->pos() );
		* if ( ( not mCategory.isNull() ) or ( not mCategory.isEmpty() ) ) {
		* 	if ( appModel->isCategoryVisible( mCategory ) )
		* 		appModel->hideCategory( mCategory );
        *
		* 	else
		* 		appModel->showCategory( mCategory );
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
		QStringList execList = index.data( NBApplicationsModel::ExecList ).toStringList();
		bool runInTerminal = index.data( NBApplicationsModel::Terminal ).toBool();

		if ( not runInTerminal ) {

			// Try to run this program
			QProcess::startDetached( execList.at( 0 ) );
		}

		else {
			QStringList terminalList = getTerminal().join( " " ).arg( QDir::homePath() ).arg( execList.at( 0 ) ).split( " " );
			QProcess::startDetached( terminalList.takeFirst(), terminalList );
		}
	}

	event->accept();
};

void NBApplicationsView::runApplication(const QModelIndex &index ) {

	QStringList execList = index.data( NBApplicationsModel::ExecList ).toStringList();
	bool runInTerminal = index.data( NBApplicationsModel::Terminal ).toBool();

	if ( not runInTerminal ) {

		// Try to run this program
		QProcess::startDetached( execList.at( 0 ) );
	}

	else {
		QStringList terminalList = getTerminal().join( " " ).arg( QDir::homePath() ).arg( execList.at( 0 ) ).split( " " );
		QProcess::startDetached( terminalList.takeFirst(), terminalList );
	}
};
