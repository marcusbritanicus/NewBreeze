/*
	*
	* NBIconView.cpp - NewBreeze IconView Class
	*
*/

#include <NBIconView.hpp>

NBIconView::NBIconView( NBFileSystemModel *fsModel ) : QAbstractItemView() {

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

	// Icon Size
	myIconSize = QSize( 48, 48 );

	// Persistent vertical column
	persistentVCol = 0;

	// Items per visual row
	itemsPerRow = 1;
	numberOfRows = 0;
	padding = 0;

	// Set the Apps Delegate
	setItemDelegate( new NBIconDelegate() );

	// Applications Model
	cModel = fsModel;
	setModel( cModel );

	// Icon Size
	setIconSize( Settings->Session.IconSize );

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

void NBIconView::setModel( QAbstractItemModel *model ) {

	QAbstractItemView::setModel( model );
	hashIsDirty = true;
};

void NBIconView::updateViewMode() {

	computeGridSize( Settings->Session.IconSize );

	if ( Settings->General.FolderView == QString ( "DetailsView" ) ){}
		// showHeader();

	else{}
		// hideHeader();

	hashIsDirty = true;
	calculateRectsIfNecessary();

	return;
};

int NBIconView::categoryHeight() const {

	return myCategoryHeight;
};

void NBIconView::setCategoryHeight( int newCategoryHeight ) {

	myCategoryHeight = newCategoryHeight;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QSize NBIconView::iconSize() const {

	return myIconSize;
};

void NBIconView::setIconSize( QSize newIconSize ) {

	myIconSize = newIconSize;
	computeGridSize( myIconSize );

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
		if ( QRect( i.value(), myGridSize ).adjusted( padding / 2, padding / 2, -padding / 2, -padding / 2 ).contains( point ) )
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
	persistentVCol = 0;
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

	QModelIndexList selectedIndexes;
	for ( int row = 0; row < cModel->rowCount( rootIndex() ); row++ ) {
		QModelIndex index = cModel->index( row, 0, rootIndex() );

		QRect rect = viewportRectForRow( row );
		if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
			continue;

		QStyleOptionViewItem option = viewOptions();
		option.rect = rect;

		if ( selectionModel()->isSelected( index ) )
			selectedIndexes << index;

		if ( currentIndex() == index )
			option.state |= QStyle::State_HasFocus;

		QPoint mousePos = mapFromGlobal( cursor().pos() );
		if ( rect.contains( mousePos ) )
			option.state |= QStyle::State_MouseOver;

		option.decorationSize = myIconSize;

		itemDelegate()->paint( &painter, option, index );
	}

	paintSelection( &painter, selectedIndexes );

	painter.end();
	event->accept();
};

void NBIconView::paintSelection( QPainter *painter, const QModelIndexList indexes ) const {

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

void NBIconView::resizeEvent( QResizeEvent* ) {

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::mousePressEvent( QMouseEvent *mpEvent ) {

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

void NBIconView::mouseMoveEvent( QMouseEvent *mmEvent ) {

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

	if ( selectionModel()->hasSelection() and indexAt( dragStartPosition ).isValid() ) {

		QDrag *drag = new QDrag( this );

		QList<QUrl> urlList;
		Q_FOREACH( QModelIndex idx, selectionModel()->selectedIndexes() )
			if ( not idx.column() )
				urlList << QUrl::fromLocalFile( cModel->nodePath( idx ) );

		QMimeData *mimedata = new QMimeData();
		mimedata->setUrls( urlList );

		drag->setMimeData( mimedata );
		drag->exec( Qt::CopyAction | Qt::MoveAction | Qt::LinkAction );
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

void NBIconView::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	/* If its just a click and not a drag */
	if ( dragStartPosition == mrEvent->pos() ) {
		QModelIndex idx = indexAt( mrEvent->pos() );
		if ( idx.isValid() ) {
			/* Click landed on an index. Select / deselect the item(s) */
			QModelIndexList selected = selectionModel()->selectedRows( 0 );

			/* This is the starting index in the selection */
			QModelIndex start = ( selected.count() ? selected.at( 0 ) : currentIndex() );
			if ( not start.isValid() )
				start = cModel->index( 0, 0, rootIndex() );

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

	repaint();

	mrEvent->accept();
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

	/* Incoming drop from else where */
	if ( source != cModel->currentDir() ) {
		if ( isWritable( cModel->currentDir() ) ) {
			dmEvent->setDropAction( Qt::CopyAction );
			dmEvent->accept();
		}

		else
			dmEvent->ignore();
	}

	/* drag and drop in the current folder */
	else if ( indexAt( dmEvent->pos() ).isValid() ) {
		QModelIndex idx = indexAt( dmEvent->pos() );
		QString mtpt = cModel->nodePath( cModel->data( idx ).toString() );

		Q_FOREACH( QUrl url, mData->urls() ) {
			if ( url.toLocalFile() == mtpt ) {
				dmEvent->ignore();
				return;
			}
		}

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

		if ( dpEvent->keyboardModifiers() == Qt::NoModifier ) {
			args << mtpt;

			NBMenu *menu = new NBMenu( this );

			QAction *mvAct = new QAction( QIcon::fromTheme( "go-jump" ), QString( "&Move Here" ), this );
			mvAct->setData( QVariant( args ) );
			connect( mvAct, SIGNAL( triggered() ), this, SLOT( emitCML() ) );

			QAction *cpAct = new QAction( QIcon::fromTheme( "edit-copy" ), QString( "&Copy Here" ), this );
			cpAct->setData( QVariant( args ) );
			connect( cpAct, SIGNAL( triggered() ), this, SLOT( emitCML() ) );

			QAction *lnAct = new QAction( QIcon::fromTheme( "edit-link" ), QString( "&Link Here" ), this );
			lnAct->setData( QVariant( args ) );
			connect( lnAct, SIGNAL( triggered() ), this, SLOT( emitCML() ) );

			QAction *cancelAct = new QAction( QIcon::fromTheme( "dialog-close" ), QString( "Cancel" ), this );
			connect( cancelAct, SIGNAL( triggered() ), menu, SLOT( close() ) );

			menu->addAction( mvAct );
			menu->addAction( cpAct );
			menu->addAction( lnAct );
			menu->addSeparator();
			menu->addAction( cancelAct );

			menu->exec( mapToGlobal( dpEvent->pos() ) );
			dpEvent->accept();
		}

		else if ( dpEvent->keyboardModifiers() == Qt::ShiftModifier ) {

			qDebug() << "DnD Move";
			emit move( args, mtpt );
		}

		else if ( dpEvent->keyboardModifiers() == Qt::ControlModifier ) {

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

void NBIconView::computeGridSize( QSize iconSize ) {

	if ( Settings->General.FolderView == "IconsView" ) {
		/*
			* width: 3 * iconSize
			* height: iconSize + iconSize * 2
		*/
		myGridSizeMin = QSize( qMin( 256, qMax( 144, iconSize.width() * 3 ) ), qMax( iconSize.height() + 21, iconSize.height() * 2 ) );
	}

	else if ( Settings->General.FolderView == QString( "TilesView" ) ) {
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

QModelIndex NBIconView::moveCursorCategorized( QAbstractItemView::CursorAction cursorAction ) {

	QModelIndex idx = currentIndex();
	QStringList categoryList = cModel->categories();

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

		// Calculate the visual row of this index
		int rowInCategory = thisCategoryIndexes.indexOf( idx );
		int vrow = rowInCategory / itemsPerRow;

		switch( cursorAction ) {
			case QAbstractItemView::MoveNext:
			case QAbstractItemView::MoveRight: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < cModel->rowCount() - 1 ) {
					/* If this is the visual index of the catgory, then next persistentVCol will be zero */
					if ( idx == thisCategoryIndexes.last() )
						persistentVCol = 0;

					else
						persistentVCol = ( rowInCategory + 1 ) % itemsPerRow;

					return cModel->index( idx.row() + 1, 0, rootIndex() );
				}

				/* Current index is the last visible index */
				else {
					persistentVCol = 0;
					return cModel->index( 0, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MovePrevious:
			case QAbstractItemView::MoveLeft: {
				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < cModel->rowCount() ) {
					if ( idx == thisCategoryIndexes.first() )
						persistentVCol = ( prevCategoryIndexes.count() - 1 ) % itemsPerRow;

					else
						persistentVCol = ( rowInCategory - 1 ) % itemsPerRow;
					return cModel->index( idx.row() - 1, 0, rootIndex() );
				}

				/* The current index is the first one */
				else {
					// #warning "FIXME: This implementation is buggy"
					persistentVCol = ( prevCategoryIndexes.count() - 1 ) % itemsPerRow;
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveDown: {
				if ( Settings->General.FolderView == "DetailsView" ) {
					if ( idx.row() == cModel->rowCount() - 1 ) {
						return cModel->index( 0, 0, idx.parent() );
					}
					else {
						return cModel->index( idx.row() + 1, 0, idx.parent() );
					}
				}

				int newVRow = vrow + 1;

				if ( ( newVRow * itemsPerRow + persistentVCol ) < thisCategoryIndexes.count() ) {
					// We have an idx below this idx from the same category
					return thisCategoryIndexes.value( newVRow * itemsPerRow + persistentVCol );
				}

				else if ( newVRow * itemsPerRow < thisCategoryIndexes.count() ) {
					// We have indexes with row greater than this one, not below this one
					return thisCategoryIndexes.last();
				}

				else {
					// We need to return the idx from the next category same or lower column
					if ( nextCategoryIndexes.count() ) {
						if ( nextCategoryIndexes.count() <= persistentVCol )
							return nextCategoryIndexes.last();

						else
							return nextCategoryIndexes.value( persistentVCol );
					}

					else {

						return nextCategoryIndexes.last();
					};
				}

			}

			case QAbstractItemView::MoveUp: {

				if ( Settings->General.FolderView == "DetailsView" ) {
					if ( idx.row() == 0 ) {
						return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
					}
					else {
						return cModel->index( idx.row() - 1, 0, idx.parent() );
					}
				}

				int newVRow = vrow - 1;

				if ( newVRow >= 0 ) {

					// This means that there is a visual row before this one
					return thisCategoryIndexes.value( newVRow * itemsPerRow + persistentVCol );
				}

				else {
					if ( prevCategoryIndexes.count() ) {
						int nrow = ( int )ceil( 1.0 * prevCategoryIndexes.count() / itemsPerRow );
						if ( prevCategoryIndexes.count() > ( nrow - 1 ) * itemsPerRow + persistentVCol )
							return prevCategoryIndexes.value( ( nrow - 1 ) * itemsPerRow + persistentVCol );

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
				persistentVCol = 0;
				return cModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				persistentVCol = ( cModel->indexListCountForCategory( categoryList.last() ) - 1 ) % itemsPerRow;
				return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
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
				return cModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				persistentVCol = ( cModel->indexListCountForCategory( categoryList.last() ) - 1 ) % itemsPerRow;
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
				if ( Settings->General.FolderView == "DetailsView" ) {
					if ( idx.row() == cModel->rowCount() - 1 ) {
						return cModel->index( 0, 0, idx.parent() );
					}
					else {
						return cModel->index( idx.row() + 1, 0, idx.parent() );
					}
				}

				int nextRow = idx.row() + itemsPerRow;
				if ( nextRow >= cModel->rowCount() )
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );

				else
					return cModel->index( nextRow, 0, rootIndex() );
			}

			case QAbstractItemView::MoveUp: {
				if ( Settings->General.FolderView == "DetailsView" ) {
					if ( idx.row() == 0 ) {
						return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
					}
					else {
						return cModel->index( idx.row() - 1, 0, idx.parent() );
					}
				}

				int prevRow = idx.row() - itemsPerRow;
				if ( prevRow < 0 )
					return cModel->index( 0, 0, rootIndex() );

				else
					return cModel->index( prevRow, 0, rootIndex() );
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

			case QAbstractItemView::MovePageUp: {
				int viewportHeight = viewport()->height();
				int rowsInVisualArea = viewportHeight / myGridSize.height();
				int itemsInVisualArea = rowsInVisualArea * itemsPerRow;

				int prevIdx = idx.row() - itemsInVisualArea;
				if ( not cModel->index( prevIdx, 0, rootIndex() ).isValid() )
					return cModel->index( 0, 0, rootIndex() );

				return cModel->index( prevIdx, 0, rootIndex() );
			}

			case QAbstractItemView::MovePageDown: {
				int viewportHeight = viewport()->height();
				int rowsInVisualArea = viewportHeight / myGridSize.height();
				int itemsInVisualArea = rowsInVisualArea * itemsPerRow;

				int nextIdx = idx.row() + itemsInVisualArea;
				if ( not cModel->index( nextIdx, 0, rootIndex() ).isValid() )
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );

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

void NBIconView::calculateRectsIfNecessary() const {

	if ( not hashIsDirty )
		return;

	computeRowsAndColumns();

	rectForRow.clear();
	rectForCategory.clear();

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

	if ( Settings->General.FolderView == QString( "IconsView" ) )
		calculateCategorizedIconsRects();

	else if ( Settings->General.FolderView == QString( "DetailsView" ) )
		calculateCategorizedDetailsRects();

	else
		calculateCategorizedTilesRects();
};

void NBIconView::calculateCategorizedIconsRects() const {

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

void NBIconView::calculateCategorizedTilesRects() const {

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

void NBIconView::calculateCategorizedDetailsRects() const {

	if ( not hashIsDirty )
		return;

	int y = 0, prevRows = 0, totalRows = 0;
	QStringList categoryList = cModel->categories();

	int catX = myContentsMargins.left() ;
	int catW = myGridSize.width() + myInlayMargins.left() + myInlayMargins.right();
	int x = myContentsMargins.left() + myInlayMargins.left();

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = cModel->indexListForCategory( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		int minY = myContentsMargins.top() + catIdx * myCategoryHeight + catIdx * myCategorySpacing + totalRows * myGridSize.height();

		rectForCategory[ catIdx ] = QRect( catX, minY, catW, myCategoryHeight );

		// Mimimum X and Y for indexes
		minY += myCategoryHeight;

		prevRows = cModel->indexListCountForCategory( categoryList.at( catIdx ) );
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
	idealHeight += cModel->rowCount() * myGridSize.height();

	hashIsDirty = false;
	viewport()->update();
};

void NBIconView::calculateNonCategorizedRects() const {

	if ( Settings->General.FolderView == QString( "IconsView" ) )
		calculateNonCategorizedIconsRects();

	else if ( Settings->General.FolderView == QString( "DetailsView" ) )
		calculateNonCategorizedDetailsRects();

	else
		calculateNonCategorizedTilesRects();
};

void NBIconView::calculateNonCategorizedIconsRects() const {

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
	}

	idealHeight = myContentsMargins.top() + numberOfRows * myGridSize.height() + myContentsMargins.bottom();

	hashIsDirty = false;
	viewport()->update();
};

void NBIconView::calculateNonCategorizedTilesRects() const {

	int x = 0, y = 0, totalRows = 0;

	for( int lrow = 0; lrow < cModel->rowCount(); lrow++ ) {

		totalRows = ( lrow + 1 ) / itemsPerRow;
		if ( ( lrow + 1 ) % itemsPerRow )
			totalRows++;

		// Minimum X and Y for Category Rectangle
		int minX = myContentsMargins.left();
		int minY = myContentsMargins.top() + ( totalRows - 1 ) * myGridSize.height();

		int col = lrow % itemsPerRow;

		x = minX + col * myGridSize.width();
		y = minY;

		rectForRow[ lrow ] = QPoint( x, y );
	}

	idealHeight = myContentsMargins.top() + numberOfRows * myGridSize.height() + myContentsMargins.bottom();

	hashIsDirty = false;
	viewport()->update();
};

void NBIconView::calculateNonCategorizedDetailsRects() const {

	int x = 0, y = 0, totalRows = cModel->rowCount();

	for( int lrow = 0; lrow < cModel->rowCount(); lrow++ ) {

		x = myContentsMargins.left();
		y = myContentsMargins.top() + lrow * myGridSize.height();

		rectForRow[ lrow ] = QPoint( x, y );
	}

	idealHeight = myContentsMargins.top() + totalRows * myGridSize.height() + myContentsMargins.bottom();

	hashIsDirty = false;
	viewport()->update();
};

void NBIconView::computeRowsAndColumns() const {

	int vWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
	vWidth = vWidth - myInlayMargins.left() - myInlayMargins.right();

	if ( Settings->General.FolderView == QString( "DetailsView" ) ) {
		itemsPerRow = 1;
		numberOfRows = cModel->rowCount();
	}

	else {
		itemsPerRow = qMax( 1, vWidth / myGridSizeMin.width() );
		numberOfRows = ( int )( cModel->rowCount() / itemsPerRow );

		if ( ( cModel->rowCount() % itemsPerRow ) > 0 )
			numberOfRows++;
	}

	int newGridWidth = ( int )( vWidth / itemsPerRow );
	myGridSize = QSize( newGridWidth, myGridSizeMin.height() );
};

QRect NBIconView::viewportRectForRow( int row ) const {

	calculateRectsIfNecessary();
	QPoint pt = rectForRow.value( row );
	if ( !QRect( pt, myGridSize ).isValid() )
		return QRect();

    return QRect( pt.x(), pt.y() - verticalScrollBar()->value(), myGridSize.width(), myGridSize.height() );
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

	Q_UNUSED( categoryText );

	return QIcon::fromTheme( "arrow-right" ).pixmap( 16, 16 );
};

void NBIconView::zoomIn() {

	if ( Settings->General.FolderView == QString( "DetailsView" ) ) {
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

	Settings->Session.IconSize = myIconSize;
};

void NBIconView::zoomOut() {

	if ( myIconSize.width() <= 16 )
		setIconSize( QSize( 16, 16 ) );

	else
		setIconSize( myIconSize - QSize( 4, 4 ) );

	Settings->Session.IconSize = myIconSize;
};

void NBIconView::emitCML() {

	QAction *act = qobject_cast<QAction*>( sender() );

	QStringList args = act->data().toStringList();
	QString mtpt = args.takeLast();

	if ( act->text().contains( "Copy" ) ) {

		emit copy( args, mtpt );
	}

	else if ( act->text().contains( "Move" ) ) {

		emit move( args, mtpt );
	}

	else {

		emit link( args, mtpt );
	}
};
