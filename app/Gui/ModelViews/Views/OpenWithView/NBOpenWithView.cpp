/*
	*
	* NBAppsView.cpp - NewBreeze FolderView Class
	*
*/

#include "NBOpenWithView.hpp"
#include "NBCategoryMenu.hpp"

NBOpenWithView::NBOpenWithView() : QAbstractItemView() {

	/* Icon rects */
	idealHeight = 0;
	hashIsDirty = true;

	/* Category Height */
	myCategoryHeight = 24;

	/* Category Spacing */
	myCategorySpacing = 10;

	/* Contents Margins */
	myContentsMargins = QMargins( 10, 10, 10, 10 );

	/* Inlay Margins */
	myInlayMargins = QMargins( 10, 0, 10, 0 );

	/* Sizes */
	viewWidth = 320;
	myIconSize = QSize( 24, 24 );

	/* Persistent vertical column */
	persistentVCol = 0;

	/* QPair( catIdx, elmInCatIdx ) */
	curIdx = qMakePair( 0, 0 );

	/* Items per visual row */
	numberOfRows = 0;
	padding = 0;

	// Applications Model
	appModel = new NBItemViewModel();
	appModel->setCategorizationEnabled( true );
	setModel( appModel );

	/* Set the Apps Delegate */
	setItemDelegate( new NBOpenWithDelegate() );

	/* Default Selection Rules */
	setSelectionMode( QAbstractItemView::SingleSelection );
	setSelectionBehavior( QAbstractItemView::SelectRows );
	mSelectedIndexes = QModelIndexList();

	/* Internal Object Name */
	setObjectName( "mainList" );

	/* Minimum Size */
	setMinimumWidth( 320 );

	/* Focus Policy */
	setFocusPolicy( Qt::StrongFocus );

	/* Mouse tracking */
	setMouseTracking( true );

	/* No Horizontal ScrollBar */
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	/* DragAndDrop */
	viewport()->setAcceptDrops( false );
	setDragEnabled( false );
	setAcceptDrops( false );

	/* Context Menu */
	setContextMenuPolicy( Qt::NoContextMenu );

	connect( appModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( reload() ) );
	connect( appModel, SIGNAL( layoutChanged() ), this, SLOT( reload() ) );

	appModel->setRootPath( "NB://Applications" );
};

void NBOpenWithView::setModel( QAbstractItemModel *model ) {

	QAbstractItemView::setModel( model );

	hashIsDirty = true;
};

QRect NBOpenWithView::visualRect( const QModelIndex &index ) const {

	QRect rect;
	if ( index.isValid() )
		rect = viewportRectForRow( index.row() );

	return rect;
};

QRect NBOpenWithView::categoryRect( int categoryIndex ) const {

	calculateRectsIfNecessary();
	QRect rect = rectForCategory.value( categoryIndex );
	if ( not rect.isValid() )
		return QRect();

    return QRect( rect.x(), rect.y() - verticalScrollBar()->value(), rect.width(), rect.height() );
};

void NBOpenWithView::scrollTo( const QModelIndex &index, QAbstractItemView::ScrollHint ) {

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

QModelIndex NBOpenWithView::indexAt( const QPoint &point_ ) const {

	QPoint point( point_ );
	point.ry() += verticalScrollBar()->value();

	calculateRectsIfNecessary();
	QHashIterator<int, QRect> rIt( rectForRow );
	while ( rIt.hasNext() ) {

		rIt.next();
		if ( rIt.value().adjusted( padding / 2, padding / 2, -padding, -padding ).contains( point ) )
			return appModel->index( rIt.key(), 0, rootIndex() );
	}

	return QModelIndex();
};

QString NBOpenWithView::categoryAt( const QPoint &point_ ) const {

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

void NBOpenWithView::dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight ) {

	hashIsDirty = true;
	QAbstractItemView::dataChanged( topLeft, bottomRight );
};

void NBOpenWithView::rowsInserted( const QModelIndex &parent, int start, int end ) {

	hashIsDirty = true;
	QAbstractItemView::rowsInserted( parent, start, end );
};

void NBOpenWithView::rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end ) {

	hashIsDirty = true;
	QAbstractItemView::rowsAboutToBeRemoved( parent, start, end );
};

void NBOpenWithView::updateGeometries() {

	if ( idealHeight <= viewport()->height() ) {
		verticalScrollBar()->setValue( 0 );
		verticalScrollBar()->hide();
	}

	else {
		verticalScrollBar()->show();
		verticalScrollBar()->setSingleStep( 32 );
		verticalScrollBar()->setPageStep( viewport()->height() );
		verticalScrollBar()->setRange( 0, qMax( 0, idealHeight - viewport()->height() ) );
	}
};

void NBOpenWithView::reload() {

	hashIsDirty = true;
	persistentVCol = 0;

	hiddenCategories.clear();

	categoryList = appModel->categories();
	calculateRectsIfNecessary();
};

QModelIndex NBOpenWithView::moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers ) {

	/* Clear mouse selection */
	mSelectedIndexes.clear();
	mSelectedCategories.clear();

	/* Current Index */
	QModelIndex idx = currentIndex();

	/* If there exists a current index */
	if ( idx.isValid() ) {

		switch( cursorAction ) {
			case QAbstractItemView::MoveNext:
			case QAbstractItemView::MoveRight:
				emit selectionChanged();
				return nextIndex();

			case QAbstractItemView::MovePrevious:
			case QAbstractItemView::MoveLeft:
				emit selectionChanged();
				return prevIndex();

			case QAbstractItemView::MoveDown: {
				if ( idx.row() == appModel->rowCount() - 1 ) {
					emit selectionChanged();
					return appModel->index( 0, 0, idx.parent() );
				}

				else {
					emit selectionChanged();
					return appModel->index( idx.row() + 1, 0, idx.parent() );
				}

				emit selectionChanged();
				return belowIndex();
			}

			case QAbstractItemView::MoveUp: {

				if ( idx.row() == 0 ) {
					emit selectionChanged();
					return appModel->index( appModel->rowCount() - 1, 0, idx.parent() );
				}
				else {
					emit selectionChanged();
					return appModel->index( idx.row() - 1, 0, idx.parent() );
				}

				emit selectionChanged();
				return aboveIndex();
			}

			case QAbstractItemView::MoveHome: {

				emit selectionChanged();
				return firstIndex();
			}

			case QAbstractItemView::MoveEnd: {

				emit selectionChanged();
				return lastIndex();
			}

			case QAbstractItemView::MovePageUp: {

				emit selectionChanged();
				return indexPageAbove();
			}

			case QAbstractItemView::MovePageDown: {

				emit selectionChanged();
				return indexPageBelow();
			}

			default: {
				emit selectionChanged();
				return QModelIndex();
			}
		}
	}

	/* If there is no current index */
	else {
		switch( cursorAction ) {
			case QAbstractItemView::MoveHome:
			case QAbstractItemView::MoveRight:
			case QAbstractItemView::MoveNext:
			case QAbstractItemView::MoveDown:
				emit selectionChanged();
				return firstIndex();

			case QAbstractItemView::MoveEnd:
			case QAbstractItemView::MoveLeft:
			case QAbstractItemView::MovePrevious:
			case QAbstractItemView::MoveUp:
				emit selectionChanged();
				return firstIndex();

			default:
				emit selectionChanged();
				return firstIndex();
		}
	}

	emit selectionChanged();
	return idx;
};

int NBOpenWithView::horizontalOffset() const {

	return horizontalScrollBar()->value();
};

int NBOpenWithView::verticalOffset() const {

	return verticalScrollBar()->value();
};

void NBOpenWithView::scrollContentsBy( int dx, int dy ) {

	scrollDirtyRegion( dx, dy );
	viewport()->scroll( dx, dy );
};

void NBOpenWithView::setSelection( const QRect &rect, QFlags<QItemSelectionModel::SelectionFlag> flags ) {

	QRect rectangle = rect.translated( horizontalScrollBar()->value(), verticalScrollBar()->value() ).normalized();

	calculateRectsIfNecessary();
	QHashIterator<int, QRect> rIt( rectForRow );
	int firstRow = appModel->rowCount();
	int lastRow = -1;
	while ( rIt.hasNext() ) {

		rIt.next();
		if ( rIt.value().intersects( rectangle ) ) {
			firstRow = firstRow < rIt.key() ? firstRow : rIt.key();
			lastRow = lastRow > rIt.key() ? lastRow : rIt.key();
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

QRegion NBOpenWithView::visualRegionForSelection( const QItemSelection &selection ) const {

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

QModelIndexList NBOpenWithView::selectedIndexes() {

	QSet<QModelIndex> idxSet;
	idxSet.unite( QSet<QModelIndex>::fromList( mSelectedIndexes ) );
	idxSet.unite( QSet<QModelIndex>::fromList( selectionModel()->selectedIndexes() ) );

	return idxSet.toList();
};

QModelIndexList NBOpenWithView::selection() {

	QSet<QModelIndex> idxSet;
	idxSet.unite( QSet<QModelIndex>::fromList( mSelectedIndexes ) );
	idxSet.unite( QSet<QModelIndex>::fromList( selectionModel()->selectedIndexes() ) );

	return idxSet.toList();
};

bool NBOpenWithView::isIndexVisible( QModelIndex idx ) const {

	/* We consider the space reserved for the category but not the indexes listed under it */
	if ( hiddenCategories.contains( appModel->category( idx ) ) )
		return false;

	QRect rect = viewportRectForRow( idx.row() );

	return viewport()->rect().intersects( rect );
};

void NBOpenWithView::paintEvent( QPaintEvent* event ) {

	QPainter painter( viewport() );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing );

	painter.fillRect( viewport()->rect(), Qt::transparent );

	/* We need to draw the categories only if the model is categorization enabled */
	if ( appModel->isCategorizationEnabled() ) {
		for ( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {

			QRect rect = categoryRect( catIdx );
			if ( not viewport()->rect().intersects( rect ) )
				continue;

			paintCategory( &painter, rect, categoryList.at( catIdx ) );
		}
	}

	/* We paint the icons index-wise. */
	for ( int row = 0; row < appModel->rowCount( rootIndex() ); row++ ) {
		QModelIndex idx = appModel->index( row, 0, rootIndex() );
		if ( not isIndexVisible( idx ) )
			continue;

		QRect rect = viewportRectForRow( row );
		QStyleOptionViewItem option = QStyleOptionViewItem( viewOptions() );

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
		QFileInfo ftype = appModel->nodeInfo( idx );

		/* Dark text colors will suit here */
		if ( isBrightColor( pltt.color( QPalette::Base ), pltt.color( QPalette::Highlight ) ) ) {
			if ( option.state & QStyle::State_Selected )
				pltt.setColor( QPalette::Text, pltt.color( QPalette::HighlightedText ) );

			else
				pltt.setColor( QPalette::Text, palette().color( QPalette::Text ) );

			/* BrightText option will be used for drawing the 'current rect' */
			pltt.setColor( QPalette::BrightText, pltt.color( QPalette::Highlight ).darker() );
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

		NBOpenWithDelegate *dlgt = qobject_cast<NBOpenWithDelegate*>( itemDelegate() );
		dlgt->paint( &painter, option, idx );
	}

	painter.end();
	event->accept();
};

void NBOpenWithView::paintCategory( QPainter *painter, const QRect &rectangle, const QString &text ) const {

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

	QPixmap pix = appModel->pixmapForCategory( text, hiddenCategories.contains( text ) );
	QPoint topLeft = rectangle.topLeft();
	QPoint topRight = rectangle.topRight();

	painter->drawPixmap( topLeft.x() + 4, topLeft.y() + 4, 16, 16, pix );

	/* Category Menu prompt */
	painter->drawPixmap( topRight.x() - 20, topLeft.y() + 4, 16, 16, QIcon( ":/icons/dot.png" ).pixmap( 16 ) );

	/* We draw the '+' if the category is folded or hidden */
	if ( hiddenCategories.contains( text ) ) {
		painter->drawPixmap( topRight.x() - 36, topLeft.y() + 4, 16, 16, QIcon( ":/icons/category-expand.png" ).pixmap( 16 ) );
	}

	/* We have to draw the '-' only when the number of rows in a category is more than 1 */
	else if ( appModel->indexListCountForCategory( text ) > 1 ) {
		painter->drawPixmap( topRight.x() - 36, topLeft.y() + 4, 16, 16, QIcon( ":/icons/category-collapse.png" ).pixmap( 16 ) );
	}

	QFont categoryFont = qApp->font();
	categoryFont.setBold( true );
	painter->setFont( categoryFont );
	painter->drawText( topLeft.x() + 24, topLeft.y(), rectangle.width() - 48, rectangle.height(), Qt::AlignVCenter, text );

	painter->restore();
};

void NBOpenWithView::paintSelection( QPainter *painter, const QModelIndexList indexes ) const {

	if ( not indexes.count() )
		return;

	painter->save();
	painter->setPen( QPen( palette().color( QPalette::Highlight ), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

	QColor bg = palette().color( QPalette::Highlight );
	bg.setAlpha( 120 );
	painter->setBrush( bg );

	QPainterPath path;
	path.setFillRule( Qt::WindingFill );
	Q_FOREACH( QModelIndex idx, indexes )
		path.addRoundedRect( QRectF( viewportRectForRow( idx.row() ) ), 4, 4 );

	painter->drawPath( path );
	painter->restore();
};

void NBOpenWithView::resizeEvent( QResizeEvent *rEvent ) {

	viewWidth = rEvent->size().width() - myContentsMargins.left() - myContentsMargins.right() - myInlayMargins.left() - myInlayMargins.right();

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBOpenWithView::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( mpEvent->button() != Qt::LeftButton ) {
		QAbstractItemView::mousePressEvent( mpEvent );
		return;
	}

	/* Index at mouse position */
	QModelIndex idx = indexAt( mpEvent->pos() );

	/* NoModifier/AltModifier pressed with the mouse */
	if ( qApp->keyboardModifiers().testFlag( Qt::NoModifier ) or qApp->keyboardModifiers().testFlag( Qt::AltModifier ) ) {

		/* Valid index */
		if ( idx.isValid() ) {
			/* Index already selected, start the drag */
			if ( mSelectedIndexes.contains( idx ) or selectionModel()->isSelected( idx ) ) {
				int iSize = myIconSize.width();
				if ( iSize < 48 )
					return;

				return;
			}

			/* Index not selected, select it */
			else {
				selectionModel()->clearSelection();
				mSelectedIndexes.clear();

				mSelectedIndexes << idx;
				mSelStartIdx = idx;
			}

			/* Repaint the viewport */
			viewport()->repaint();
		}

		/* Valid category */
		else if ( categoryAt( mpEvent->pos() ).count() ) {
			QRect rct = categoryRect( categoryList.indexOf( categoryAt( mpEvent->pos() ) ) );
			QString catName = categoryAt( mpEvent->pos() );
			QPoint topLeft = rct.topLeft();
			QPoint topRight = rct.topRight();

			/* Category controls clicked */
			if ( QRect( topRight.x() - 20, topLeft.y() + 4, 16, 16 ).contains( mpEvent->pos() ) ) {
				// Show Category Menu
				QModelIndexList list = appModel->indexListForCategory( catName );
				NBCategoryMenu *catMenu = new NBCategoryMenu( catName, appModel->pixmapForCategory( catName ), appModel->currentDir(), list, this );
				connect( catMenu, SIGNAL( showCategory( QString ) ), this, SLOT( showCategory( QString ) ) );
				connect( catMenu, SIGNAL( foldCategory( QString ) ), this, SLOT( foldCategory( QString ) ) );
				connect( catMenu, SIGNAL( hideCategory( QString ) ), this, SLOT( hideCategory( QString ) ) );

				catMenu->exec( mapToGlobal( mpEvent->pos() ) );
			}

			else if ( QRect( topRight.x() - 36, topLeft.y() + 4, 16, 16 ).contains( mpEvent->pos() ) ) {
				/* The viewMode is Details, Show/Hide it */
				showHideCategory( catName );
			}

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
	}

	/* Shift Modifier */
	else if ( qApp->keyboardModifiers() & Qt::ShiftModifier ) {
		/* Current Index */

		/* Valid index */
		if ( idx.isValid() ) {
			/* Forward selection */
			if ( mSelStartIdx.row() < idx.row() ) {
				mSelectedIndexes.clear();
				for( int i = mSelStartIdx.row(); i <= idx.row(); i++ )
					mSelectedIndexes << appModel->index( i, 0, rootIndex() );
			}

			/* Reverse selection */
			else {
				mSelectedIndexes.clear();
				for( int i = idx.row(); i <= mSelStartIdx.row(); i++ )
					mSelectedIndexes << appModel->index( i, 0, rootIndex() );
			}

			/* Repaint the viewport */
			viewport()->repaint();
		}
	}

	/* Control Modifier */
	else if ( qApp->keyboardModifiers() & Qt::ControlModifier ) {
		/* Valid index */
		if ( idx.isValid() ) {
			if ( mSelectedIndexes.contains( idx ) )
				mSelectedIndexes.removeAll( idx );

			else
				mSelectedIndexes << idx;

			viewport()->repaint();
		}
	}

	/* Set the clicked index as the current index */
	if( idx.isValid() ) {
		setCurrentIndex( idx );
	}

	mpEvent->accept();
};

void NBOpenWithView::mouseMoveEvent( QMouseEvent *mmEvent ) {

	QToolTip::hideText();

	/* Left Mouse Button is pressed down */
	if ( mmEvent->buttons() & Qt::LeftButton ) {
		/* Nothing here. Single selections, so mouse down and move is ignored */
		mmEvent->accept();
		return;
	}

	else {
		setCursor( Qt::ArrowCursor );

		QString ctg = categoryAt( mmEvent->pos() );
		if ( ctg.count() ) {
			QRect rct = categoryRect( categoryList.indexOf( ctg ) );
			QPoint topLeft = rct.topLeft();
			QPoint topRight = rct.topRight();

			/* +/- Buttons */
			if ( QRect( topRight.x() - 36, topLeft.y() + 4, 16, 16 ).contains( mmEvent->pos() ) ) {
				if ( appModel->indexListCountForCategory( ctg ) > 1 )
					QToolTip::showText( mapToGlobal( mmEvent->pos() ), "Click to fold/unfold", this, QRect(), 1500 );
			}

			else if ( QRect( topRight.x() - 20, topLeft.y() + 4, 16, 16 ).contains( mmEvent->pos() ) ) {
				QToolTip::showText( mapToGlobal( mmEvent->pos() ), "Click to show category menu", this, QRect(), 1500 );
			}
		}
	}

	viewport()->repaint();
	QAbstractItemView::mouseMoveEvent( mmEvent );
};

void NBOpenWithView::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	mrEvent->accept();
};

void NBOpenWithView::mouseDoubleClickEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		QModelIndex idx = indexAt( mEvent->pos() );
		if ( idx.isValid() ) {
			/* We need to decide what has to be done here */
		}

		else if ( not categoryAt( mEvent->pos() ).isEmpty() ) {
			showHideCategory( categoryAt( mEvent->pos() ) );
		}
	}

	mEvent->accept();
};

QModelIndex NBOpenWithView::nextIndex() {

	/* The next index is in the current category */
	int nextIdx = curIdx.second + 1;
	if ( nextIdx < appModel->indexListCountForCategory( curIdx.first ) ) {
		QString curCat = categoryList.at( curIdx.first );

		/* If the current category is not folded, just return the next index */
		if ( not hiddenCategories.contains( curCat ) ) {
			curIdx = qMakePair( curIdx.first, nextIdx );
			persistentVCol = nextIdx;
			return appModel->indexListForCategory( curIdx.first )[ nextIdx ];
		}

		/* If the current category is folded, and the next index in in the first row of the current category */
		else if ( hiddenCategories.contains( curCat ) and nextIdx < 0 ) {
			curIdx = qMakePair( curIdx.first, nextIdx );
			persistentVCol = nextIdx;
			return appModel->indexListForCategory( curIdx.first )[ nextIdx ];
		}
	}

	/* The next index is the first index of the next visible category */
	int nCatIdx = curIdx.first < categoryList.count() - 1 ? curIdx.first + 1 : 0;
	while ( ( nCatIdx < categoryList.count() ) and ( nCatIdx != curIdx.first ) ) {
		if ( not hiddenCategories.contains( categoryList.at( nCatIdx ) ) )
			break;

		nCatIdx += 1;
		if ( nCatIdx == categoryList.count() )
			nCatIdx = 0;
	}

	curIdx = qMakePair( nCatIdx, 0 );
	persistentVCol = 0;
	return appModel->indexListForCategory( nCatIdx ).first();

	return QModelIndex();
};

QModelIndex NBOpenWithView::prevIndex() {

	/* The previous index is in the current category */
	int prevIdx = curIdx.second - 1;
	if ( prevIdx >= 0 ) {
		curIdx = qMakePair( curIdx.first, prevIdx );
		persistentVCol = prevIdx;
		return appModel->indexListForCategory( curIdx.first )[ prevIdx ];
	}

	/* The prev index is the last index of the previous visible category */
	int pCatIdx = curIdx.first > 0 ? curIdx.first - 1 : categoryList.count() - 1;
	while ( ( pCatIdx < categoryList.count() ) and ( pCatIdx != curIdx.first ) ) {
		if ( not hiddenCategories.contains( categoryList.at( pCatIdx ) ) )
			break;

		pCatIdx -= 1;
		if ( pCatIdx == -1 )
			pCatIdx = categoryList.count() - 1;
	}

	QString prevCat = categoryList.at( pCatIdx );
	/* If the previous category is folded */
	if ( hiddenCategories.contains( prevCat ) ) {
		curIdx = qMakePair( pCatIdx, 1 - 2 );
		persistentVCol = 1 - 2;
		return appModel->indexListForCategory( pCatIdx )[ 1 - 2 ];
	}

	/* The category is not folded */
	else {
		QModelIndexList catIdxes = appModel->indexListForCategory( pCatIdx );
		curIdx = qMakePair( pCatIdx, catIdxes.count() - 1 );
		persistentVCol = catIdxes.count() - 1;
		return catIdxes.last();
	}

	return QModelIndex();
};

QModelIndex NBOpenWithView::belowIndex() {

	QModelIndex idx = currentIndex();

	/* If the current category is not folded, just return the next index */
	if ( not hiddenCategories.contains( appModel->category( idx ) ) ) {
		/* The next index is in the current category */
		int nextIdx = curIdx.second + 1;
		if ( nextIdx <= appModel->indexListCountForCategory( curIdx.first ) - 1 ) {

			curIdx = qMakePair( curIdx.first, nextIdx );
			return appModel->indexListForCategory( curIdx.first )[ nextIdx ];
		}

		/* If the number of rows is greater than the current row of the current index */
		else {
			int curRow = curIdx.second;
			int catRows = appModel->indexListCountForCategory( curIdx.first );
			if ( catRows > curRow and curRow != 0 ) {
				curIdx = qMakePair( curIdx.first, appModel->indexListCountForCategory( curIdx.first ) - 1 );
				return appModel->indexListForCategory( curIdx.first ).last();
			}
		}
	}

	/* The next index is in the next visible category */
	int nCatIdx = curIdx.first < categoryList.count() - 1 ? curIdx.first + 1 : 0;
	while ( ( nCatIdx < categoryList.count() ) and ( nCatIdx != curIdx.first ) ) {
		if ( not hiddenCategories.contains( categoryList.at( nCatIdx ) ) )
			break;

		nCatIdx += 1;
		if ( nCatIdx == categoryList.count() )
			nCatIdx = 0;
	}

	/* The next category is folded */
	if ( hiddenCategories.contains( categoryList.at( nCatIdx ) ) ) {
		if ( persistentVCol <= 1 - 2 ) {
			curIdx = qMakePair( nCatIdx, persistentVCol );
			return appModel->indexListForCategory( nCatIdx ).at( persistentVCol );
		}

		else {
			curIdx = qMakePair( nCatIdx, 1 - 2 );
			return appModel->indexListForCategory( nCatIdx ).at( 1 - 2 );
		}
	}

	if ( persistentVCol < appModel->indexListCountForCategory( nCatIdx ) - 1 ) {
		curIdx = qMakePair( nCatIdx, persistentVCol );
		return appModel->indexListForCategory( nCatIdx ).at( persistentVCol );
	}

	else {
		curIdx = qMakePair( nCatIdx, appModel->indexListCountForCategory( nCatIdx ) - 1 );
		return appModel->indexListForCategory( nCatIdx ).last();
	}

	return QModelIndex();
};

QModelIndex NBOpenWithView::aboveIndex() {

	/* The previous index is in the current category */
	/* Note that if this category is folded, the next block will have already put the
	 * current index in the first row of this category. */
	int curRow = curIdx.second;
	if ( curRow > 0 ) {
		int nIdx = ( curRow - 1 ) * 1 + persistentVCol;
		curIdx = qMakePair( curIdx.first, nIdx );
		return appModel->indexListForCategory( curIdx.first ).at( nIdx );
	}

	/* The previous index is in the previous category */
	int pCatIdx = curIdx.first > 0 ? curIdx.first - 1 : categoryList.count() - 1;
	while ( ( pCatIdx < categoryList.count() ) and ( pCatIdx != curIdx.first ) ) {
		if ( not hiddenCategories.contains( categoryList.at( pCatIdx ) ) )
			break;

		pCatIdx -= 1;
		if ( pCatIdx == -1 )
			pCatIdx = categoryList.count() - 1;
	}

	/* The prev index is the last visible index of the previous visible category */
	QString prevCat = categoryList.at( pCatIdx );
	if ( hiddenCategories.contains( prevCat ) ) {
		if ( persistentVCol <= appModel->indexListCountForCategory( prevCat ) - 1 ) {
			if ( persistentVCol <= 1 - 2 ) {
				curIdx = qMakePair( pCatIdx, persistentVCol );
				return appModel->indexListForCategory( prevCat ).at( persistentVCol );
			}

			else {
				curIdx = qMakePair( pCatIdx, 1 - 2 );
				return appModel->indexListForCategory( prevCat ).at( 1 - 2 );
			}
		}

		/* Safegaurd: This should never happen */
		else {
			curIdx = qMakePair( pCatIdx, appModel->indexListCountForCategory( pCatIdx ) - 1 );
			return appModel->indexListForCategory( pCatIdx ).last();
		}
	}

	/* Last index of the previous category is the above index */
	int lIdxCol = appModel->indexListCountForCategory( prevCat );
	if ( lIdxCol <= persistentVCol and lIdxCol != 0 ) {
		curIdx = qMakePair( pCatIdx, appModel->indexListCountForCategory( pCatIdx ) - 1 );
		return appModel->indexListForCategory( pCatIdx ).last();
	}

	else {
		int nRows = appModel->indexListCountForCategory( prevCat );
		int pIdx = ( nRows - 1 ) * 1 + persistentVCol;
		curIdx = qMakePair( pCatIdx, pIdx );
		return appModel->indexListForCategory( pCatIdx ).at( pIdx );
	}

	return QModelIndex();
};

QModelIndex NBOpenWithView::firstIndex() {

	persistentVCol = 0;

	int firstVisibleCatIdx = -1;
	Q_FOREACH( QString category, categoryList ) {
		if ( not hiddenCategories.contains( category ) ) {
			firstVisibleCatIdx = categoryList.indexOf( category );
			break;
		}
	}

	curIdx = qMakePair( firstVisibleCatIdx, 0 );

	verticalScrollBar()->setValue( 0 );

	if ( firstVisibleCatIdx >= 0 )
		return appModel->indexListForCategory( categoryList.at( firstVisibleCatIdx ) ).first();

	else
		return QModelIndex();
};

QModelIndex NBOpenWithView::lastIndex() {

	int lastVisibleCatIdx = -1;
	for( lastVisibleCatIdx = categoryList.count() - 1; lastVisibleCatIdx >= 0; lastVisibleCatIdx-- ) {
		if ( not hiddenCategories.contains( categoryList.at( lastVisibleCatIdx ) ) )
			break;
	}

	verticalScrollBar()->setValue( verticalScrollBar()->maximum() );

	int categoryNodeCount = appModel->indexListCountForCategory( lastVisibleCatIdx );

	if ( hiddenCategories.contains( categoryList.at( lastVisibleCatIdx ) ) ) {
		persistentVCol = ( categoryNodeCount < 1 ? categoryNodeCount : 1 - 2 );
		curIdx = qMakePair( lastVisibleCatIdx, persistentVCol );
	}

	else {
		persistentVCol = categoryNodeCount - 1;
		curIdx = qMakePair( lastVisibleCatIdx, categoryNodeCount - 1 );
	}

	if ( lastVisibleCatIdx >= 0 )
		return appModel->indexListForCategory( categoryList.at( lastVisibleCatIdx ) ).at( curIdx.second );

	return QModelIndex();
};

QModelIndex NBOpenWithView::indexPageBelow() {

	QModelIndex idx = currentIndex();

	QRectF curRect = viewportRectForRow( idx.row() );
	QPointF newPoint = curRect.topLeft() + QPointF( viewWidth / 2, viewport()->height() );

	if ( indexAt( newPoint.toPoint() ).isValid() )
		return indexAt( newPoint.toPoint() );

	else
		return lastIndex();
};

QModelIndex NBOpenWithView::indexPageAbove() {

	QModelIndex idx = currentIndex();

	QRectF curRect = viewportRectForRow( idx.row() );
	QPointF newPoint = curRect.topLeft() - QPointF( -viewWidth / 2, viewport()->height() );

	if ( indexAt( newPoint.toPoint() ).isValid() )
		return indexAt( newPoint.toPoint() );

	else if ( indexAt( QPoint( 0, -myCategoryHeight - myCategorySpacing ) + newPoint.toPoint() ).isValid() )
		return indexAt( newPoint.toPoint() );

	else
		return firstIndex();
};

void NBOpenWithView::calculateRectsIfNecessary() const {

	if ( not hashIsDirty )
		return;

	rectForRow.clear();
	rectForCategory.clear();

	calculateCategorizedRects();

	if ( idealHeight <= viewport()->height() ) {

		verticalScrollBar()->setRange( 0, 0 );
		verticalScrollBar()->hide();
	}

	else {
		verticalScrollBar()->show();

		verticalScrollBar()->setSingleStep( 32 );
		verticalScrollBar()->setPageStep( viewport()->height() );
		verticalScrollBar()->setRange( 0, qMax( 0, idealHeight - viewport()->height() ) );
	}
};

void NBOpenWithView::calculateCategorizedRects() const {

	if ( not hashIsDirty )
		return;

	int catX = myContentsMargins.left() ;
	int catW = viewWidth + myInlayMargins.left() + myInlayMargins.right();
	int x = myContentsMargins.left() + myInlayMargins.left();

	int minY = myContentsMargins.top();

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = appModel->indexListForCategory( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		rectForCategory[ catIdx ] = QRect( catX, minY, catW, myCategoryHeight );
		minY += myCategoryHeight;

		// We consider the space reserved for the category but not the indexes listed under it
		if ( hiddenCategories.contains( categoryList[ catIdx ] ) )
			continue;

		else {
			for( int lrow = 0; lrow < mList.count(); lrow++ ) {
				rectForRow[ mList[ lrow ].row() ] = QRect( x, minY, viewWidth, 32 );
				minY += 32;
			}
		}

		minY += myCategorySpacing;
	}

	idealHeight = minY + myContentsMargins.bottom();

	hashIsDirty = false;
	viewport()->update();
};

QRect NBOpenWithView::viewportRectForRow( int row ) const {

	calculateRectsIfNecessary();
	QRect rct = rectForRow.value( row );
	if ( not rct.isValid() )
		return QRect();

	return QRect( rct.x(), rct.y() - verticalScrollBar()->value(), rct.width(), rct.height() );
};

void NBOpenWithView::showHideCategory( QString category ) {

	if ( hiddenCategories.contains( category ) )
		hiddenCategories.removeAll( category );

	else
		hiddenCategories.append( category );

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBOpenWithView::toggleFoldCategory( QString category ) {

	showHideCategory( category );
};

void NBOpenWithView::showCategory( QString category ) {

	hiddenCategories.removeAll( category );

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBOpenWithView::foldCategory( QString category ) {

	hideCategory( category );
};

void NBOpenWithView::hideCategory( QString category ) {

	hiddenCategories.append( category );

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBOpenWithView::currentChanged( const QModelIndex &cur, const QModelIndex &prev ) {

	QAbstractItemView::currentChanged( cur, prev );

	curIdx = qMakePair( categoryList.indexOf( appModel->category( cur ) ), appModel->indexInCategory( cur ) );
	persistentVCol = appModel->indexInCategory( cur );
};
