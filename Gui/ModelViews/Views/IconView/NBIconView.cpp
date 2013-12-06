/*
	*
	* NBIconView.cpp - NewBreeze IconView Class
	*
*/

#include <NBIconView.hpp>

NBIconView::NBIconView( NBFileSystemModel *fsModel ) : QAbstractItemView() {

	// Set the Apps Delegate
	setItemDelegate( new NBIconDelegate() );

	// Applications Model
	cModel = fsModel;
	setModel( cModel );

	// Grid Size
	setGridSize( QSize( 120, 80 ) );

	// Icon Size
	setIconSize( QSize( 48, 48 ) );

	// Selection
	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setSelectionBehavior( QAbstractItemView::SelectRows );

	// Internal Object Name
	setObjectName( "mainList" );

	// Minimum Size
	setMinimumWidth( 640 );

	// Focus Policy
	setFocusPolicy( Qt::StrongFocus );

	// Font
	setFont( qApp->font() );

	// Mouse tracking
	setMouseTracking( true );

	// No Horizontal ScrollBar
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	horizontalScrollBar()->hide();

	// DragAndDrop
	viewport()->setAcceptDrops( true );
	setDragEnabled( true );
	setAcceptDrops( true );
	setDragDropMode( QListView::DragDrop );
	setDropIndicatorShown( true );

	// Context Menu
	setContextMenuPolicy( Qt::CustomContextMenu );

	connect(
		this, SIGNAL( activated( QModelIndex ) ),
		this, SIGNAL( open( QModelIndex ) )
	);

	connect(
		cModel, SIGNAL( dirLoading( QString ) ),
		this, SLOT( reload() )
	);

	connect(
		cModel, SIGNAL( directoryLoaded( QString ) ),
		this, SLOT( reload() )
	);

	connect(
			this, SIGNAL( customContextMenuRequested( QPoint ) ),
			this, SIGNAL( contextMenuRequested( QPoint ) )
	);
};

void NBIconView::setModel( QAbstractItemModel *model ) {

	QAbstractItemView::setModel( model );
	hashIsDirty = true;
};

void NBIconView::updateViewMode() {

	/*
		*
		* SmallListView, NormalListView, TilesView, SmallIconsView, NormalIconsView, LargeIconsView, HugeIconsView
		*

		*
		* SmallListView: Wrapping, Mode( ListMode ), Flow( TopToBottom ), GridSize( 256, 32 ), IconSize( 24, 24 )
		* NormalListView: No Wrapping, Mode( ListMode ) Flow( TopToBottom ), GridSize( 256, 48 ), IconSize( 48, 48 )
		* Tiles: Wrapping, Mode( ListMode ), Flow( LeftToRight ), GridSize( 256, 64 ), IconSize( 48, 48 )
		* SmallIconsView: Wrapping, Mode( IconMode ), Flow( LeftToRight ), GridSize( 120, 48 ), IconSize( 24, 24 )
		* NormalIconsView: Wrapping, Mode( IconMode ), Flow( LeftToRight ), GridSize( 120, 80 ), IconSize( 48, 48 )
		* LargeIconsView: Wrapping, Mode( IconMode ), Flow( LeftToRight ), GridSize( 160, 100 ), IconSize( 64, 64 )
		* HugeIconView: Wrapping, Mode( IconMode ), Flow( LeftToRight ), GridSize( 320, 180 ), IconSize( 128, 128 )
		*
	*/

	if ( Settings->General.FolderView == QString( "SmallListView" ) ) {
		// Sizes
		setGridSize( QSize( 256, 28 ) );
		setIconSize( QSize( 24, 24 ) );
	}

	else if ( Settings->General.FolderView == QString( "NormalListView" ) ) {
		// Sizes
		setGridSize( QSize( 256, 52 ) );
		setIconSize( QSize( 48, 48 ) );

		// Wrapping
		// setWrapping( false );
	}

	else if ( Settings->General.FolderView == QString( "TilesView" ) ) {
		// Sizes
		setGridSize( QSize( 256, 64 ) );
		setIconSize( QSize( 48, 48 ) );
	}

	else if ( Settings->General.FolderView == QString( "SmallIconsView" ) ) {
		// Sizes
		setGridSize( QSize( 80, 48 ) );
		setIconSize( QSize( 24, 24 ) );
	}

	else if ( Settings->General.FolderView == QString( "NormalIconsView" ) ) {
		// Sizes
		setGridSize( QSize( 120, 80 ) );
		setIconSize( QSize( 48, 48 ) );
	}

	else if ( Settings->General.FolderView == QString( "LargeIconsView" ) ) {
		// Sizes
		setGridSize( QSize( 160, 100 ) );
		setIconSize( QSize( 64, 64 ) );
	}

	else if ( Settings->General.FolderView == QString( "HugeIconsView" ) ) {
		// Sizes
		setGridSize( QSize( 300, 180 ) );
		setIconSize( QSize( 128, 128 ) );
	}
};

int NBIconView::categoryHeight() const {

	return myCategoryHeight;
};

void NBIconView::setCategoryHeight( int newCategoryHeight ) {

	myCategoryHeight = newCategoryHeight;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QSize NBIconView::gridSize() const {

	return myGridSizeMin;
};

void NBIconView::setGridSize( QSize newGridSize ) {

	myGridSizeMin = newGridSize;

	computeRowsAndColumns();

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::setGridSize( int nWidth, int nHeight ) {

	setGridSize( QSize( nWidth, nHeight ) );
};

QSize NBIconView::iconSize() const {

	return myIconSize;
};

void NBIconView::setIconSize( QSize newIconSize ) {

	myIconSize = newIconSize;
	viewport()->update();
};

void NBIconView::setIconSize( int nWidth, int nHeight ) {

	setIconSize( QSize( nWidth, nHeight ) );
};

QMargins NBIconView::contentsMargins() const {

	return myContentsMargins;
};

void NBIconView::setContentsMargins( QMargins newContentsMargins ) {

	myContentsMargins = newContentsMargins;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::setContentsMargins( int nLeft, int nTop, int nRight, int nBottom ) {

	setContentsMargins( QMargins( nLeft, nTop, nRight, nBottom ) );
};

QMargins NBIconView::inlayMargins() const {

	return myInlayMargins;
};

void NBIconView::setInlayMargins( QMargins newInlayMargins ) {

	myInlayMargins = newInlayMargins;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::setInlayMargins( int nLeft, int nTop, int nRight, int nBottom ) {

	setInlayMargins( QMargins( nLeft, nTop, nRight, nBottom ) );

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

int NBIconView::categorySpacing() const {

	return myCategorySpacing;
};

void NBIconView::setCategorySpacing( int newCategorySpacing ) {

	myCategorySpacing = newCategorySpacing;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QRect NBIconView::visualRect( const QModelIndex &index ) const {

	QRect rect;
	if ( index.isValid() )
		rect = viewportRectForRow( index.row() );

	return rect;
};

QRect NBIconView::categoryRect( int categoryIndex ) const {

	calculateRectsIfNecessary();
	QRect rect = rectForCategory.value( categoryIndex );
	if ( not rect.isValid() )
		return QRect();

    return QRect( rect.x(), rect.y() - verticalScrollBar()->value(), rect.width(), rect.height() );
};

void NBIconView::scrollTo( const QModelIndex &index, QAbstractItemView::ScrollHint ) {

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

QModelIndex NBIconView::indexAt( const QPoint &point_ ) const {

	QPoint point( point_ );
	point.ry() += verticalScrollBar()->value();
	calculateRectsIfNecessary();
	QHashIterator<int, QPoint> i( rectForRow );
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( i.value(), myItemSize ).contains( point ) )
			return cModel->index( i.key(), 0, rootIndex() );
	}
	return QModelIndex();
};

QString NBIconView::categoryAt( const QPoint &point_ ) const {

	QPoint point( point_ );
	point.ry() += verticalScrollBar()->value();
	calculateRectsIfNecessary();
	QHashIterator<int, QRect> i( rectForCategory );
	while ( i.hasNext() ) {

		i.next();
		if ( i.value().contains( point ) )
			return cModel->categories().value( i.key() );
	}

	return QString();
};

void NBIconView::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight ) {

	hashIsDirty = true;
	QAbstractItemView::dataChanged( topLeft, bottomRight );
};

void NBIconView::rowsInserted( const QModelIndex &parent, int start, int end ) {

	hashIsDirty = true;
	QAbstractItemView::rowsInserted( parent, start, end );
};

void NBIconView::rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end ) {

	hashIsDirty = true;
	QAbstractItemView::rowsAboutToBeRemoved( parent, start, end );
};

void NBIconView::updateGeometries() {

	computeRowsAndColumns();

	if ( idealHeight <= viewport()->height() ) {
		verticalScrollBar()->setValue( 0 );
		verticalScrollBar()->hide();
	}

	else {
		verticalScrollBar()->show();
		verticalScrollBar()->setSingleStep( myGridSize.height() );
		verticalScrollBar()->setPageStep( viewport()->height() );
		verticalScrollBar()->setRange( 0, qMax( 0, idealHeight - viewport()->height() ) );
	}
};

void NBIconView::reload() {

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QModelIndex NBIconView::moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers ) {

	if ( cModel->isCategorizationEnabled() )
		return moveCursorCategorized( cursorAction );

	else
		return moveCursorNonCategorized( cursorAction );
};

int NBIconView::horizontalOffset() const {

	return horizontalScrollBar()->value();
};

int NBIconView::verticalOffset() const {

	return verticalScrollBar()->value();
};

void NBIconView::scrollContentsBy( int dx, int dy ) {

	scrollDirtyRegion( dx, dy );
	viewport()->scroll( dx, dy );
};

void NBIconView::setSelection( const QRect &rect, QFlags<QItemSelectionModel::SelectionFlag> flags ) {

	QRect rectangle = rect.translated( horizontalScrollBar()->value(), verticalScrollBar()->value() ).normalized();

	calculateRectsIfNecessary();
	QHashIterator<int, QPoint> i( rectForRow );
	int firstRow = cModel->rowCount();
	int lastRow = -1;
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( i.value(), myItemSize ).intersects( rectangle ) ) {
			firstRow = firstRow < i.key() ? firstRow : i.key();
			lastRow = lastRow > i.key() ? lastRow : i.key();
		}
	}

	if ( firstRow != cModel->rowCount() && lastRow != -1 ) {
		QItemSelection selection( cModel->index( firstRow, 0, rootIndex() ), cModel->index( lastRow, 0, rootIndex() ) );
		selectionModel()->select( selection, flags );
	}

	else {
		QModelIndex invalid;
		QItemSelection selection( invalid, invalid );
		selectionModel()->select( selection, flags );
	}
};

QRegion NBIconView::visualRegionForSelection( const QItemSelection &selection ) const {

	QRegion region;
	foreach ( const QItemSelectionRange &range, selection ) {
		for ( int row = range.top(); row <= range.bottom(); ++row ) {
			for ( int column = range.left(); column < range.right(); ++column ) {
				QModelIndex index = cModel->index( row, column, rootIndex() );
				region += visualRect( index );
			}
		}
	}
	return region;
};

void NBIconView::paintEvent( QPaintEvent* event ) {

	QPainter painter( viewport() );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	/* We need to draw the categories only if the model is categorization enabled */
	if ( cModel->isCategorizationEnabled() ) {
		for ( int catIdx = 0; catIdx < cModel->categories().count(); catIdx++ ) {

			QRect rect = categoryRect( catIdx );
			if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
				continue;

			paintCategory( &painter, rect, cModel->categories().at( catIdx ) );
		}
	}

	for ( int row = 0; row < cModel->rowCount( rootIndex() ); row++ ) {
		QModelIndex index = cModel->index( row, 0, rootIndex() );

		if ( not cModel->isCategoryVisible( cModel->category( index ) ) )
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

void NBIconView::resizeEvent( QResizeEvent* ) {

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( mpEvent->button() == Qt::LeftButton )
		dragStartPosition = mpEvent->pos();

	else
		dragStartPosition = QPoint();

	if ( mpEvent->button() == Qt::RightButton ) {
		mpEvent->ignore();
		return;
	}

	QModelIndex idx = indexAt( mpEvent->pos() );
	QItemSelectionModel *selector = selectionModel();
	if ( selector->isSelected( idx ) )
		selector->select( idx, QItemSelectionModel::SelectCurrent );

	else
		selector->select( idx, QItemSelectionModel::Deselect );

	QAbstractItemView::mousePressEvent( mpEvent );
};

void NBIconView::mouseMoveEvent( QMouseEvent *mmEvent ) {

	if ( not ( mmEvent->buttons() & Qt::LeftButton ) ) {

		QAbstractItemView::mouseMoveEvent( mmEvent );
		return;
	}

	else if ( dragStartPosition.isNull() ) {

		QAbstractItemView::mouseMoveEvent( mmEvent );
		return;
	}

	else if ( ( mmEvent->pos() - dragStartPosition ).manhattanLength() < QApplication::startDragDistance() ) {

		QAbstractItemView::mouseMoveEvent( mmEvent );
		return;
	}

	if ( selectionModel()->hasSelection() ) {

		QDrag *drag = new QDrag( this );

		QList<QUrl> urlList;
		foreach( QModelIndex item, selectionModel()->selectedRows() )
			urlList << QUrl( cModel->nodePath( item.data().toString() ) );

		QMimeData *mimedata = new QMimeData();
		mimedata->setUrls( urlList );

		drag->setMimeData( mimedata );

		drag->exec( Qt::CopyAction | Qt::MoveAction | Qt::LinkAction );
	}

	mmEvent->accept();
};

void NBIconView::mouseDoubleClickEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		QModelIndex idx = indexAt( mEvent->pos() );
		if ( idx.isValid() )
			emit open( idx );
	}

	mEvent->accept();
};

void NBIconView::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBIconView::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	const QMimeData *mData = dmEvent->mimeData();
	if ( not mData->hasUrls() )
		return;

	QString source = dirName( mData->urls().at( 0 ).toLocalFile() );
	source += ( source.endsWith( "/" ) ? "" : "/" );

	if ( source != cModel->currentDir() ) {
		if ( isWritable( cModel->currentDir() ) ) {
			dmEvent->setDropAction( Qt::CopyAction );
			dmEvent->accept();
		}

		else
			dmEvent->ignore();
	}

	else if ( indexAt( dmEvent->pos() ).isValid() ) {
		QModelIndex idx = indexAt( dmEvent->pos() );
		QString mtpt = cModel->nodePath( cModel->data( idx ).toString() );

		if ( isWritable( mtpt ) and isDir( mtpt ) ) {
			dmEvent->setDropAction( Qt::CopyAction );
			dmEvent->accept();
		}

		else
			dmEvent->ignore();
	}

	else
		dmEvent->ignore();
};

void NBIconView::dropEvent( QDropEvent *dpEvent ) {

	QModelIndex idx = indexAt( dpEvent->pos() );
	QString mtpt = cModel->nodePath( cModel->data( idx ).toString() );

	if ( not isDir( mtpt ) ) {
		dpEvent->ignore();
		return;
	}

	const QMimeData *mData = dpEvent->mimeData();
	if ( mData->hasUrls() ) {

		QStringList args;
		foreach( QUrl url, mData->urls() )
			args << url.toLocalFile();

		if ( dpEvent->keyboardModifiers() == Qt::ShiftModifier ) {

			qDebug() << "DnD Move";
			emit move( args, mtpt );
		}

		else if ( ( dpEvent->keyboardModifiers() == Qt::ControlModifier ) or ( dpEvent->keyboardModifiers() == Qt::NoModifier ) ) {

			qDebug() << "DnD Copy";
			emit copy( args, mtpt );
		}

		else if ( dpEvent->keyboardModifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) ) {

			qDebug() << "DnD Link";
			emit link( args, mtpt );
		}

		else if ( dpEvent->keyboardModifiers() == ( Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier ) ) {

			qDebug() << "Ctrl+Shift+Alt+Drop. Alphabetical Copy activated";
			QProcess::startDetached( "sh", QStringList() << "find -type f -print0 | sort -z | cpio -0 -pd " + mtpt );
			emit acopy( args, mtpt );
		}

		else {

			dpEvent->ignore();
			return;
		}
	}

	dpEvent->accept();
};

QModelIndex NBIconView::moveCursorCategorized( QAbstractItemView::CursorAction cursorAction ) {

	QModelIndex idx = currentIndex();
	if ( idx.isValid() ) {

		QStringList categoryList = cModel->categories();
		switch( cursorAction ) {
			case QAbstractItemView::MoveRight: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < cModel->rowCount() - 1 )
					return cModel->index( idx.row() + 1, 0, rootIndex() );

				/* Current index is the last visible index */
				else
					return cModel->index( 0, 0, rootIndex() );
			}

			case QAbstractItemView::MoveLeft: {
				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < cModel->rowCount() )
					return cModel->index( idx.row() - 1, 0, rootIndex() );

				/* The current index is the first one */
				else
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
			}

			case QAbstractItemView::MoveDown: {

				int thisCategoryIdx = cModel->categoryIndex( idx );
				/* If we in the last category go to the first one */
				int nextCategoryIdx = ( thisCategoryIdx == cModel->categoryCount() - 1 ? 0 : thisCategoryIdx + 1 );

				QString thisCategory = cModel->category( idx );
				QString nextCategory = categoryList.value( nextCategoryIdx );

				QModelIndexList thisCategoryIndexes = cModel->indexListForCategory( thisCategory );
				QModelIndexList nextCategoryIndexes = cModel->indexListForCategory( nextCategory );

				// Calculate the visual row of this index
				int rowInCategory = thisCategoryIndexes.indexOf( idx );
				int vrow = rowInCategory / itemsPerRow;
				int vcol = rowInCategory % itemsPerRow;
				int nrow = ( int )ceil( 1.0 * thisCategoryIndexes.count() / itemsPerRow );

				int newVRow = vrow + 1;

				if ( ( newVRow * itemsPerRow + vcol ) < thisCategoryIndexes.count() ) {
					// We have an idx below this idx from the same category
					return thisCategoryIndexes.value( newVRow * itemsPerRow + vcol );
				}

				else if ( newVRow < nrow ) {
					// We have indexes with row greater than this one, not below this one
					return thisCategoryIndexes.last();
				}

				else {
					// We need to return the idx from the next category same or lower column
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
				int thisCategoryIdx = cModel->categoryIndex( idx );
				/* If we in the first category go to the last one */
				int prevCategoryIdx = ( thisCategoryIdx == 0 ? cModel->categoryCount() - 1 : thisCategoryIdx - 1 );

				QString thisCategory = cModel->category( idx );
				QString prevCategory = categoryList.value( prevCategoryIdx );

				QModelIndexList thisCategoryIndexes = cModel->indexListForCategory( thisCategory );
				QModelIndexList prevCategoryIndexes = cModel->indexListForCategory( prevCategory );

				// Calculate the visual row of this index
				int rowInCategory = thisCategoryIndexes.indexOf( idx );
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

				verticalScrollBar()->setValue( 0 );
				return cModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
			}

			case QAbstractItemView::MoveNext: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < cModel->rowCount() - 1 )
					return cModel->index( idx.row() + 1, 0, rootIndex() );

				/* Current index is the last visible index */
				else
					return cModel->index( 0, 0, rootIndex() );
			}

			case QAbstractItemView::MovePrevious: {

				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < cModel->rowCount() )
					return cModel->index( idx.row() - 1, 0, rootIndex() );

				/* The current index is the first one */
				else
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
			}

			default: {
				return QModelIndex();
			}
		}
	}

	else {
		switch( cursorAction ) {
			case QAbstractItemView::MoveHome: {

				return cModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
			}

			default: {

				return cModel->index( 0, 0, rootIndex() );
			}
		}
	}

	return idx;
};

QModelIndex NBIconView::moveCursorNonCategorized( QAbstractItemView::CursorAction cursorAction ) {

	QModelIndex idx = currentIndex();
	if ( idx.isValid() ) {

		QStringList categoryList = cModel->categories();
		switch( cursorAction ) {
			case QAbstractItemView::MoveRight: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < cModel->rowCount() - 1 )
					return cModel->index( idx.row() + 1, 0, rootIndex() );

				/* Current index is the last visible index */
				else
					return cModel->index( 0, 0, rootIndex() );
			}

			case QAbstractItemView::MoveLeft: {
				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < cModel->rowCount() )
					return cModel->index( idx.row() - 1, 0, rootIndex() );

				/* The current index is the first one */
				else
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
			}

			case QAbstractItemView::MoveDown: {
				int nextRow = idx.row() + itemsPerRow;
				if ( nextRow >= cModel->rowCount() )
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );

				else
					return cModel->index( nextRow, 0, rootIndex() );
			}

			case QAbstractItemView::MoveUp: {
				int prevRow = idx.row() - itemsPerRow;
				if ( prevRow < 0 )
					return cModel->index( 0, 0, rootIndex() );

				else
					return cModel->index( prevRow, 0, rootIndex() );
			}

			case QAbstractItemView::MoveHome: {

				verticalScrollBar()->setValue( 0 );
				return cModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
			}

			case QAbstractItemView::MoveNext: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < cModel->rowCount() - 1 )
					return cModel->index( idx.row() + 1, 0, rootIndex() );

				/* Current index is the last visible index */
				else
					return cModel->index( 0, 0, rootIndex() );
			}

			case QAbstractItemView::MovePrevious: {

				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < cModel->rowCount() )
					return cModel->index( idx.row() - 1, 0, rootIndex() );

				/* The current index is the first one */
				else
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
			}

			default: {
				return cModel->index( 0, 0, rootIndex() );
			}
		}
	}

	else {
		switch( cursorAction ) {
			case QAbstractItemView::MoveHome: {

				return cModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
			}

			default: {

				return cModel->index( 0, 0, rootIndex() );
			}
		}
	}

	return idx;
};

void NBIconView::calculateRectsIfNecessary() const {

	if ( not hashIsDirty )
		return;

	computeRowsAndColumns();

	if ( cModel->isCategorizationEnabled() )
		calculateCategorizedRects();

	else
		calculateNonCategorizedRects();

	if ( idealHeight <= viewport()->height() ) {
		verticalScrollBar()->hide();
	}

	else {
		verticalScrollBar()->show();
		verticalScrollBar()->setSingleStep( myGridSize.height() );
		verticalScrollBar()->setPageStep( viewport()->height() );
		verticalScrollBar()->setRange( 0, qMax( 0, idealHeight - viewport()->height() ) );
	}
};

void NBIconView::calculateCategorizedRects() const {

	int x = 0, y = 0, prevRows = 0, totalRows = 0;
	QStringList categoryList = cModel->categories();

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = cModel->indexListForCategory( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		int minX = myContentsMargins.left();
		int minY = myContentsMargins.top() + catIdx * myCategoryHeight + catIdx * myCategorySpacing + totalRows * myGridSize.height();

		int categoryWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
		rectForCategory[ catIdx ] = QRect( minX, minY, categoryWidth, 24 );

		if ( not cModel->isCategoryVisible( cModel->category( mList.value( 0 ) ) ) )
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

			// qApp->processEvents();
		}
	}

	idealHeight = myContentsMargins.top() + myContentsMargins.bottom() + categoryList.count() * ( myCategoryHeight + myCategorySpacing );
	idealHeight += totalRows * myGridSize.height();

	hashIsDirty = false;
	viewport()->update();
};

void NBIconView::calculateNonCategorizedRects() const {

	int x = 0, y = 0, totalRows = 0;

	for( int lrow = 0; lrow < cModel->rowCount(); lrow++ ) {

		totalRows = ( lrow + 1 ) / itemsPerRow;
		if ( ( lrow + 1 ) % itemsPerRow )
			totalRows++;

		// Minimum X and Y for Category Rectangle
		int minX = myContentsMargins.left();
		int minY = myContentsMargins.top() + ( totalRows - 1 ) * myGridSize.height();

		int col = lrow % itemsPerRow;

		x = minX + col * myGridSize.width() + 5;
		y = minY + 5;

		rectForRow[ lrow ] = QPoint( x, y );

		// qApp->processEvents();
	}

	idealHeight = myContentsMargins.top() + numberOfRows * myGridSize.height() + myContentsMargins.bottom();

	hashIsDirty = false;
	viewport()->update();
};

void NBIconView::computeRowsAndColumns() const {

	int vWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
	vWidth = vWidth - myInlayMargins.left() - myInlayMargins.right();

	itemsPerRow = qMax( 1, vWidth / myGridSizeMin.width() );

	numberOfRows = ( int )( cModel->rowCount() / itemsPerRow );
	if ( ( cModel->rowCount() % itemsPerRow ) > 0 )
		numberOfRows++;

	int newGridWidth = ( int )( vWidth / itemsPerRow );
	myGridSize = QSize( newGridWidth, myGridSizeMin.height() );
	myItemSize = myGridSize - QSize( 10, 10 );
};

QRect NBIconView::viewportRectForRow( int row ) const {

	calculateRectsIfNecessary();
	QPoint pt = rectForRow.value( row );
	if ( !QRect( pt, myGridSize ).isValid() )
		return QRect();

    return QRect( pt.x(), pt.y() - verticalScrollBar()->value(), myItemSize.width(), myItemSize.height() );
};

void NBIconView::paintCategory( QPainter *painter, const QRect &rectangle, const QString &text ) const {

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

QPixmap NBIconView::pixmapForCategory( QString categoryText ) const {

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
		return QIcon::fromTheme( "preferences-system" ).pixmap( 16, 16 );

	else if ( categoryText == "System" )
		return QIcon::fromTheme( "applications-system" ).pixmap( 16, 16 );

	else
		return QIcon::fromTheme( "preferences-other" ).pixmap( 16, 16 );
};
