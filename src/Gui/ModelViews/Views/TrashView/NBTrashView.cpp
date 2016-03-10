/*
	*
	* NBTrashView.cpp - NewBreeze TrashView Class
	*
*/

#include <NBTrashView.hpp>

NBTrashView::NBTrashView( NBTrashModel *model ) : QAbstractItemView() {

	idealHeight = 0;
	hashIsDirty = true;

	// Category Height
	myCategoryHeight = 24;

	// Category Spacing
	myCategorySpacing = 10;

	// Contents Margins
	myContentsMargins = QMargins( 10, 10, 10, 10 );

	// Inlay Margins
	myInlayMargins = QMargins( 10, 0, 10, 0 );

	// Grid Size
	myGridSizeMin = QSize( 120, 80 );
	myGridSize = QSize( 120, 80 );
	myItemSize = QSize( 110, 70 );

	// Icon Size
	myIconSize = QSize( 48, 48 );

	// Persistent vertical column
	persistentVCol = 0;

	// Items per cisual row
	itemsPerRow = 1;
	numberOfRows = 0;
	padding = 0;

	// Item Model
	tModel = model;
	QAbstractItemView::setModel( model );

	// Set the Apps Delegate
	setItemDelegate( new NBTrashDelegate() );

	// Icon Size
	setIconSize( QSize( 48, 48 ) );

	// Update View
	updateViewMode();

	// Selection
	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setSelectionBehavior( QAbstractItemView::SelectRows );

	// Internal Object Name
	setObjectName( "mainList" );

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
	viewport()->setAcceptDrops( false );
	setDragEnabled( false );
	setAcceptDrops( false );
	setDragDropMode( QListView::NoDragDrop );
	setDropIndicatorShown( false );

	// Context Menu
	setContextMenuPolicy( Qt::CustomContextMenu );

	connect(
			this, SIGNAL( customContextMenuRequested( QPoint ) ),
			this, SIGNAL( contextMenuRequested( QPoint ) )
	);

	connect( tModel, SIGNAL( trashLoaded() ), this, SLOT( reload() ) );

	// Zoom In and Out actions
	QAction *zoomInAct = new QAction( "Zoom In", this );
	zoomInAct->setShortcut( QKeySequence::ZoomIn );
	connect( zoomInAct, SIGNAL( triggered() ), this, SLOT( zoomIn() ) );
	addAction( zoomInAct );

	QAction *zoomOutAct = new QAction( "Zoom Out", this );
	zoomOutAct->setShortcut( QKeySequence::ZoomOut );
	connect( zoomOutAct, SIGNAL( triggered() ), this, SLOT( zoomOut() ) );
	addAction( zoomOutAct );

	// Item Selection Rectangle
	rBand = new QRubberBand( QRubberBand::Rectangle, this );
};

void NBTrashView::updateViewMode() {

	computeGridSize( myIconSize );

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

int NBTrashView::categoryHeight() const {

	return myCategoryHeight;
};

void NBTrashView::setCategoryHeight( int newCategoryHeight ) {

	myCategoryHeight = newCategoryHeight;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QSize NBTrashView::iconSize() const {

	return myIconSize;
};

void NBTrashView::setIconSize( QSize newIconSize ) {

	myIconSize = newIconSize;
	computeGridSize( myIconSize );

	viewport()->update();
};

void NBTrashView::setIconSize( int nWidth, int nHeight ) {

	setIconSize( QSize( nWidth, nHeight ) );
};

QMargins NBTrashView::contentsMargins() const {

	return myContentsMargins;
};

void NBTrashView::setContentsMargins( QMargins newContentsMargins ) {

	myContentsMargins = newContentsMargins;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBTrashView::setContentsMargins( int nLeft, int nTop, int nRight, int nBottom ) {

	setContentsMargins( QMargins( nLeft, nTop, nRight, nBottom ) );
};

QMargins NBTrashView::inlayMargins() const {

	return myInlayMargins;
};

void NBTrashView::setInlayMargins( QMargins newInlayMargins ) {

	myInlayMargins = newInlayMargins;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBTrashView::setInlayMargins( int nLeft, int nTop, int nRight, int nBottom ) {

	setInlayMargins( QMargins( nLeft, nTop, nRight, nBottom ) );

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

int NBTrashView::categorySpacing() const {

	return myCategorySpacing;
};

void NBTrashView::setCategorySpacing( int newCategorySpacing ) {

	myCategorySpacing = newCategorySpacing;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QRect NBTrashView::visualRect( const QModelIndex &index ) const {

	QRect rect;
	if ( index.isValid() )
		rect = viewportRectForRow( index.row() );

	return rect;
};

QRect NBTrashView::categoryRect( int categoryIndex ) const {

	calculateRectsIfNecessary();
	QRect rect = rectForCategory.value( categoryIndex );
	if ( not rect.isValid() )
		return QRect();

    return QRect( rect.x(), rect.y() - verticalScrollBar()->value(), rect.width(), rect.height() );
};

void NBTrashView::scrollTo( const QModelIndex &index, QAbstractItemView::ScrollHint ) {

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

QModelIndex NBTrashView::indexAt( const QPoint &point_ ) const {

	QPoint point( point_ );
	point.ry() += verticalScrollBar()->value();

	calculateRectsIfNecessary();
	QHashIterator<int, QPoint> i( rectForRow );
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( i.value(), myGridSize ).adjusted( padding / 2, padding / 2, -padding / 2, -padding / 2 ).contains( point ) )
			return tModel->index( i.key(), 0, rootIndex() );
	}

	return QModelIndex();
};

QString NBTrashView::categoryAt( const QPoint &point_ ) const {

	QPoint point( point_ );
	point.ry() += verticalScrollBar()->value();
	calculateRectsIfNecessary();
	QHashIterator<int, QRect> i( rectForCategory );
	while ( i.hasNext() ) {

		i.next();
		if ( i.value().contains( point ) )
			return tModel->categories().value( i.key() );
	}

	return QString();
};

void NBTrashView::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight ) {

	hashIsDirty = true;
	QAbstractItemView::dataChanged( topLeft, bottomRight );
};

void NBTrashView::rowsInserted( const QModelIndex &parent, int start, int end ) {

	hashIsDirty = true;
	QAbstractItemView::rowsInserted( parent, start, end );
};

void NBTrashView::rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end ) {

	hashIsDirty = true;
	QAbstractItemView::rowsAboutToBeRemoved( parent, start, end );
};

void NBTrashView::updateGeometries() {

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

void NBTrashView::reload() {

	hashIsDirty = true;
	persistentVCol = 0;

	rectForRow.clear();
	rectForCategory.clear();
	repaint();

	calculateRectsIfNecessary();
};

QModelIndex NBTrashView::moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers ) {

	return moveCursorCategorized( cursorAction );
};

int NBTrashView::horizontalOffset() const {

	return horizontalScrollBar()->value();
};

int NBTrashView::verticalOffset() const {

	return verticalScrollBar()->value();
};

void NBTrashView::scrollContentsBy( int dx, int dy ) {

	scrollDirtyRegion( dx, dy );
	viewport()->scroll( dx, dy );
};

void NBTrashView::setSelection( const QRect &rect, QFlags<QItemSelectionModel::SelectionFlag> flags ) {

	QRect rectangle = rect.translated( horizontalScrollBar()->value(), verticalScrollBar()->value() ).normalized();

	calculateRectsIfNecessary();
	QHashIterator<int, QPoint> i( rectForRow );
	int firstRow = tModel->rowCount();
	int lastRow = -1;
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( i.value(), myGridSize ).intersects( rectangle ) ) {
			firstRow = firstRow < i.key() ? firstRow : i.key();
			lastRow = lastRow > i.key() ? lastRow : i.key();
		}
	}

	if ( firstRow != tModel->rowCount() && lastRow != -1 ) {
		QItemSelection selection( tModel->index( firstRow, 0, rootIndex() ), tModel->index( lastRow, 0, rootIndex() ) );
		selectionModel()->select( selection, flags );
	}

	else {
		QModelIndex invalid;
		QItemSelection selection( invalid, invalid );
		selectionModel()->select( selection, flags );
	}
};

QRegion NBTrashView::visualRegionForSelection( const QItemSelection &selection ) const {

	QRegion region;
	foreach ( const QItemSelectionRange &range, selection ) {
		for ( int row = range.top(); row <= range.bottom(); ++row ) {
			for ( int column = range.left(); column < range.right(); ++column ) {
				QModelIndex index = tModel->index( row, column, rootIndex() );
				region += visualRect( index );
			}
		}
	}
	return region;
};

void NBTrashView::paintEvent( QPaintEvent* event ) {

	QPainter painter( viewport() );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	/* We need to draw the categories only if the model is categorization enabled */
	for ( int catIdx = 0; catIdx < tModel->categories().count(); catIdx++ ) {

		QRect rect = categoryRect( catIdx );
		if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
			continue;

		paintCategory( &painter, rect, tModel->categories().at( catIdx ) );
	}

	for ( int row = 0; row < tModel->rowCount( rootIndex() ); row++ ) {
		QModelIndex idx = tModel->index( row, 0, rootIndex() );

		QRect rect = viewportRectForRow( row );
		if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
			continue;

		QStyleOptionViewItem option = viewOptions();
		option.rect = rect;

		if ( selectionModel()->isSelected( idx ) )
			option.state |= QStyle::State_Selected;

		if ( currentIndex() == idx )
			option.state |= QStyle::State_HasFocus;

		QPoint mousePos = mapFromGlobal( cursor().pos() );
		if ( rect.contains( mousePos ) )
			option.state |= QStyle::State_MouseOver;

		option.decorationSize = myIconSize;

		/* Palette */
		QPalette pltt = qApp->palette();

		/* Dark text colors will suit here */
		if ( isBrightColor( pltt.color( QPalette::Base ), pltt.color( QPalette::Highlight ) ) ) {
			if ( option.state & QStyle::State_Selected )
				pltt.setColor( QPalette::Text, pltt.color( QPalette::HighlightedText ) );

			else
				pltt.setColor( QPalette::Text, palette().color( QPalette::Text ) );

			/* Bright text will be used for drawing the 'current rect' */
			pltt.setColor( QPalette::BrightText, pltt.color( QPalette::Highlight ).darker() );

			/* ButtonText will be used to paint the extra details */
			pltt.setColor( QPalette::BrightText, pltt.color( QPalette::Text ).lighter( 135 ) );
		}

		/* Light text colors to be used here */
		else {
			if ( option.state & QStyle::State_Selected )
				pltt.setColor( QPalette::Text, pltt.color( QPalette::HighlightedText ) );

			else
				pltt.setColor( QPalette::Text, palette().color( QPalette::Text ) );

			/* Bright text will be used for drawing the 'current rect' */
			pltt.setColor( QPalette::BrightText, pltt.color( QPalette::Highlight ).lighter() );
		}

		option.palette = pltt;

		itemDelegate()->paint( &painter, option, idx );
	}

	event->accept();
};

void NBTrashView::resizeEvent( QResizeEvent* ) {

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBTrashView::mousePressEvent( QMouseEvent *mpEvent ) {

	// Do nothing whenever mouse is not left clicked
	if ( mpEvent->button() != Qt::LeftButton ) {
		mpEvent->ignore();
		return;
	}

	// Start Drag drop
	dragStartPosition = mpEvent->pos();

	QModelIndex idx = indexAt( mpEvent->pos() );
	if ( not idx.isValid() ) {
		/* Click did not land on an index. Draw an item selection rectangle */
		selectionModel()->clearSelection();
		repaint();

		rBand->setGeometry( QRect( dragStartPosition, QSize() ) );
		rBand->show();
	}

	else {

		return;
	}

	QAbstractItemView::mousePressEvent( mpEvent );
};

void NBTrashView::mouseMoveEvent( QMouseEvent *mmEvent ) {

	if ( not ( mmEvent->buttons() & Qt::LeftButton ) ) {

		QAbstractItemView::mouseMoveEvent( mmEvent );
		return;
	}

	else if ( dragStartPosition.isNull() ) {

		QAbstractItemView::mouseMoveEvent( mmEvent );
		return;
	}

	else if ( not ( mmEvent->pos() - dragStartPosition ).manhattanLength() ) {

		QAbstractItemView::mouseMoveEvent( mmEvent );
		return;
	}

	QPoint rEnd = mmEvent->pos();
	if ( dragStartPosition.x() < rEnd.x() and dragStartPosition.y() < rEnd.y() )
		rBand->setGeometry( QRect( dragStartPosition, rEnd ) );

	else if ( dragStartPosition.x() > rEnd.x() and dragStartPosition.y() > rEnd.y() )
		rBand->setGeometry( QRect( rEnd, dragStartPosition ) );

	else if ( dragStartPosition.x() < rEnd.x() and dragStartPosition.y() > rEnd.y() ) {
		QPoint tl( dragStartPosition.x(), rEnd.y() );
		QPoint br( rEnd.x(), dragStartPosition.y() );

		rBand->setGeometry( QRect( tl, br ) );
	}

	else if ( dragStartPosition.x() > rEnd.x() and dragStartPosition.y() < rEnd.y() ) {
		QPoint tl( rEnd.x(), dragStartPosition.y() );
		QPoint br( dragStartPosition.x(), rEnd.y() );

		rBand->setGeometry( QRect( tl, br ) );
	}

	setSelection( rBand->geometry(), QItemSelectionModel::ClearAndSelect );
	repaint();

	mmEvent->accept();
};

void NBTrashView::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	/* If its just a click and not a drag */
	if ( dragStartPosition == mrEvent->pos() ) {
		QModelIndex idx = indexAt( mrEvent->pos() );
		if ( idx.isValid() ) {
			/* Click landed on an index. Select / deselect the item(s) */
			QModelIndexList selected = selectionModel()->selectedRows( 0 );

			/* This is the starting index in the selection */
			QModelIndex start = ( selected.count() ? selected.at( 0 ) : currentIndex() );
			if ( not start.isValid() )
				start = tModel->index( 0, 0, rootIndex() );

			/* On click-release, we make the index below the mouse as the current */
			setCurrentIndex( idx );

			/* If the index below the mouse is selected, deselect it and vice-versa */
			if ( qApp->keyboardModifiers() & Qt::ControlModifier ) {

				/* Index below the mouse is selected, deselect it */
				if ( selected.contains( idx ) )
					selected.removeAll( idx );

				/* Index below the mouse is no selected, select it */
				else
					selected << idx;

				/* Prepare the selection */
				QItemSelection selection;
				Q_FOREACH( QModelIndex sidx, selected )
					selection.select( sidx, sidx );

				/* Apply the selection */
				selectionModel()->select( selection, QItemSelectionModel::Select );
			}

			/* Continuous Selection */
			else if ( qApp->keyboardModifiers() & Qt::ShiftModifier ) {
				QItemSelection selection( start, idx );
				selectionModel()->clearSelection();
				selectionModel()->select( selection, QItemSelectionModel::Select );
			}
		}
	}

	if ( rBand->isVisible() )
		rBand->hide();

	mrEvent->accept();
};

void NBTrashView::mouseDoubleClickEvent( QMouseEvent *mEvent ) {

	mEvent->accept();
};

void NBTrashView::computeGridSize( QSize iconSize ) {

	int availWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
	padding = ( int ) round( iconSize.width() * 0.1 );
	myGridSizeMin = QSize( availWidth, iconSize.height() + padding * 2 );

	computeRowsAndColumns();

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QModelIndex NBTrashView::moveCursorCategorized( QAbstractItemView::CursorAction cursorAction ) {

	QModelIndex idx = currentIndex();
	QStringList categoryList = tModel->categories();

	/* If there exists a current index */
	if ( idx.isValid() ) {

		int thisCategoryIdx = tModel->categoryIndex( idx );
		/* If we are in the first category go to the last one */
		int prevCategoryIdx = ( thisCategoryIdx == 0 ? tModel->categoryCount() - 1 : thisCategoryIdx - 1 );
		/* If we are in the last category go to the first one */
		int nextCategoryIdx = ( ( thisCategoryIdx == tModel->categoryCount() - 1 ) ? 0 : thisCategoryIdx + 1 );

		QString prevCategory = categoryList.value( prevCategoryIdx );
		QString thisCategory = tModel->category( idx );
		QString nextCategory = categoryList.value( nextCategoryIdx );

		QModelIndexList prevCategoryIndexes = tModel->indexListForCategory( prevCategory );
		QModelIndexList thisCategoryIndexes = tModel->indexListForCategory( thisCategory );
		QModelIndexList nextCategoryIndexes = tModel->indexListForCategory( nextCategory );

		// Calculate the visual row of this index
		int rowInCategory = thisCategoryIndexes.indexOf( idx );

		switch( cursorAction ) {
			case QAbstractItemView::MoveNext:
			case QAbstractItemView::MoveRight: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < tModel->rowCount() - 1 ) {
					/* If this is the visual index of the catgory, then next persistentVCol will be zero */
					if ( idx == thisCategoryIndexes.last() )
						persistentVCol = 0;

					else
						persistentVCol = ( rowInCategory + 1 ) % itemsPerRow;

					return tModel->index( idx.row() + 1, 0, rootIndex() );
				}

				/* Current index is the last visible index */
				else {
					persistentVCol = 0;
					return tModel->index( 0, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MovePrevious:
			case QAbstractItemView::MoveLeft: {
				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < tModel->rowCount() ) {
					if ( idx == thisCategoryIndexes.first() )
						persistentVCol = ( prevCategoryIndexes.count() - 1 ) % itemsPerRow;

					else
						persistentVCol = ( rowInCategory - 1 ) % itemsPerRow;
					return tModel->index( idx.row() - 1, 0, rootIndex() );
				}

				/* The current index is the first one */
				else {
					// #warning "FIXME: This implementation is buggy"
					persistentVCol = ( prevCategoryIndexes.count() - 1 ) % itemsPerRow;
					return tModel->index( tModel->rowCount() - 1, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveDown: {

				if ( idx.row() == tModel->rowCount() - 1 ) {
					return tModel->index( 0, 0, idx.parent() );
				}
				else {
					return tModel->index( idx.row() + 1, 0, idx.parent() );
				}
			}

			case QAbstractItemView::MoveUp: {

				if ( idx.row() == 0 ) {
					return tModel->index( tModel->rowCount() - 1, 0, idx.parent() );
				}
				else {
					return tModel->index( idx.row() - 1, 0, idx.parent() );
				}
			}

			case QAbstractItemView::MoveHome: {

				verticalScrollBar()->setValue( 0 );
				persistentVCol = 0;
				return tModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				persistentVCol = ( tModel->indexListCountForCategory( categoryList.last() ) - 1 ) % itemsPerRow;
				return tModel->index( tModel->rowCount() - 1, 0, idx.parent() );
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
				return tModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				persistentVCol = ( tModel->indexListCountForCategory( categoryList.last() ) - 1 ) % itemsPerRow;
				return tModel->index( tModel->rowCount() - 1, 0, idx.parent() );
			}

			default: {

				return tModel->index( 0, 0, rootIndex() );
			}
		}
	}

	return idx;
};

void NBTrashView::calculateRectsIfNecessary() const {

	if ( not hashIsDirty )
		return;

	computeRowsAndColumns();

	rectForRow.clear();
	rectForCategory.clear();

	calculateCategorizedRects();

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

void NBTrashView::calculateCategorizedRects() const {

	if ( not hashIsDirty )
		return;

	int y = 0, prevRows = 0, totalRows = 0;
	QStringList categoryList = tModel->categories();

	int catX = myContentsMargins.left() ;
	int catW = myGridSize.width() + myInlayMargins.left() + myInlayMargins.right();
	int x = myContentsMargins.left() + myInlayMargins.left();

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = tModel->indexListForCategory( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		int minY = myContentsMargins.top() + catIdx * myCategoryHeight + catIdx * myCategorySpacing + totalRows * myGridSize.height();

		rectForCategory[ catIdx ] = QRect( catX, minY, catW, myCategoryHeight );

		// Mimimum X and Y for indexes
		minY += myCategoryHeight;

		prevRows = tModel->indexListCountForCategory( categoryList.at( catIdx ) );
		totalRows += prevRows;

		for( int lrow = 0; lrow < mList.count(); lrow++ ) {
			y = minY + lrow * myGridSize.height();

			rectForRow[ mList[ lrow ].row() ] = QPoint( x, y );
		}
	}

	// Top margin size
	idealHeight = myContentsMargins.top();
	// Category Heights
	idealHeight += categoryList.count() * myCategoryHeight;
	// Category Spacings
	idealHeight += categoryList.count() * myCategorySpacing;
	// Bottom Margin Size
	idealHeight += myContentsMargins.bottom();
	// All item sizes
	idealHeight += tModel->rowCount() * myGridSize.height();

	hashIsDirty = false;
	viewport()->update();
};

void NBTrashView::computeRowsAndColumns() const {

	int vWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
	vWidth = vWidth - myInlayMargins.left() - myInlayMargins.right();

	itemsPerRow = 1;
	numberOfRows = tModel->rowCount();

	int newGridWidth = ( int )( vWidth / itemsPerRow );
	myGridSize = QSize( newGridWidth, myGridSizeMin.height() );
};

QRect NBTrashView::viewportRectForRow( int row ) const {

	calculateRectsIfNecessary();
	QPoint pt = rectForRow.value( row );
	if ( !QRect( pt, myGridSize ).isValid() )
		return QRect();

    return QRect( pt.x(), pt.y() - verticalScrollBar()->value(), myGridSize.width(), myGridSize.height() );
};

void NBTrashView::paintCategory( QPainter *painter, const QRect &rectangle, const QString &text ) const {

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

QPixmap NBTrashView::pixmapForCategory( QString categoryText ) const {

	Q_UNUSED( categoryText );

	return QIcon::fromTheme( "arrow-right" ).pixmap( 16, 16 );
};

void NBTrashView::zoomIn() {

	if ( myIconSize.width() >= 64 )
		setIconSize( QSize( 64, 64 ) );

	else
		setIconSize( myIconSize + QSize( 4, 4 ) );
};

void NBTrashView::zoomOut() {

	if ( myIconSize.width() <= 16 )
		setIconSize( QSize( 16, 16 ) );

	else
		setIconSize( myIconSize - QSize( 4, 4 ) );
};
