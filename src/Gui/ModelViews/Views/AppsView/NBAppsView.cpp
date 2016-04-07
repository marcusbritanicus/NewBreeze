/*
	*
	* NBAppsView.cpp - NewBreeze FolderView Class
	*
*/

#include <NBAppsView.hpp>

NBApplicationsView::NBApplicationsView() : QAbstractItemView() {

	// Current folder viewMode
	currentViewMode = Settings->General.ViewMode;

	// Icon rects
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

	// Persistent vertical column
	persistentVCol = 0;

	// Items per visual row
	itemsPerRow = 1;
	numberOfRows = 0;
	padding = 0;

	// Set the Apps Delegate
	setItemDelegate( new NBAppsDelegate() );

	// Applications Model
	cModel = new NBApplicationsModel();
	setModel( cModel );

	// Icon Size
	setIconSize( Settings->General.IconSize );

	// Selection
	setSelectionMode( QAbstractItemView::SingleSelection );
	setSelectionBehavior( QAbstractItemView::SelectRows );
	mSelectedIndexes = QModelIndexList();

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
	viewport()->setAcceptDrops( false );
	setDragEnabled( false );
	setAcceptDrops( false );
	setDragDropMode( QListView::NoDragDrop );
	setDropIndicatorShown( false );

	// Context Menu
	setContextMenuPolicy( Qt::CustomContextMenu );

	connect( cModel, SIGNAL( modelLoaded() ), this, SLOT( reload() ) );
	connect( cModel, SIGNAL( layoutChanged() ), this, SLOT( reload() ) );

	// Zoom In and Out actions
	QAction *zoomInAct = new QAction( "Zoom In", this );
	zoomInAct->setShortcut( QKeySequence::ZoomIn );
	connect( zoomInAct, SIGNAL( triggered() ), this, SLOT( zoomIn() ) );
	addAction( zoomInAct );

	QAction *zoomOutAct = new QAction( "Zoom Out", this );
	zoomOutAct->setShortcut( QKeySequence::ZoomOut );
	connect( zoomOutAct, SIGNAL( triggered() ), this, SLOT( zoomOut() ) );
	addAction( zoomOutAct );
};

void NBApplicationsView::setModel( QAbstractItemModel *model ) {

	QAbstractItemView::setModel( model );
	hashIsDirty = true;
};

void NBApplicationsView::updateViewMode() {

	currentViewMode = Settings->General.ViewMode;
	computeGridSize( Settings->General.IconSize );

	return;
};

int NBApplicationsView::categoryHeight() const {

	return myCategoryHeight;
};

void NBApplicationsView::setCategoryHeight( int newCategoryHeight ) {

	myCategoryHeight = newCategoryHeight;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QSize NBApplicationsView::iconSize() const {

	return myIconSize;
};

void NBApplicationsView::setIconSize( QSize newIconSize ) {

	myIconSize = newIconSize;
	computeGridSize( myIconSize );

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

QRect NBApplicationsView::visualRect( const QModelIndex &index ) const {

	QRect rect;
	if ( index.isValid() )
		rect = viewportRectForRow( index.row() );

	return rect;
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
		if ( QRect( i.value(), myGridSize ).adjusted( padding / 2, padding / 2, -padding, -padding ).contains( point ) )
			return cModel->index( i.key(), 0, rootIndex() );
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
			return categoryList.value( i.key() );
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

void NBApplicationsView::updateGeometries() {

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

void NBApplicationsView::reload() {

	currentViewMode = Settings->General.ViewMode;
	setIconSize( Settings->General.IconSize.width(), Settings->General.IconSize.width() );

	hashIsDirty = true;
	persistentVCol = 0;

	mSelectedIndexes.clear();

	categoryList = cModel->categories();
	calculateRectsIfNecessary();
};

QModelIndex NBApplicationsView::moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers ) {
	/*
		*
		* We still do not handle keyboard modifiers. This is yet to be implemented
		*
	*/

	return moveCursorCategorized( cursorAction );
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
	int firstRow = cModel->rowCount();
	int lastRow = -1;
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( i.value(), myGridSize ).intersects( rectangle ) ) {
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

QRegion NBApplicationsView::visualRegionForSelection( const QItemSelection &selection ) const {

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

QModelIndexList NBApplicationsView::selectedIndexes() {

	QSet<QModelIndex> idxSet;
	idxSet.unite( QSet<QModelIndex>::fromList( mSelectedIndexes ) );
	idxSet.unite( QSet<QModelIndex>::fromList( selectionModel()->selectedIndexes() ) );

	return idxSet.toList();
};

QModelIndexList NBApplicationsView::selection() {

	QSet<QModelIndex> idxSet;
	idxSet.unite( QSet<QModelIndex>::fromList( mSelectedIndexes ) );
	idxSet.unite( QSet<QModelIndex>::fromList( selectionModel()->selectedIndexes() ) );

	return idxSet.toList();
};

bool NBApplicationsView::isIndexVisible( QModelIndex idx ) const {

	// /* See if the index is in the hidden categories list */
	// if

	QRect rect = viewportRectForRow( idx.row() );
	if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
		return false;

	else
		return true;
};

void NBApplicationsView::paintEvent( QPaintEvent* event ) {

	QPainter painter( viewport() );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing );

	/* We need to draw the categories only if the model is categorization enabled */
	for ( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {

		QRect rect = categoryRect( catIdx );
		if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
			continue;

		paintCategory( &painter, rect, categoryList.at( catIdx ) );
	}

	for ( int row = 0; row < cModel->rowCount( rootIndex() ); row++ ) {
		QModelIndex idx = cModel->index( row, 0, rootIndex() );
		if ( not canShowIndex( idx ) )
			continue;

		QRect rect = viewportRectForRow( row );
		if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
			continue;

		QStyleOptionViewItem option = viewOptions();

		/* Rect */
		option.rect = rect;

		/* Selection */
		if ( mSelectedIndexes.contains( idx ) or selectionModel()->isSelected( idx ) )
			option.state |= QStyle::State_Selected;

		/* Focus */
		if ( currentIndex() == idx )
			option.state |= QStyle::State_HasFocus;

		/* Mouse over */
		QPoint mousePos = mapFromGlobal( cursor().pos() );
		if ( rect.contains( mousePos ) )
			option.state |= QStyle::State_MouseOver;

		/* decoration size */
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
			pltt.setColor( QPalette::ButtonText, pltt.color( QPalette::Text ).lighter( 135 ) );
		}

		/* Light text colors to be used here */
		else {
			if ( option.state & QStyle::State_Selected )
				pltt.setColor( QPalette::Text, pltt.color( QPalette::HighlightedText ) );

			else
				pltt.setColor( QPalette::Text, palette().color( QPalette::Text ) );

			/* Bright text will be used for drawing the 'current rect' */
			pltt.setColor( QPalette::BrightText, pltt.color( QPalette::Highlight ).lighter() );

			/* ButtonText will be used to paint the extra details */
			pltt.setColor( QPalette::ButtonText, pltt.color( QPalette::Text ).darker( 135 ) );
		}

		option.palette = pltt;

		NBAppsDelegate *dlgt = qobject_cast<NBAppsDelegate*>( itemDelegate() );
		if ( currentViewMode == QString( "IconsView" ) )
			dlgt->paintIcons( &painter, option, idx );

		else if ( currentViewMode == QString( "TilesView" ) )
			dlgt->paintTiles( &painter, option, idx );

		else
			dlgt->paintDetails( &painter, option, idx );
	}

	painter.end();
	event->accept();
};

void NBApplicationsView::paintSelection( QPainter *painter, const QModelIndexList indexes ) const {

	if ( not indexes.count() )
		return;

	painter->save();
	painter->setPen( QPen( palette().color( QPalette::Highlight ), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

	QColor bg = palette().color( QPalette::Highlight );
	bg.setAlpha( 120 );
	painter->setBrush( bg );

	QPainterPath path;
	Q_FOREACH( QModelIndex idx, indexes )
		path.addRoundedRect( QRectF( viewportRectForRow( idx.row() ) ), 4, 4 );

	painter->drawPath( path );
	painter->restore();
};

void NBApplicationsView::resizeEvent( QResizeEvent* ) {

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBApplicationsView::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( mpEvent->button() != Qt::LeftButton ) {
		QAbstractItemView::mousePressEvent( mpEvent );
		return;
	}

	/* Index at mouse position */
	QModelIndex idx = indexAt( mpEvent->pos() );

	if ( idx.isValid() ) {
		setCurrentIndex( idx );
		mpEvent->accept();
		return;
	}

	else if ( categoryAt( mpEvent->pos() ).count() ) {
		QRect rct = categoryRect( categoryList.indexOf( categoryAt( mpEvent->pos() ) ) );
		QPoint topLeft = rct.topLeft();

		/* Category arrow clicked */
		if ( QRect( topLeft.x() + 4, topLeft.y() + 4, 16, 16 ).contains( mpEvent->pos() ) )
			showHideCategory( categoryAt( mpEvent->pos() ) );

		/* Category bar pressed */
		else
			toggleCategorySelection( categoryAt( mpEvent->pos() ) );

		/* Repaint the viewport */
		viewport()->repaint();
	}

	/* Click on empty space */
	else {
		/* Clear the selections */
		mSelectedIndexes.clear();
		selectionModel()->clearSelection();

		/* Repaint the viewport */
		viewport()->repaint();
	}
};

void NBApplicationsView::mouseDoubleClickEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		QModelIndex idx = indexAt( mEvent->pos() );
		if ( idx.isValid() )
			emit runApplication( idx );

		else if ( not categoryAt( mEvent->pos() ).isEmpty() ) {
			showHideCategory( categoryAt( mEvent->pos() ) );
		}
	}

	mEvent->accept();
};

void NBApplicationsView::keyPressEvent( QKeyEvent *kEvent ) {

	if ( ( kEvent->key() == Qt::Key_Return ) and ( selectionModel()->isSelected( currentIndex() ) ) )
		emit runApplication( currentIndex() );

	QAbstractItemView::keyPressEvent( kEvent );
};

void NBApplicationsView::computeGridSize( QSize iconSize ) {

	if ( currentViewMode == "IconsView" ) {
		/*
			* width: 3 * iconSize
			* height: iconSize + iconSize * 2
		*/
		myGridSizeMin = QSize( qMin( 256, qMax( 144, iconSize.width() * 3 ) ), qMax( iconSize.height() + 21, iconSize.height() * 2 ) );
	}

	else if ( currentViewMode == QString( "TilesView" ) ) {
		/* iconSize + padding left + padding right + fixed width text ( max = 256px ) */
		padding = ( int ) round( iconSize.width() * 0.1 );
		myGridSizeMin = QSize( iconSize.width() + padding * 2 + qMin( 256, qMax( 144, iconSize.width() * 3 ) ), iconSize.height() + padding * 2 );
	}

	else {
		int availWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
		padding = ( int ) round( iconSize.width() * 0.1 );
		myGridSizeMin = QSize( availWidth, iconSize.height() + padding * 2 );
	}

	computeRowsAndColumns();

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QModelIndex NBApplicationsView::moveCursorCategorized( QAbstractItemView::CursorAction cursorAction ) {

	/* Clear mouse selection */
	mSelectedIndexes.clear();
	mSelectedCategories.clear();

	/* Current Index */
	QModelIndex idx = currentIndex();

	/* If there exists a current index */
	if ( idx.isValid() ) {

		int thisCategoryIdx = cModel->categoryIndex( idx );
		/* If we are in the first category go to the last one */
		int prevCategoryIdx = ( thisCategoryIdx == 0 ? cModel->categoryCount() - 1 : thisCategoryIdx - 1 );
		/* If we are in the last category go to the first one */
		int nextCategoryIdx = ( ( thisCategoryIdx == cModel->categoryCount() - 1 ) ? 0 : thisCategoryIdx + 1 );

		QString prevCategory = categoryList.value( prevCategoryIdx );
		QString thisCategory = cModel->category( idx );
		QString nextCategory = categoryList.value( nextCategoryIdx );

		QModelIndexList prevCategoryIndexes = cModel->indexListForCategory( prevCategory );
		QModelIndexList thisCategoryIndexes = cModel->indexListForCategory( thisCategory );
		QModelIndexList nextCategoryIndexes = cModel->indexListForCategory( nextCategory );

		switch( cursorAction ) {
			case QAbstractItemView::MoveNext:
				return nextIndex();

			case QAbstractItemView::MoveRight:
				return nextIndex();

			case QAbstractItemView::MovePrevious:
				return prevIndex();

			case QAbstractItemView::MoveLeft:
				return prevIndex();

			case QAbstractItemView::MoveDown: {
				if ( currentViewMode == "DetailsView" ) {
					if ( idx.row() == cModel->rowCount() - 1 ) {
						return cModel->index( 0, 0, idx.parent() );
					}

					else {
						return cModel->index( idx.row() + 1, 0, idx.parent() );
					}
				}

				return belowIndex();
			}

			case QAbstractItemView::MoveUp: {

				if ( currentViewMode == "DetailsView" ) {
					if ( idx.row() == 0 ) {
						return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
					}
					else {
						return cModel->index( idx.row() - 1, 0, idx.parent() );
					}
				}

				return aboveIndex();
			}

			case QAbstractItemView::MoveHome: {

				return firstIndex();
			}

			case QAbstractItemView::MoveEnd: {

				return lastIndex();
			}

			case QAbstractItemView::MovePageUp: {

				return indexPageAbove();
			}

			case QAbstractItemView::MovePageDown: {

				return indexPageBelow();
			}

			default: {
				return QModelIndex();
			}
		}
	}

	/* If there is no current index */
	else {
		switch( cursorAction ) {
			case QAbstractItemView::MoveHome:
				return firstIndex();

			case QAbstractItemView::MoveRight:
				return firstIndex();

			case QAbstractItemView::MoveNext:
				return firstIndex();

			case QAbstractItemView::MoveDown:
				return firstIndex();

			case QAbstractItemView::MoveEnd:
				return lastIndex();

			case QAbstractItemView::MoveLeft:
				return lastIndex();

			case QAbstractItemView::MovePrevious:
				return lastIndex();

			case QAbstractItemView::MoveUp:
				return firstIndex();

			default:
				return firstIndex();
		}
	}

	return idx;
};

void NBApplicationsView::toggleCategorySelection( QString category ) {

	/* Single Selection, clicking category does not change anything */
};

void NBApplicationsView::setCategorySelected( QString category, bool yes ) {

	if ( yes ) {
		if ( not mSelectedCategories.contains( category ) ) {
			mSelectedCategories << category;
			Q_FOREACH( QModelIndex idx, cModel->indexListForCategory( category ) ) {
				if ( not mSelectedIndexes.contains( idx ) )
					mSelectedIndexes << idx;
			}
		}
	}

	else {
		mSelectedCategories.removeAll( category );
		Q_FOREACH( QModelIndex idx, cModel->indexListForCategory( category ) )
			mSelectedIndexes.removeAll( idx );
	}
};

QModelIndex NBApplicationsView::nextIndex() {

	QModelIndex idx = currentIndex();
	/*
		*
		* 1. Check if the next index in still in this category.
		* 2. If not, list out the next visible category
		*
	*/

	// CurrentCategory
	QString cCategory = cModel->category( idx );
	int cCatIdx = categoryList.indexOf( cCategory );
	QModelIndexList cIdxList = cModel->indexListForCategory( cCategory );

	// Next Category
	QString nCategory;
	int nCatIdx = -1;

	/* We search for all the categories ahead, from the current category */
	for( int i = cCatIdx + 1; i < categoryList.count(); i++ ) {
		if ( not hiddenCategories.contains( categoryList.at( i ) ) ) {
			nCatIdx = i;
			nCategory = categoryList.at( i );
			break;
		}
	}
	/* If we find none, we search for all categories from 0, till the current one */
	if ( nCatIdx == -1 ) {
		for( int i = 0; i <= cCatIdx; i++ ) {
			if ( not hiddenCategories.contains( categoryList.at( i ) ) ) {
				nCatIdx = i;
				nCategory = categoryList.at( i );
				break;
			}
		}
	}

	QModelIndexList nIdxList = cModel->indexListForCategory( nCategory );

	/* If the current category is hidden, close your eyes and return the first index of the next category */
	if ( hiddenCategories.contains( cCategory ) ) {
		persistentVCol = 0;
		if ( nIdxList.count() )
			return nIdxList.first();

		else
			return QModelIndex();
	}

	// This is not the last index of the current category
	if ( cIdxList.last() != idx ) {
		persistentVCol = ( cIdxList.indexOf( idx ) + 1 ) % itemsPerRow;
		return cIdxList.at( cIdxList.indexOf( idx ) + 1 );
	}

	// This is the last index of the current category
	else {
		persistentVCol = 0;
		return nIdxList.first();
	}
};

QModelIndex NBApplicationsView::prevIndex() {

	QModelIndex idx = currentIndex();
	/*
		*
		* 1. Check if the next index in still in this category.
		* 2. If not, list out the previous visible category
		*
	*/

	// CurrentCategory
	QString cCategory = cModel->category( idx );
	int cCatIdx = categoryList.indexOf( cCategory );
	QModelIndexList cIdxList = cModel->indexListForCategory( cCategory );

	// Previous Category
	QString pCategory;
	int pCatIdx = -1;

	/* We search for all the categories before this one */
	for( int i = cCatIdx - 1; i >= 0; i-- ) {
		if ( not hiddenCategories.contains( categoryList.at( i ) ) ) {
			pCatIdx = i;
			pCategory = categoryList.at( i );
			break;
		}
	}
	/* If we find none, we search backwards from the lastt to the current one */
	if ( pCatIdx == -1 ) {
		for( int j = categoryList.count() - 1; j >= cCatIdx; j-- ) {
			if ( not hiddenCategories.contains( categoryList.at( j ) ) ) {
				pCatIdx = j;
				pCategory = categoryList.at( j );
				break;
			}
		}
	}

	QModelIndexList pIdxList = cModel->indexListForCategory( pCategory );

	// This is not the first index of this category
	if ( cIdxList.first() != idx ) {
		persistentVCol = ( cIdxList.indexOf( idx ) - 1 ) % itemsPerRow;
		return cIdxList.at( cIdxList.indexOf( idx ) - 1 );
	}

	// This is the first index of the current category
	else {
		persistentVCol = ( pIdxList.count() - 1 ) % itemsPerRow;
		return pIdxList.last();
	}
};

QModelIndex NBApplicationsView::belowIndex() {

	QModelIndex idx = currentIndex();
	/*
		*
		* 1. If we have an index below this in this category
		* 2. Else, we have a row below this in this category
		* 3. If we have an index below this in the next category
		* 4. Else, we return the last index in the next category
		*
	*/

	// CurrentCategory
	QString cCategory = cModel->category( idx );
	int cCatIdx = categoryList.indexOf( cCategory );
	QModelIndexList cIdxList = cModel->indexListForCategory( cCategory );

	// Next Category
	QString nCategory;
	int nCatIdx = -1;

	/* We search for all the categories ahead, from the current category */
	for( int i = cCatIdx + 1; i < categoryList.count(); i++ ) {
		if ( not hiddenCategories.contains( categoryList.at( i ) ) ) {
			nCatIdx = i;
			nCategory = categoryList.at( i );
			break;
		}
	}
	/* If we find none, we search for all categories from 0, till the current one */
	if ( nCatIdx == -1 ) {
		for( int i = 0; i <= cCatIdx; i++ ) {
			if ( not hiddenCategories.contains( categoryList.at( i ) ) ) {
				nCatIdx = i;
				nCategory = categoryList.at( i );
				break;
			}
		}
	}

	QModelIndexList nIdxList = cModel->indexListForCategory( nCategory );

	/* Compute the number of rows in the current category */
	int cRows = cIdxList.count() / itemsPerRow;
	if ( cIdxList.count() % itemsPerRow )
		cRows++;

	/* Check which row are we in */
	int cRow = ( 1 + cIdxList.indexOf( idx ) ) / itemsPerRow;
	if ( ( 1 + cIdxList.indexOf( idx ) ) % itemsPerRow )
		cRow++;

	/* Compute the number of rows in the previous category */
	int nRows = nIdxList.count() / itemsPerRow;
	if ( nIdxList.count() % itemsPerRow )
		nRows++;

	/* If we have a row below us and persistentVCol index exists */
	if ( ( cRow < cRows ) and ( ( cRow * itemsPerRow + persistentVCol ) < cIdxList.count() - 1 ) ) {

		return cIdxList.at( cIdxList.indexOf( idx ) + itemsPerRow );
	}

	// We have another row, just not an index below this
	else if ( ( cRow < cRows ) and ( ( cRow * itemsPerRow + persistentVCol ) >= cIdxList.count() - 1 ) ) {

		return cIdxList.last();
	}

	// There is an index below this one in the next category
	else if ( nIdxList.count() > persistentVCol ) {

		return nIdxList.at( persistentVCol );
	}

	// Last one in the next category
	else {

		return nIdxList.last();
	}
};

QModelIndex NBApplicationsView::aboveIndex() {

	QModelIndex idx = currentIndex();
	/*
		*
		* 1. Check if there are rows above the current one in this index
		* 2. If not, check if there is an index above this in the previous category
		* 3. Else, the last index of the previous category
		*
	*/

	// CurrentCategory
	QString cCategory = cModel->category( idx );
	int cCatIdx = categoryList.indexOf( cCategory );
	QModelIndexList cIdxList = cModel->indexListForCategory( cCategory );

	// Previous Category
	QString pCategory;
	int pCatIdx = -1;

	/* We search for all the categories before this one */
	for( int i = cCatIdx - 1; i >= 0; i-- ) {
		if ( not hiddenCategories.contains( categoryList.at( i ) ) ) {
			pCatIdx = i;
			pCategory = categoryList.at( i );
			break;
		}
	}
	/* If we find none, we search backwards from the lastt to the current one */
	if ( pCatIdx == -1 ) {
		for( int j = categoryList.count() - 1; j >= cCatIdx; j-- ) {
			if ( not hiddenCategories.contains( categoryList.at( j ) ) ) {
				pCatIdx = j;
				pCategory = categoryList.at( j );
				break;
			}
		}
	}

	QModelIndexList pIdxList = cModel->indexListForCategory( pCategory );

	/* Compute the number of rows in the current category */
	int cRows = cIdxList.count() / itemsPerRow;
	if ( cIdxList.count() % itemsPerRow )
		cRows++;

	/* Check which row are we in */
	int cRow = ( 1 + cIdxList.indexOf( idx ) ) / itemsPerRow;
	if ( ( 1 + cIdxList.indexOf( idx ) ) % itemsPerRow )
		cRow++;

	/* Compute the number of rows in the previous category */
	int pRows = pIdxList.count() / itemsPerRow;
	if ( pIdxList.count() % itemsPerRow )
		pRows++;

	/* Check the number of items in the last row */
	int nLast = pIdxList.count() - ( pRows - 1 ) * itemsPerRow;

	/* If we have a row above us in the current category, return the persistent index above */
	if ( cRow > 1 )
		return cIdxList.at( ( cIdxList.indexOf( idx ) / itemsPerRow - 1 ) * itemsPerRow + persistentVCol );

	/* We have to return from the previous category */
	else if ( persistentVCol < nLast )
		return pIdxList.at( ( pRows - 1 ) * itemsPerRow + persistentVCol );

	/* The last index of the previous category */
	else
		return pIdxList.last();
};

QModelIndex NBApplicationsView::firstIndex() {

	persistentVCol = 0;

	int firstVisibleCatIdx = -1;
	Q_FOREACH( QString category, categoryList ) {
		if ( not hiddenCategories.contains( category ) ) {
			firstVisibleCatIdx = categoryList.indexOf( category );
			break;
		}
	}

	if ( firstVisibleCatIdx >= 0 )
		return cModel->indexListForCategory( categoryList.at( firstVisibleCatIdx ) ).first();

	else
		return QModelIndex();
};

QModelIndex NBApplicationsView::lastIndex() {

	int lastVisibleCatIdx = -1;
	for( int i = categoryList.count() - 1; i >= 0; i-- ) {
		if ( not hiddenCategories.contains( categoryList.at( i ) ) ) {
			lastVisibleCatIdx = i;
			break;
		}
	}

	persistentVCol = ( cModel->indexListCountForCategory( categoryList.at( lastVisibleCatIdx ) ) - 1 ) % itemsPerRow;

	if ( lastVisibleCatIdx >= 0 )
		return cModel->indexListForCategory( categoryList.at( lastVisibleCatIdx ) ).last();

	else
		return QModelIndex();
};

QModelIndex NBApplicationsView::indexPageBelow() {

	QModelIndex idx = currentIndex();
	QScrollBar *bar = verticalScrollBar();
	bar->setValue( bar->value() + bar->pageStep() );

	QModelIndex nIdx;
	for ( int row = idx.row(); row < cModel->rowCount( rootIndex() ); row++ ) {
		QModelIndex idx = cModel->index( row, 0, rootIndex() );
		if ( not canShowIndex( idx ) )
			continue;

		QRect rect = viewportRectForRow( row );
		if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
			continue;

		nIdx =  cModel->index( row, 0, rootIndex() );
	}

	QString cCategory = cModel->category( idx );
	QModelIndexList cIdxList = cModel->indexListForCategory( cCategory );

	/* Compute the number of rows in the current category */
	int cRows = cIdxList.count() / itemsPerRow;
	if ( cIdxList.count() % itemsPerRow )
		cRows++;

	/* Check which row are we in */
	int cRow = ( 1 + cIdxList.indexOf( idx ) ) / itemsPerRow;
	if ( ( 1 + cIdxList.indexOf( idx ) ) % itemsPerRow )
		cRow++;

	if ( ( cRow - 1 ) * itemsPerRow + persistentVCol < cIdxList.count() )
		return cIdxList.at( ( cRow - 1 ) * itemsPerRow + persistentVCol );

	else
		return cIdxList.last();
};

QModelIndex NBApplicationsView::indexPageAbove() {

	return QModelIndex();
};

QModelIndex NBApplicationsView::moveCursorNonCategorized( QAbstractItemView::CursorAction cursorAction ) {

	QModelIndex idx = currentIndex();
	if ( idx.isValid() ) {

		switch( cursorAction ) {
			case QAbstractItemView::MoveRight: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < cModel->rowCount() - 1 ) {
					persistentVCol = ( idx.row() + 1 ) % itemsPerRow;
					return cModel->index( idx.row() + 1, 0, rootIndex() );
				}

				/* Current index is the last visible index */
				else {
					persistentVCol = 0;
					return cModel->index( 0, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveLeft: {
				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < cModel->rowCount() ) {
					persistentVCol = ( idx.row() - 1 ) % itemsPerRow;
					return cModel->index( idx.row() - 1, 0, rootIndex() );
				}

				/* The current index is the first one */
				else {
					persistentVCol = ( cModel->rowCount() - 1 ) % itemsPerRow;
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveDown: {
				if ( currentViewMode == "DetailsView" ) {
					if ( idx.row() == cModel->rowCount() - 1 ) {
						return cModel->index( 0, 0, idx.parent() );
					}
					else {
						return cModel->index( idx.row() + 1, 0, idx.parent() );
					}
				}

				int nextRow = idx.row() + itemsPerRow;
				if ( nextRow >= cModel->rowCount() ) {
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
				}

				else {
					return cModel->index( nextRow, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveUp: {
				if ( currentViewMode == "DetailsView" ) {
					if ( idx.row() == 0 ) {
						return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
					}
					else {
						return cModel->index( idx.row() - 1, 0, idx.parent() );
					}
				}

				int prevRow = idx.row() - itemsPerRow;
				if ( prevRow < 0 ) {
					return cModel->index( 0, 0, rootIndex() );
				}

				else {
					return cModel->index( prevRow, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveHome: {

				persistentVCol = 0;
				verticalScrollBar()->setValue( 0 );
				return cModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				persistentVCol = ( cModel->rowCount() - 1 ) % itemsPerRow;
				return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
			}

			case QAbstractItemView::MoveNext: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < cModel->rowCount() - 1 ) {
					return cModel->index( idx.row() + 1, 0, rootIndex() );
				}

				/* Current index is the last visible index */
				else {
					return cModel->index( 0, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MovePrevious: {

				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < cModel->rowCount() ) {
					return cModel->index( idx.row() - 1, 0, rootIndex() );
				}

				/* The current index is the first one */
				else {
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MovePageUp: {
				int viewportHeight = viewport()->height();
				int rowsInVisualArea = viewportHeight / myGridSize.height();
				int itemsInVisualArea = rowsInVisualArea * itemsPerRow;

				int prevIdx = idx.row() - itemsInVisualArea;
				if ( not cModel->index( prevIdx, 0, rootIndex() ).isValid() ) {
					return cModel->index( 0, 0, rootIndex() );
				}

				return cModel->index( prevIdx, 0, rootIndex() );
			}

			case QAbstractItemView::MovePageDown: {
				int viewportHeight = viewport()->height();
				int rowsInVisualArea = viewportHeight / myGridSize.height();
				int itemsInVisualArea = rowsInVisualArea * itemsPerRow;

				int nextIdx = idx.row() + itemsInVisualArea;
				if ( not cModel->index( nextIdx, 0, rootIndex() ).isValid() ) {
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
				}

				return cModel->index( nextIdx, 0, rootIndex() );
			}

			default: {
				return cModel->index( 0, 0, rootIndex() );
			}
		}
	}

	else {
		switch( cursorAction ) {
			case QAbstractItemView::MoveHome: {

				persistentVCol = 0;
				return cModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				persistentVCol = ( cModel->rowCount() - 1 ) % itemsPerRow;
				return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
			}

			default: {

				return cModel->index( 0, 0, rootIndex() );
			}
		}
	}

	return idx;
};

void NBApplicationsView::calculateRectsIfNecessary() const {

	if ( not hashIsDirty )
		return;

	computeRowsAndColumns();

	rectForRow.clear();
	rectForCategory.clear();

	calculateCategorizedRects();

	if ( idealHeight <= viewport()->height() ) {

		verticalScrollBar()->setRange( 0, 0 );
		verticalScrollBar()->hide();
	}

	else {
		verticalScrollBar()->show();

		verticalScrollBar()->setSingleStep( myGridSize.height() );
		verticalScrollBar()->setPageStep( viewport()->height() );
		verticalScrollBar()->setRange( 0, qMax( 0, idealHeight - viewport()->height() ) );
	}
};

void NBApplicationsView::calculateCategorizedRects() const {

	int x = 0, y = 0, prevRows = 0, totalRows = 0;
	categoryList = cModel->categories();

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = cModel->indexListForCategory( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		int minX = myContentsMargins.left();
		int minY = myContentsMargins.top() + catIdx * myCategoryHeight + catIdx * myCategorySpacing + totalRows * myGridSize.height();

		int categoryWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
		rectForCategory[ catIdx ] = QRect( minX, minY, categoryWidth, 24 );

		// We consider the space reserved for the category but not the indexes listed under it
		if ( hiddenCategories.contains( cModel->category( mList.value( 0 ) ) ) )
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

			x = minX + col * myGridSize.width();
			y = minY + row * myGridSize.height();

			rectForRow[ mList[ lrow ].row() ] = QPoint( x, y );
		}
	}

	idealHeight = myContentsMargins.top() + myContentsMargins.bottom() + categoryList.count() * ( myCategoryHeight + myCategorySpacing );
	idealHeight += totalRows * myGridSize.height();

	hashIsDirty = false;
	viewport()->update();
};

void NBApplicationsView::computeRowsAndColumns() const {

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

QRect NBApplicationsView::viewportRectForRow( int row ) const {

	calculateRectsIfNecessary();
	QPoint pt = rectForRow.value( row );
	if ( !QRect( pt, myGridSize ).isValid() )
		return QRect();

    return QRect( pt.x(), pt.y() - verticalScrollBar()->value(), myGridSize.width(), myGridSize.height() );
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

void NBApplicationsView::runApplication( const QModelIndex &index ) {

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
		return QIcon::fromTheme( "preferences-system" ).pixmap( 16, 16 );

	else if ( categoryText == "System" )
		return QIcon::fromTheme( "applications-system" ).pixmap( 16, 16 );

	else
		return QIcon::fromTheme( "preferences-other" ).pixmap( 16, 16 );
};

void NBApplicationsView::zoomIn() {

	if ( currentViewMode == QString( "DetailsView" ) ) {
		if ( myIconSize.width() >= 64 )
			setIconSize( QSize( 64, 64 ) );

		else
			setIconSize( myIconSize + QSize( 4, 4 ) );
	}

	else {
		if ( myIconSize.width() >= 128 )
			setIconSize( QSize( 128, 128 ) );

		else
			setIconSize( myIconSize + QSize( 4, 4 ) );
	}
};

void NBApplicationsView::zoomOut() {

	if ( myIconSize.width() <= 16 )
		setIconSize( QSize( 16, 16 ) );

	else
		setIconSize( myIconSize - QSize( 4, 4 ) );
};

void NBApplicationsView::showHideCategory( QString category ) {

	if ( hiddenCategories.contains( category ) )
		hiddenCategories.removeAll( category );

	else
		hiddenCategories.append( category );

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

bool NBApplicationsView::canShowIndex( QModelIndex idx ) {

	return not hiddenCategories.contains( cModel->category( idx ) );
};
