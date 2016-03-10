/*
	*
	* NBAppsView.cpp - NewBreeze FolderView Class
	*
*/

#include <NBOpenWithView.hpp>

NBOpenWithView::NBOpenWithView() : QAbstractItemView() {

	idealHeight = 0;
	viewWidth = 0;
	hashIsDirty = true;

	// Category Height
	myCategoryHeight = 32;

	// Category Spacing
	myCategorySpacing = 10;

	// Contents Margins
	myContentsMargins = QMargins( 10, 10, 10, 10 );

	// Inlay Margins
	myInlayMargins = QMargins( 10, 0, 10, 0 );

	// Grid Size
	myItemHeight = 32;

	// Icon Size
	myIconSize = QSize( 24, 24 );

	// Set the Apps Delegate
	setItemDelegate( new NBOpenWithDelegate() );

	// Applications Model
	appModel = new NBApplicationsModel();
	setModel( appModel );

	// Selection
	setSelectionMode( QAbstractItemView::SingleSelection );
	setSelectionBehavior( QAbstractItemView::SelectRows );

	// Internal Object Name
	setObjectName( "mainList" );

	// Minimum Size
	setMinimumWidth( 256 );

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
	setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	verticalScrollBar()->show();
	verticalScrollBar()->setRange( 0, 0 );
};

void NBOpenWithView::setModel( QAbstractItemModel *model ) {

	QAbstractItemView::setModel( model );
	hashIsDirty = true;
};

int NBOpenWithView::categoryHeight() const {

	return myCategoryHeight;
};

void NBOpenWithView::setCategoryHeight( int newCategoryHeight ) {

	myCategoryHeight = newCategoryHeight;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

int NBOpenWithView::itemHeight() const {

	return myItemHeight;
};

void NBOpenWithView::setItemHeight( int newItemHeight ) {

	myItemHeight = newItemHeight;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

QSize NBOpenWithView::iconSize() const {

	return myIconSize;
};

void NBOpenWithView::setIconSize( QSize newIconSize ) {

	myIconSize = newIconSize;
	viewport()->update();
};

void NBOpenWithView::setIconSize( int nWidth, int nHeight ) {

	setIconSize( QSize( nWidth, nHeight ) );
};

QMargins NBOpenWithView::contentsMargins() const {

	return myContentsMargins;
};

void NBOpenWithView::setContentsMargins( QMargins newContentsMargins ) {

	myContentsMargins = newContentsMargins;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBOpenWithView::setContentsMargins( int nLeft, int nTop, int nRight, int nBottom ) {

	setContentsMargins( QMargins( nLeft, nTop, nRight, nBottom ) );
};

QMargins NBOpenWithView::inlayMargins() const {

	return myInlayMargins;
};

void NBOpenWithView::setInlayMargins( QMargins newInlayMargins ) {

	myInlayMargins = newInlayMargins;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBOpenWithView::setInlayMargins( int nLeft, int nTop, int nRight, int nBottom ) {

	setInlayMargins( QMargins( nLeft, nTop, nRight, nBottom ) );

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

int NBOpenWithView::categorySpacing() const {

	return myCategorySpacing;
};

void NBOpenWithView::setCategorySpacing( int newCategorySpacing ) {

	myCategorySpacing = newCategorySpacing;

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBOpenWithView::calculateRectsIfNecessary() const {

	if ( not hashIsDirty )
		return;

	int y = 0, prevRows = 0, totalRows = 0;
	QStringList categoryList = appModel->categories();
	viewWidth = viewport()->width();

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = appModel->indexListForCategory( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		int minY = myContentsMargins.top() + catIdx * myCategoryHeight + catIdx * myCategorySpacing + totalRows * myItemHeight;

		yOffsetForCategory[ catIdx ] = minY;

		if ( not appModel->isCategoryVisible( appModel->category( mList.value( 0 ) ) ) )
			continue;

		// Mimimum X and Y for indexes
		minY += myCategoryHeight;

		prevRows = appModel->indexListCountForCategory( categoryList.at( catIdx ) );
		totalRows += prevRows;

		for( int lrow = 0; lrow < mList.count(); lrow++ ) {
			y = minY + lrow * myItemHeight;

			yOffsetForRow[ mList[ lrow ].row() ] = y;
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
	idealHeight += appModel->rowCount() * myItemHeight;

	hashIsDirty = false;
	viewport()->update();
};

QRect NBOpenWithView::visualRect( const QModelIndex &index ) const {

	QRect rect;
	if ( index.isValid() )
		rect = viewportRectForRow( index.row() );

	return rect;
};

QRect NBOpenWithView::viewportRectForRow( int row ) const {

	calculateRectsIfNecessary();
	int yOffset = yOffsetForRow.value( row );
	if ( not yOffset )
		return QRect();

    return QRect( myInlayMargins.left(), yOffset - verticalScrollBar()->value(), viewWidth - myInlayMargins.right(), myItemHeight );
};

QRect NBOpenWithView::categoryRect( int categoryIndex ) const {

	calculateRectsIfNecessary();
	int yOffset = yOffsetForCategory.value( categoryIndex );
	if ( not yOffset )
		return QRect();

    return QRect( 0, yOffset - verticalScrollBar()->value(), viewWidth * 0.9, myCategoryHeight );
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
	QHashIterator<int, int> i( yOffsetForRow );
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( 0, i.value(), viewWidth, myItemHeight ).contains( point ) )
			return appModel->index( i.key(), 0, rootIndex() );
	}
	return QModelIndex();
};

QString NBOpenWithView::categoryAt( const QPoint &point_ ) const {

	QPoint point( point_ );
	point.ry() += verticalScrollBar()->value();
	calculateRectsIfNecessary();
	QHashIterator<int, int> i( yOffsetForCategory );
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( 0, i.value(), viewWidth, myItemHeight ).contains( point ) )
			return appModel->categories().value( i.key() );
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

QModelIndex NBOpenWithView::moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers ) {

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
				int vrow = rowInCategory / 1;
				int vcol = 0;
				int nrow = ( int )ceil( 1.0 * thisCategoryIndexes.count() / 1 );

				int newVRow = vrow + 1;

				if ( ( newVRow * 1 + vcol ) < thisCategoryIndexes.count() ) {
					// We have an index below this index from the same category
					return thisCategoryIndexes.value( newVRow * 1 + vcol );
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
				int vrow = rowInCategory / 1;
				int vcol = 0;

				int newVRow = vrow - 1;

				if ( newVRow >= 0 ) {

					// This means that there is a visual row before this one
					return thisCategoryIndexes.value( newVRow * 1 + vcol );
				}
				else {
					if ( prevCategoryIndexes.count() ) {
						int nrow = ( int )ceil( 1.0 * prevCategoryIndexes.count() / 1 );
						if ( prevCategoryIndexes.count() > ( nrow - 1 ) * 1 + vcol )
							return prevCategoryIndexes.value( ( nrow - 1 ) * 1 + vcol );

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
	QHashIterator<int, int> i( yOffsetForRow );
	int firstRow = appModel->rowCount();
	int lastRow = -1;
	while ( i.hasNext() ) {

		i.next();
		if ( QRect( 0, i.value(), viewWidth, myItemHeight ).intersects( rectangle ) ) {
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

void NBOpenWithView::paintEvent( QPaintEvent* event ) {

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

		itemDelegate()->paint( &painter, option, index );
	}

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
	painter->drawLine( rectangle.bottomLeft(), rectangle.bottomRight() );

	painter->setPen( QPen( textColor ) );

	QPixmap pix = pixmapForCategory( text );
	QPoint topLeft = rectangle.topLeft();

	painter->drawPixmap( topLeft.x() + 4, topLeft.y() + 4, 24, 24, pix );
	QFont categoryFont = qApp->font();
	categoryFont.setBold( true );
	painter->setFont( categoryFont );
	painter->drawText( topLeft.x() + myItemHeight, topLeft.y(), rectangle.width() - 64, rectangle.height(), Qt::AlignVCenter, text );

	painter->restore();
};

QPixmap NBOpenWithView::pixmapForCategory( QString categoryText ) const {

	if ( categoryText == "Accessories" )
		return QIcon::fromTheme( "applications-accessories" ).pixmap( 24, 24 );

	else if ( categoryText == "Development" )
		return QIcon::fromTheme( "applications-development" ).pixmap( 24, 24 );

	else if ( categoryText == "Education" )
		return QIcon::fromTheme( "applications-education" ).pixmap( 24, 24 );

	else if ( categoryText == "Games" )
		return QIcon::fromTheme( "applications-games" ).pixmap( 24, 24 );

	else if ( categoryText == "Graphics" )
		return QIcon::fromTheme( "applications-graphics" ).pixmap( 24, 24 );

	else if ( categoryText == "Internet" )
		return QIcon::fromTheme( "applications-internet" ).pixmap( 24, 24 );

	else if ( categoryText == "Multimedia" )
		return QIcon::fromTheme( "applications-multimedia" ).pixmap( 24, 24 );

	else if ( categoryText == "Office" )
		return QIcon::fromTheme( "applications-office" ).pixmap( 24, 24 );

	else if ( categoryText == "Science and Math" )
		return QIcon::fromTheme( "applications-science" ).pixmap( 24, 24 );

	else if ( categoryText == "Settings" )
		return QIcon::fromTheme( "preferences-system" ).pixmap( 24, 24 );

	else if ( categoryText == "System" )
		return QIcon::fromTheme( "applications-system" ).pixmap( 24, 24 );

	else
		return QIcon::fromTheme( "preferences-other" ).pixmap( 24, 24 );
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

void NBOpenWithView::resizeEvent( QResizeEvent* ) {

	hashIsDirty = true;

	calculateRectsIfNecessary();
	updateGeometries();
};

void NBOpenWithView::updateGeometries() {

	verticalScrollBar()->setSingleStep( myItemHeight );
	verticalScrollBar()->setPageStep( viewport()->height() );
	verticalScrollBar()->setRange( 0, qMax( 0, idealHeight - viewport()->height() ) );
};

void NBOpenWithView::mousePressEvent( QMouseEvent *event ) {

	QAbstractItemView::mousePressEvent( event );
	setCurrentIndex( indexAt( event->pos() ) );
};

void NBOpenWithView::mouseDoubleClickEvent( QMouseEvent *event ) {

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
