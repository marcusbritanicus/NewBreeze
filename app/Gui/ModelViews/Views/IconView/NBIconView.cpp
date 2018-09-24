/*
	*
	* NBIconView.cpp - NewBreeze IconView Class
	*
*/

#include "NBIconView.hpp"
#include "NBStyleOptionViewItem.hpp"
#include "NBFunctions.hpp"
#include "NBPluginManager.hpp"
#include "NBMessageDialog.hpp"
#include "NBCategoryMenu.hpp"
#include "NBTerminal.hpp"

NBIconView::NBIconView( NBItemViewModel *fsModel, QWidget *parent ) : QAbstractItemView( parent ) {

	/* Current folder viewMode */
	currentViewMode = Settings->View.ViewMode;

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

	/* Grid Size */
	myGridSizeMin = QSize( 120, 80 );
	myGridSize = QSize( 120, 80 );
	myItemSize = QSize( 110, 70 );

	/* Persistent vertical column */
	persistentVCol = 0;
	/* QPair( catIdx, elmInCatIdx */
	curIdx = qMakePair( 0, 0 );

	/* Items per visual row */
	itemsPerRow = 1;
	numberOfRows = 0;
	padding = 0;

	/* Set the Apps Delegate */
	setItemDelegate( new NBIconDelegate() );

	/* Applications Model */
	cModel = fsModel;
	setModel( cModel );

	/* Icon Size */
	if ( Settings->View.ViewMode == "Icons" )
		setIconSize( Settings->View.IconsImageSize );

	else if ( Settings->View.ViewMode == "Tiles" )
		setIconSize( Settings->View.TilesImageSize );

	else
		setIconSize( Settings->View.DetailsImageSize );

	/* Default Selection Rules */
	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setSelectionBehavior( QAbstractItemView::SelectRows );
	mSelectedIndexes = QModelIndexList();

	/* Internal Object Name */
	setObjectName( "mainList" );

	/* Styling */
	setStyleSheet( "#mainList{ border:none; }" );

	/* Minimum Size */
	setMinimumWidth( 320 );

	/* Focus Policy */
	setFocusPolicy( Qt::StrongFocus );

	/* Font */
	setFont( qApp->font() );

	/* Mouse tracking */
	setMouseTracking( true );

	/* No Horizontal ScrollBar */
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	/* DragAndDrop */
	viewport()->setAcceptDrops( true );
	setDragEnabled( true );
	setAcceptDrops( true );
	setDragDropMode( QListView::DragDrop );
	setDropIndicatorShown( true );

	/* Context Menu */
	setContextMenuPolicy( Qt::CustomContextMenu );

	connect( cModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( reload() ) );
	connect( cModel, SIGNAL( layoutChanged() ), this, SLOT( reload() ) );
	connect( this, SIGNAL( customContextMenuRequested( QPoint ) ), this, SIGNAL( contextMenuRequested( QPoint ) ) );

	/* Zoom In and Out actions */
	QAction *zoomInAct = new QAction( "Zoom In", this );
	zoomInAct->setShortcut( QKeySequence::ZoomIn );
	connect( zoomInAct, SIGNAL( triggered() ), this, SLOT( zoomIn() ) );
	addAction( zoomInAct );

	QAction *zoomOutAct = new QAction( "Zoom Out", this );
	zoomOutAct->setShortcut( QKeySequence::ZoomOut );
	connect( zoomOutAct, SIGNAL( triggered() ), this, SLOT( zoomOut() ) );
	addAction( zoomOutAct );
};

void NBIconView::setModel( QAbstractItemModel *model ) {

	QAbstractItemView::setModel( model );

	hashIsDirty = true;
};

void NBIconView::updateViewMode() {

	currentViewMode = Settings->View.ViewMode;
	if ( Settings->View.ViewMode == "Icons" )
		setIconSize( Settings->View.IconsImageSize );

	else if ( Settings->View.ViewMode == "Tiles" )
		setIconSize( Settings->View.TilesImageSize );

	else
		setIconSize( Settings->View.DetailsImageSize );

	return;
};

QString NBIconView::viewMode() {

	return currentViewMode;
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
		if ( QRect( i.value(), myGridSize ).adjusted( padding / 2, padding / 2, -padding, -padding ).contains( point ) )
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
			return categoryList.value( i.key() );
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

	switch( cModel->modelDataType() ) {
		case NBItemViewModel::FileSystem: {

			setSelectionMode( QAbstractItemView::ExtendedSelection );
			break;
		}

		case NBItemViewModel::Catalogs: {

			setSelectionMode( QAbstractItemView::ExtendedSelection );
			break;
		}

		default: {

			setSelectionMode( QAbstractItemView::SingleSelection );
			break;
		}
	}

	QString viewMode;
	int iconSize;

	/* Change view mode and icon size according to the .desktop file */
	if ( not cModel->isRealLocation() ) {
		QString location = cModel->currentDir().replace( "NB://", "" );
		QSettings sett( "NewBreeze", location );

		viewMode = sett.value( "NewBreeze/ViewMode", Settings->View.ViewMode ).toString();

		if ( viewMode == "Icons" )
			iconSize = sett.value( "NewBreeze/IconsImageSize", Settings->View.IconsImageSize.width() ).toInt();

		else if ( viewMode == "Icons" )
			iconSize = sett.value( "NewBreeze/TilesImageSize", Settings->View.TilesImageSize.width() ).toInt();

		else
			iconSize = sett.value( "NewBreeze/DetailsImageSize", Settings->View.DetailsImageSize.width() ).toInt();

		Settings->General.SortColumn = sett.value( "NewBreeze/SortColumn", 2 ).toInt();
	}

	else {
		QSettings sett( cModel->nodePath( ".directory" ), QSettings::NativeFormat );

		viewMode = sett.value( "NewBreeze/ViewMode", Settings->View.ViewMode ).toString();

		if ( viewMode == "Icons" )
			iconSize = sett.value( "NewBreeze/IconsImageSize", Settings->View.IconsImageSize.width() ).toInt();

		else if ( viewMode == "Icons" )
			iconSize = sett.value( "NewBreeze/TilesImageSize", Settings->View.TilesImageSize.width() ).toInt();

		else
			iconSize = sett.value( "NewBreeze/DetailsImageSize", Settings->View.DetailsImageSize.width() ).toInt();

		Settings->General.SortColumn = sett.value( "NewBreeze/SortColumn", 2 ).toInt();
	}

	emit updateViewMode( viewMode );

	currentViewMode = viewMode;
	setIconSize( iconSize, iconSize );

	hashIsDirty = true;
	persistentVCol = 0;

	mSelectedIndexes.clear();
	hiddenCategories.clear();

	categoryList = cModel->categories();
	calculateRectsIfNecessary();
};

QModelIndex NBIconView::moveCursor( QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers ) {
	/*
		*
		* We still do not handle keyboard modifiers. This is yet to be implemented
		*
	*/

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

QModelIndexList NBIconView::selectedIndexes() {

	QSet<QModelIndex> idxSet;
	idxSet.unite( QSet<QModelIndex>::fromList( mSelectedIndexes ) );
	idxSet.unite( QSet<QModelIndex>::fromList( selectionModel()->selectedIndexes() ) );

	return idxSet.toList();
};

QModelIndexList NBIconView::selection() {

	QSet<QModelIndex> idxSet;
	idxSet.unite( QSet<QModelIndex>::fromList( mSelectedIndexes ) );
	idxSet.unite( QSet<QModelIndex>::fromList( selectionModel()->selectedIndexes() ) );

	return idxSet.toList();
};

bool NBIconView::isIndexVisible( QModelIndex idx ) const {

	if ( Settings->General.Grouping ) {
		/* We consider the space reserved for the category but not the indexes listed under it */
		if ( hiddenCategories.contains( cModel->category( idx ) ) )
			return false;

		/* If the index belongs to a folded category and the index position is greater than itemsPerRow - 1 */
		if ( foldedCategories.contains( cModel->category( idx ) ) and cModel->indexInCategory( idx ) >= itemsPerRow - 1 )
			return false;
	}

	QRect rect = viewportRectForRow( idx.row() );
	if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
		return false;

	else
		return true;
};

void NBIconView::paintEvent( QPaintEvent* event ) {

	QPainter painter( viewport() );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing );

	painter.fillRect( viewport()->rect(), palette().color( QPalette::Base ) );

	/* We need to draw the categories only if the model is categorization enabled */
	if ( cModel->isCategorizationEnabled() ) {
		for ( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {

			QRect rect = categoryRect( catIdx );
			if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
				continue;

			paintCategory( &painter, rect, categoryList.at( catIdx ) );
		}
	}

	/* We paint the icons index-wise. */
	for ( int row = 0; row < cModel->rowCount( rootIndex() ); row++ ) {
		QModelIndex idx = cModel->index( row, 0, rootIndex() );
		if ( not isIndexVisible( idx ) ) {
			if ( cModel->isCategorizationEnabled() ) {
				/* If the category to which this index belongs is hidden, continue to next index */
				if ( hiddenCategories.contains( cModel->category( idx ) ) )
					continue;

				/* If the position of this index is the last in row of a folded category, paint a '+' */
				if ( cModel->indexInCategory( idx ) == itemsPerRow - 1 ) {
					QRect rect = viewportRectForRow( row - 1 );
					rect.translate( myGridSize.width(), 0 );
					if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
						continue;

					paintExpander( &painter, rect, idx );
				}
			}

			continue;
		}

		QRect rect = viewportRectForRow( row );
		if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
			continue;

		NBStyleOptionViewItem option = NBStyleOptionViewItem( viewOptions() );

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
		QFileInfo ftype = cModel->nodeInfo( idx );

		if ( ftype.isSymLink() )
			option.nodeType = NBStyleOptionViewItem::SymLink;

		else if ( isExecutable( ftype.absoluteFilePath().toLocal8Bit().data() ) && ftype.isFile() )
			option.nodeType = NBStyleOptionViewItem::Executable;


		/* Dark text colors will suit here */
		if ( isBrightColor( pltt.color( QPalette::Base ), pltt.color( QPalette::Highlight ) ) ) {
			if ( ftype.isSymLink() )
				pltt.setColor( QPalette::Text, QColor( "darkslateblue" ) );

			else if ( isExecutable( ftype.absoluteFilePath().toLocal8Bit().data() ) && ftype.isFile() )
				pltt.setColor( QPalette::Text, QColor( "darkgreen" ) );

			else if ( ftype.isHidden() )
				pltt.setColor( QPalette::Text, pltt.color( QPalette::Text ).lighter() );

			else if ( !ftype.isReadable() and cModel->isRealLocation() )
				pltt.setColor( QPalette::Text, QColor( "darkred" ) );

			else if ( option.state & QStyle::State_Selected )
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
			if ( ftype.isSymLink() )
				pltt.setColor( QPalette::Text, QColor( "skyblue" ) );

			else if ( isExecutable( ftype.absoluteFilePath().toLocal8Bit().data() ) && ftype.isFile() )
				pltt.setColor( QPalette::Text, QColor( "darkgreen" ) );

			else if ( ftype.isHidden() )
				pltt.setColor( QPalette::Text, pltt.color( QPalette::Text ).darker( 125 ) );

			else if ( !ftype.isReadable() and cModel->isRealLocation() )
				pltt.setColor( QPalette::Text, QColor( "indianred" ) );

			else if ( option.state & QStyle::State_Selected )
				pltt.setColor( QPalette::Text, pltt.color( QPalette::HighlightedText ) );

			else
				pltt.setColor( QPalette::Text, palette().color( QPalette::Text ) );

			/* Bright text will be used for drawing the 'current rect' */
			pltt.setColor( QPalette::BrightText, pltt.color( QPalette::Highlight ).lighter() );

			/* ButtonText will be used to paint the extra details */
			pltt.setColor( QPalette::ButtonText, pltt.color( QPalette::Text ).darker( 135 ) );
		}

		option.palette = pltt;

		NBIconDelegate *dlgt = qobject_cast<NBIconDelegate*>( itemDelegate() );
		if ( currentViewMode == QString( "Icons" ) ) {
			dlgt->paintIcons( &painter, option, idx );
			if ( Settings->View.PaintOverlay and ( option.state & QStyle::State_Selected ) and ( option.state & QStyle::State_MouseOver ) )
				paintIconOverlay( &painter, rect );
		}

		else if ( currentViewMode == QString( "Tiles" ) ) {
			dlgt->paintTiles( &painter, option, idx );
			/* Yet to be implemented */
			// if ( Settings->View.PaintOverlay and ( option.state & QStyle::State_Selected ) and ( option.state & QStyle::State_MouseOver ) )
				// paintTilesOverlay( &painter, option.rect, );
		}

		else {
			dlgt->paintDetails( &painter, option, idx );
			/* Yet to be implemented */
			// if ( Settings->View.PaintOverlay and ( option.state & QStyle::State_Selected ) and ( option.state & QStyle::State_MouseOver ) )
				// paintTilesOverlay( &painter, option.rect, );
		}
	}

	painter.end();
	event->accept();
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

	QPixmap pix = cModel->pixmapForCategory( text, hiddenCategories.contains( text ) );
	QPoint topLeft = rectangle.topLeft();
	QPoint topRight = rectangle.topRight();

	painter->drawPixmap( topLeft.x() + 4, topLeft.y() + 4, 16, 16, pix );

	/* Category Menu prompt */
	painter->drawPixmap( topRight.x() - 20, topLeft.y() + 4, 16, 16, QIcon( ":/icons/dot.png" ).pixmap( 16 ) );

	/* We draw the '+' if the category is folded or hidden */
	if ( foldedCategories.contains( text ) or hiddenCategories.contains( text ) )
		painter->drawPixmap( topRight.x() - 36, topLeft.y() + 4, 16, 16, QIcon( ":/icons/category-expand.png" ).pixmap( 16 ) );

	/* We have to draw the '-' only when the number of rows in a category is more than itemsPerRow */
	else if ( cModel->indexListCountForCategory( text ) > itemsPerRow )
		painter->drawPixmap( topRight.x() - 36, topLeft.y() + 4, 16, 16, QIcon( ":/icons/category-collapse.png" ).pixmap( 16 ) );

	QFont categoryFont = qApp->font();
	categoryFont.setBold( true );
	painter->setFont( categoryFont );
	painter->drawText( topLeft.x() + 24, topLeft.y(), rectangle.width() - 48, rectangle.height(), Qt::AlignVCenter, text );

	painter->restore();
};

void NBIconView::paintExpander( QPainter *painter, const QRect &rect, const QModelIndex &idx ) {

	painter->save();
	int n = cModel->indexListCountForCategory( cModel->category( idx ) ) - itemsPerRow + 1;

	int padding = ( int ) round( myIconSize.width() * 0.1 );

	QPixmap pix( ":/icons/category-more.png" );
	pix = pix.scaledToHeight( myGridSize.height() * 0.75, Qt::SmoothTransformation );

	painter->setPen( Qt::lightGray );
	painter->drawRoundedRect( rect.adjusted( padding / 2, padding / 2, -padding / 2, -padding / 2 ), 5, 5 );
	painter->drawLine( rect.topRight() - QPoint( pix.width() * 2, -padding ), rect.bottomRight() - QPoint( pix.width() * 2, padding ) );

	QRect rpix( rect.topRight() - QPoint( pix.width() * 1.5, ( pix.height() - myGridSize.height() ) / 2 ), pix.size() );
	painter->setPen( Qt::NoPen );
	painter->drawPixmap( rpix, pix );

	QRect rtext( rect.topLeft(), rect.bottomRight() - QPoint( pix.width() * 2, 0 ) );
	painter->setPen( Qt::black );
	painter->drawText( rtext, Qt::AlignCenter | Qt::TextWordWrap, QString( "%1 more item%2" ).arg( n ).arg( n > 1 ? "s" : "" ) );

	painter->restore();
};

void NBIconView::paintIconOverlay( QPainter *painter, const QRect &rect ) {

	painter->save();
	int iSize = myIconSize.width();
	if ( iSize < 48 )
		return;

	int padding = ( int ) round( iSize * 0.1 );
	QRectF oRect( rect );
	oRect.adjust( padding / 2, padding / 2, -padding / 2, -padding / 2 );
	oRect.setHeight( iSize + padding );

	QPainterPath path;
	path.setFillRule( Qt::WindingFill );
	path.addRoundedRect( oRect, 3, 3 );
	path.addRect( QRectF( QPoint( oRect.x(), oRect.y() + 10 ), QPoint( oRect.x() + oRect.width(), oRect.y() + iSize + padding ) ) );

	painter->setPen( Qt::NoPen );
	painter->setBrush( QColor( 60, 60, 60, 200 ) );
	painter->drawPath( path );

	painter->setRenderHints( QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing, false );
	painter->setPen( QPen( QColor( 200, 200, 200, 90 ), 0.5 ) );
	painter->drawLine( oRect.x() + oRect.width() / 2, oRect.y(), oRect.x() + oRect.width() / 2, oRect.y() + oRect.height() );
	painter->drawLine( oRect.x(), oRect.y() + oRect.height() / 2, oRect.x() + oRect.width(), oRect.y() + oRect.height() / 2 );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing, true );

	painter->setPen( Qt::white );
	painter->setFont( QFont( "DejaVu Sans", 8, QFont::Bold ) );
	painter->drawText( QRectF( oRect.x(), oRect.y(), oRect.width() / 2, oRect.height() / 2 ), Qt::AlignCenter, "Preview" );
	painter->drawText( QRectF( oRect.x(), oRect.y() + oRect.height() / 2, oRect.width() / 2, oRect.height() / 2 ), Qt::AlignCenter, "Open" );
	painter->drawText( QRectF( oRect.x() + oRect.width() / 2, oRect.y(), oRect.width() / 2, oRect.height() / 2 ), Qt::AlignCenter, "Menu" );
	painter->drawText( QRectF( oRect.x() + oRect.width() / 2, oRect.y() + oRect.height() / 2, oRect.width() / 2, oRect.height() / 2 ), Qt::AlignCenter, "Actions" );

	painter->restore();
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
	path.setFillRule( Qt::WindingFill );
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

	if ( mpEvent->button() != Qt::LeftButton ) {
		QAbstractItemView::mousePressEvent( mpEvent );
		return;
	}

	toolTipTimer.start( 100, this );

	/* Index at mouse position */
	QModelIndex idx = indexAt( mpEvent->pos() );
	int catIdx = expanderAt( mpEvent->pos() );

	/* NoModifier/AltModifier pressed with the mouse */
	if ( qApp->keyboardModifiers().testFlag( Qt::NoModifier ) or qApp->keyboardModifiers().testFlag( Qt::AltModifier ) ) {

		/* Valid index */
		if ( idx.isValid() ) {
			/* Index already selected, start the drag */
			if ( mSelectedIndexes.contains( idx ) or selectionModel()->isSelected( idx ) ) {
				int iSize = myIconSize.width();
				if ( iSize < 48 )
					return;

				QRect rect = viewportRectForRow( idx.row() );
				int padding = ( int ) round( iSize * 0.1 );

				QRectF oRect( rect );
				oRect.adjust( padding / 2, padding / 2, -padding / 2, -padding / 2 );
				oRect.setHeight( iSize + padding );

				QRectF peekRect( oRect.x(), oRect.y(), oRect.width() / 2, oRect.height() / 2 );
				QRectF openRect( oRect.x(), oRect.y() + oRect.height() / 2, oRect.width() / 2, oRect.height() / 2 );
				QRectF menuRect( oRect.x() + oRect.width() / 2, oRect.y(), oRect.width() / 2, oRect.height() / 2 );
				QRectF actsRect( oRect.x() + oRect.width() / 2, oRect.y() + oRect.height() / 2, oRect.width() / 2, oRect.height() / 2 );

				if ( Settings->View.PaintOverlay and peekRect.contains( mpEvent->pos() ) )
					emit peek( idx );

				else if ( Settings->View.PaintOverlay and openRect.contains( mpEvent->pos() ) )
					emit open( idx );

				else if ( Settings->View.PaintOverlay and menuRect.contains( mpEvent->pos() ) )
					emit contextMenuRequested( mpEvent->pos() );

				else if ( Settings->View.PaintOverlay and actsRect.contains( mpEvent->pos() ) )
					emit actionsMenuRequested( mpEvent->pos() );

				else
					dragStartPosition = mpEvent->pos();

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
				QModelIndexList list = cModel->indexListForCategory( catName );
				NBCategoryMenu *catMenu = new NBCategoryMenu( catName, cModel->pixmapForCategory( catName ), cModel->currentDir(), list, this );
				connect( catMenu, SIGNAL( showCategory( QString ) ), this, SLOT( showCategory( QString ) ) );
				connect( catMenu, SIGNAL( foldCategory( QString ) ), this, SLOT( foldCategory( QString ) ) );
				connect( catMenu, SIGNAL( hideCategory( QString ) ), this, SLOT( hideCategory( QString ) ) );
				connect( catMenu, SIGNAL( selection( QString, bool ) ), this, SLOT( setCategorySelected( QString, bool ) ) );

				catMenu->exec( mapToGlobal( mpEvent->pos() ) );
			}

			else if ( QRect( topRight.x() - 36, topLeft.y() + 4, 16, 16 ).contains( mpEvent->pos() ) ) {
				/* If the category is hidden, show it */
				if ( hiddenCategories.contains( catName ) )
					showHideCategory( catName );

				/* If the viewMode is not Details, fold it */
				else if ( currentViewMode != "Details" )
					toggleFoldCategory( catName );

				/* If the viewMode is Details, hide it */
				else
					showHideCategory( catName );
			}

			/* Repaint the viewport */
			viewport()->repaint();
		}

		/* We clicked on the Category expander */
		else if ( catIdx > -1 ) {
			toggleFoldCategory( categoryList.at( catIdx ) );
			return;
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
					mSelectedIndexes << cModel->index( i, 0, rootIndex() );
			}

			/* Reverse selection */
			else {
				mSelectedIndexes.clear();
				for( int i = idx.row(); i <= mSelStartIdx.row(); i++ )
					mSelectedIndexes << cModel->index( i, 0, rootIndex() );
			}

			/* Repaint the viewport */
			viewport()->repaint();
		}

		/* Valid category */
		else if ( categoryAt( mpEvent->pos() ).count() ) {
			setCategorySelected( categoryAt( mpEvent->pos() ), true );
			viewport()->repaint();
		}

		/* Click on empty space */
		else {
		}
	}

	/* Control Modifier */
	else if ( qApp->keyboardModifiers() & Qt::ControlModifier ) {
		/* Valid index */
		if ( idx.isValid() ) {
			/* Make this the current index, without selecting it */
			selectionModel()->setCurrentIndex( idx, QItemSelectionModel::NoUpdate );

			if ( selectionModel()->isSelected( idx ) )
				selectionModel()->select( idx, QItemSelectionModel::Deselect );

			else if ( mSelectedIndexes.contains( idx ) )
				mSelectedIndexes.removeAll( idx );

			else
				mSelectedIndexes << idx;

			viewport()->repaint();
		}

		/* Valid category */
		else if ( categoryAt( mpEvent->pos() ).count() ) {
			setCategorySelected( categoryAt( mpEvent->pos() ), false );
			viewport()->repaint();
		}

		/* Click on empty space */
		else {
		}
	}

	/* Set the clicked index as the current index */
	else {
		if( idx.isValid() ) {
			setCurrentIndex( idx );
		}
	}

	mpEvent->accept();
};

void NBIconView::mouseMoveEvent( QMouseEvent *mmEvent ) {

	QToolTip::hideText();

	/* Left Mouse Button is pressed down */
	if ( mmEvent->buttons() & Qt::LeftButton ) {
		/* Check for drag action: if the index at @dragStartPosition is selected, then its a drag */
		/* If there is no index at the dragStart position, then we start the selection */

		/* Index at @dragStartPosition */
		QModelIndex dIdx;
		if ( dragStartPosition.isNull() )
			dIdx = QModelIndex();

		else
			dIdx = indexAt( dragStartPosition );

		/* Start and execute the drag */
		if ( dIdx.isValid() and mSelectedIndexes.contains( dIdx ) ) {
			QDrag *drag = new QDrag( this );

			QList<QUrl> urlList;

			/* Selection based on mSelectedIndexes */
			Q_FOREACH( QModelIndex idx, mSelectedIndexes ) {
				if ( not idx.column() )
					urlList << QUrl::fromLocalFile( cModel->nodePath( idx ) );
			}

			/* Selection based on selectedModel */
			Q_FOREACH( QModelIndex idx, selectionModel()->selectedRows() ) {
				QUrl url = QUrl::fromLocalFile( cModel->nodePath( idx ) );
				if ( not urlList.contains( url ) )
					urlList << url;
			}

			QMimeData *mimedata = new QMimeData();
			mimedata->setUrls( urlList );

			drag->setMimeData( mimedata );

			/* Set the view state to drag */
			setState( QAbstractItemView::DraggingState );
			drag->exec( Qt::CopyAction | Qt::MoveAction | Qt::LinkAction );
			setState( QAbstractItemView::NoState );

			viewport()->repaint();
			return;
		}

		/* Start the selection, clear it if necessary */
		else {
			if ( indexAt( mmEvent->pos() ).isValid() ) {
				QModelIndex idx = indexAt( mmEvent->pos() );
				if ( not mSelectedIndexes.contains( idx ) )
						mSelectedIndexes << idx;
			}

			viewport()->repaint();
		}

		mmEvent->accept();
		return;
	}

	else {
		if ( expanderAt( mmEvent->pos() ) > -1 )
			setCursor( Qt::PointingHandCursor );

		else
			setCursor( Qt::ArrowCursor );

		QString ctg = categoryAt( mmEvent->pos() );
		if ( ctg.count() ) {
			QRect rct = categoryRect( categoryList.indexOf( ctg ) );
			QPoint topLeft = rct.topLeft();
			QPoint topRight = rct.topRight();

			/* +/- Buttons */
			if ( QRect( topRight.x() - 36, topLeft.y() + 4, 16, 16 ).contains( mmEvent->pos() ) ) {
				if ( cModel->indexListCountForCategory( ctg ) > itemsPerRow )
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

void NBIconView::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	mrEvent->accept();
};

void NBIconView::mouseDoubleClickEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		QModelIndex idx = indexAt( mEvent->pos() );
		if ( idx.isValid() ) {
			switch( cModel->modelDataType() ) {
				case NBItemViewModel::SuperStart: {

					/* If we are showing a drive file (Computer) */
					if ( not idx.data( Qt::UserRole + 6 ).toString().count() )
						emit open( idx.data( Qt::UserRole + 2 ).toString() );

					/* If we are showing a place */
					else
						emit open( idx );

					break;
				}

				case NBItemViewModel::Applications: {

					QStringList execList = idx.data( Qt::UserRole + 3 ).toStringList();
					bool runInTerminal = idx.data( Qt::UserRole + 7 ).toBool();

					if ( not runInTerminal ) {

						// Try to run this program
						QProcess::startDetached( execList.at( 0 ) );
					}

					else {
						QStringList terminalList = getTerminal();
						if ( terminalList.at( 0 ) == "Inbuilt" ) {
							NBTerminal *term = new NBTerminal( NBXdg::home(), execList.at( 0 ) );
							term->showMaximized();
						}

						else {
							terminalList = terminalList.join( " " ).arg( QDir::homePath() ).arg( execList.at( 0 ) ).split( " " );
							QProcess::startDetached( terminalList.takeFirst(), terminalList );
						}
					}

					break;
				}

				case NBItemViewModel::Catalogs: {

					emit open( idx.data( Qt::UserRole + 7 ).toString() );
					break;
				}

				case NBItemViewModel::FileSystem: {

					emit open( idx );
					break;
				}
			}
		}

		else if ( not categoryAt( mEvent->pos() ).isEmpty() ) {
			showHideCategory( categoryAt( mEvent->pos() ) );
		}
	}

	mEvent->accept();
};

void NBIconView::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBIconView::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	/* Default is ignore event */
	dmEvent->ignore();

	const QMimeData *mData = dmEvent->mimeData();
	if ( not mData->hasUrls() )
		return;

	QString source = dirName( mData->urls().at( 0 ).toLocalFile() );
	source += ( source.endsWith( "/" ) ? "" : "/" );

	/* Incoming drop from else where */
	if ( source != cModel->currentDir() ) {
		switch ( cModel->modelDataType() ) {
			case NBItemViewModel::SuperStart: {
				QModelIndex idx = indexAt( dmEvent->pos() );
				if ( idx.isValid() ) {
					if ( not isWritable( cModel->nodePath( idx ) ) or not isDir( cModel->nodePath( idx ) ) )
						return;

					dmEvent->setDropAction( Qt::CopyAction );
					dmEvent->accept();
				}

				break;
			}

			case NBItemViewModel::FileSystem: {
				if ( isWritable( cModel->currentDir() ) ) {
					dmEvent->setDropAction( Qt::CopyAction );
					dmEvent->accept();
				}

				break;
			}

			default : {
				dmEvent->ignore();
				break;
			}
		}
	}

	/* drag and drop in the current folder */
	else if ( indexAt( dmEvent->pos() ).isValid() ) {
		QModelIndex idx = indexAt( dmEvent->pos() );
		QString mtpt = cModel->nodePath( idx );

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

	else {

		dmEvent->ignore();
	}
};

void NBIconView::dropEvent( QDropEvent *dpEvent ) {

	QModelIndex idx = indexAt( dpEvent->pos() );
	QString mtpt;
	if ( not idx.isValid() )
		mtpt = cModel->currentDir();

	else
		mtpt = cModel->nodePath( idx );

	if ( not isDir( mtpt ) ) {
		dpEvent->ignore();
		return;
	}

	else if ( isDir( mtpt ) and not isWritable( mtpt ) ) {
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

			QMenu *menu = new QMenu( this );

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

			emit move( args, mtpt );
		}

		else if ( dpEvent->keyboardModifiers() == Qt::ControlModifier ) {

			emit copy( args, mtpt );
		}

		else if ( dpEvent->keyboardModifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) ) {

			emit link( args, mtpt );
		}

		else {

			dpEvent->ignore();
			return;
		}
	}

	dpEvent->accept();
};

void NBIconView::keyPressEvent( QKeyEvent *kEvent ) {

	QList<int> retKeys = QList<int>() << Qt::Key_Return << Qt::Key_Enter;

	if ( retKeys.contains( kEvent->key() ) and ( selectionModel()->isSelected( currentIndex() ) ) ) {

		QModelIndex idx = currentIndex();
		switch( cModel->modelDataType() ) {

			case NBItemViewModel::SuperStart: {
				if ( not idx.data( Qt::UserRole + 6 ).toString().count() )
					emit open( idx.data( Qt::UserRole + 2 ).toString() );

				else
					emit open( idx );

				break;
			}

			case NBItemViewModel::Applications: {

				QStringList execList = idx.data( Qt::UserRole + 3 ).toStringList();
				bool runInTerminal = idx.data( Qt::UserRole + 7 ).toBool();

				if ( not runInTerminal ) {

					// Try to run this program
					QProcess::startDetached( execList.at( 0 ) );
				}

				else {
					QStringList terminalList = getTerminal();
					if ( terminalList.at( 0 ) == "Inbuilt" ) {
						NBTerminal *term = new NBTerminal( NBXdg::home(), execList.at( 0 ) );
						term->showMaximized();
					}

					else {
						terminalList = terminalList.join( " " ).arg( QDir::homePath() ).arg( execList.at( 0 ) ).split( " " );
						QProcess::startDetached( terminalList.takeFirst(), terminalList );
					}
				}

				break;
			}

			case NBItemViewModel::Catalogs: {

				emit open( idx.data( Qt::UserRole + 7 ).toString() );
				break;
			}

			case NBItemViewModel::FileSystem: {

				emit open( idx );
				break;
			}
		}
	}

	QAbstractItemView::keyPressEvent( kEvent );
};

void NBIconView::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == toolTipTimer.timerId() ) {
		toolTipTimer.stop();
		if ( qApp->mouseButtons() & Qt::LeftButton )
			QToolTip::showText( QCursor::pos(), "Move the mouse over the files/folders you want to select, holding down the left button.", this );
	}

	QAbstractItemView::timerEvent( tEvent );
}

void NBIconView::computeGridSize( QSize iconSize ) {

	if ( currentViewMode == "Icons" ) {
		/*
			* width: 3 * iconSize
			* height: iconSize + iconSize * 2
		*/
		myGridSizeMin = QSize( qMin( 256, qMax( 144, iconSize.width() * 3 ) ), qMax( iconSize.height() + 21, iconSize.height() * 2 ) );
	}

	else if ( currentViewMode == QString( "Tiles" ) ) {
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
				if ( currentViewMode == "Details" ) {
					if ( idx.row() == cModel->rowCount() - 1 ) {
						emit selectionChanged();
						return cModel->index( 0, 0, idx.parent() );
					}

					else {
						emit selectionChanged();
						return cModel->index( idx.row() + 1, 0, idx.parent() );
					}
				}

				emit selectionChanged();
				return belowIndex();
			}

			case QAbstractItemView::MoveUp: {

				if ( currentViewMode == "Details" ) {
					if ( idx.row() == 0 ) {
						emit selectionChanged();
						return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
					}
					else {
						emit selectionChanged();
						return cModel->index( idx.row() - 1, 0, idx.parent() );
					}
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

void NBIconView::setCategorySelected( QString category, bool yes ) {

	if ( yes ) {
		Q_FOREACH( QModelIndex idx, cModel->indexListForCategory( category ) ) {
			if ( not mSelectedIndexes.contains( idx ) )
				mSelectedIndexes << idx;
		}
	}

	else {
		Q_FOREACH( QModelIndex idx, cModel->indexListForCategory( category ) )
			mSelectedIndexes.removeAll( idx );
	}

	viewport()->repaint();
};

QModelIndex NBIconView::nextIndex() {

	/* The next index is in the current category */
	int nextIdx = curIdx.second + 1;
	if ( nextIdx < cModel->indexListCountForCategory( curIdx.first ) ) {
		QString curCat = categoryList.at( curIdx.first );

		/* If the current category is not folded, just return the next index */
		if ( not foldedCategories.contains( curCat ) ) {
			curIdx = qMakePair( curIdx.first, nextIdx );
			persistentVCol = nextIdx % itemsPerRow;
			return cModel->indexListForCategory( curIdx.first )[ nextIdx ];
		}

		/* If the current category is folded, and the next index in in the first row of the current category */
		else if ( foldedCategories.contains( curCat ) and nextIdx < ( itemsPerRow - 1 ) ) {
			curIdx = qMakePair( curIdx.first, nextIdx );
			persistentVCol = nextIdx % itemsPerRow;
			return cModel->indexListForCategory( curIdx.first )[ nextIdx ];
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
	return cModel->indexListForCategory( nCatIdx ).first();

	return QModelIndex();
};

QModelIndex NBIconView::prevIndex() {

	/* The previous index is in the current category */
	int prevIdx = curIdx.second - 1;
	if ( prevIdx >= 0 ) {
		curIdx = qMakePair( curIdx.first, prevIdx );
		persistentVCol = prevIdx % itemsPerRow;
		return cModel->indexListForCategory( curIdx.first )[ prevIdx ];
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
	if ( foldedCategories.contains( prevCat ) ) {
		curIdx = qMakePair( pCatIdx, itemsPerRow - 2 );
		persistentVCol = itemsPerRow - 2;
		return cModel->indexListForCategory( pCatIdx )[ itemsPerRow - 2 ];
	}

	/* The category is not folded */
	else {
		QModelIndexList catIdxes = cModel->indexListForCategory( pCatIdx );
		curIdx = qMakePair( pCatIdx, catIdxes.count() - 1 );
		persistentVCol = ( catIdxes.count() - 1 ) % itemsPerRow;
		return catIdxes.last();
	}

	return QModelIndex();
};

QModelIndex NBIconView::belowIndex() {

	QModelIndex idx = currentIndex();

	/* If the current category is not folded, just return the next index */
	if ( not foldedCategories.contains( cModel->category( idx ) ) ) {
		/* The next index is in the current category */
		int nextIdx = curIdx.second + itemsPerRow;
		if ( nextIdx <= cModel->indexListCountForCategory( curIdx.first ) - 1 ) {

			curIdx = qMakePair( curIdx.first, nextIdx );
			return cModel->indexListForCategory( curIdx.first )[ nextIdx ];
		}

		/* If the number of rows is greater than the current row of the current index */
		else {
			int curRow = ceil( ( 1.0 + curIdx.second ) / itemsPerRow );
			int catRows = ceil( 1.0 * cModel->indexListCountForCategory( curIdx.first ) / itemsPerRow );
			if ( catRows > curRow and curRow != 0 ) {
				curIdx = qMakePair( curIdx.first, cModel->indexListCountForCategory( curIdx.first ) - 1 );
				return cModel->indexListForCategory( curIdx.first ).last();
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
	if ( foldedCategories.contains( categoryList.at( nCatIdx ) ) ) {
		if ( persistentVCol <= itemsPerRow - 2 ) {
			curIdx = qMakePair( nCatIdx, persistentVCol );
			return cModel->indexListForCategory( nCatIdx ).at( persistentVCol );
		}

		else {
			curIdx = qMakePair( nCatIdx, itemsPerRow - 2 );
			return cModel->indexListForCategory( nCatIdx ).at( itemsPerRow - 2 );
		}
	}

	if ( persistentVCol < cModel->indexListCountForCategory( nCatIdx ) - 1 ) {
		curIdx = qMakePair( nCatIdx, persistentVCol );
		return cModel->indexListForCategory( nCatIdx ).at( persistentVCol );
	}

	else {
		curIdx = qMakePair( nCatIdx, cModel->indexListCountForCategory( nCatIdx ) - 1 );
		return cModel->indexListForCategory( nCatIdx ).last();
	}

	return QModelIndex();
};

QModelIndex NBIconView::aboveIndex() {

	/* The previous index is in the current category */
	/* Note that if this category is folded, the next block will have already put the
	 * current index in the first row of this category. */
	int curRow = ceil( curIdx.second / itemsPerRow );
	if ( curRow > 0 ) {
		int nIdx = ( curRow - 1 ) * itemsPerRow + persistentVCol;
		curIdx = qMakePair( curIdx.first, nIdx );
		return cModel->indexListForCategory( curIdx.first ).at( nIdx );
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
	if ( foldedCategories.contains( prevCat ) ) {
		if ( persistentVCol <= cModel->indexListCountForCategory( prevCat ) - 1 ) {
			if ( persistentVCol <= itemsPerRow - 2 ) {
				curIdx = qMakePair( pCatIdx, persistentVCol );
				return cModel->indexListForCategory( prevCat ).at( persistentVCol );
			}

			else {
				curIdx = qMakePair( pCatIdx, itemsPerRow - 2 );
				return cModel->indexListForCategory( prevCat ).at( itemsPerRow - 2 );
			}
		}

		/* Safegaurd: This should never happen */
		else {
			curIdx = qMakePair( pCatIdx, cModel->indexListCountForCategory( pCatIdx ) - 1 );
			return cModel->indexListForCategory( pCatIdx ).last();
		}
	}

	/* Last index of the previous category is the above index */
	int lIdxCol = cModel->indexListCountForCategory( prevCat ) % itemsPerRow;
	if ( lIdxCol <= persistentVCol and lIdxCol != 0 ) {
		curIdx = qMakePair( pCatIdx, cModel->indexListCountForCategory( pCatIdx ) - 1 );
		return cModel->indexListForCategory( pCatIdx ).last();
	}

	else {
		int nRows = ceil( 1.0 * cModel->indexListCountForCategory( prevCat ) / itemsPerRow );
		int pIdx = ( nRows - 1 ) * itemsPerRow + persistentVCol;
		curIdx = qMakePair( pCatIdx, pIdx );
		return cModel->indexListForCategory( pCatIdx ).at( pIdx );
	}

	return QModelIndex();
};

QModelIndex NBIconView::firstIndex() {

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
		return cModel->indexListForCategory( categoryList.at( firstVisibleCatIdx ) ).first();

	else
		return QModelIndex();
};

QModelIndex NBIconView::lastIndex() {

	int lastVisibleCatIdx = -1;
	for( lastVisibleCatIdx = categoryList.count() - 1; lastVisibleCatIdx >= 0; lastVisibleCatIdx-- ) {
		if ( not hiddenCategories.contains( categoryList.at( lastVisibleCatIdx ) ) )
			break;
	}

	verticalScrollBar()->setValue( verticalScrollBar()->maximum() );

	int categoryNodeCount = cModel->indexListCountForCategory( lastVisibleCatIdx );

	if ( foldedCategories.contains( categoryList.at( lastVisibleCatIdx ) ) ) {
		persistentVCol = ( categoryNodeCount < itemsPerRow ? categoryNodeCount : itemsPerRow - 2 );
		curIdx = qMakePair( lastVisibleCatIdx, persistentVCol );
	}

	else {
		persistentVCol = ( categoryNodeCount - 1 ) % itemsPerRow;
		curIdx = qMakePair( lastVisibleCatIdx, categoryNodeCount - 1 );
	}

	if ( lastVisibleCatIdx >= 0 )
		return cModel->indexListForCategory( categoryList.at( lastVisibleCatIdx ) ).at( curIdx.second );

	return QModelIndex();
};

QModelIndex NBIconView::indexPageBelow() {

	QModelIndex idx = currentIndex();

	QRectF curRect = viewportRectForRow( idx.row() );
	QPointF newPoint = curRect.topLeft() + QPointF( myGridSize.width() / 2, viewport()->height() );

	if ( indexAt( newPoint.toPoint() ).isValid() )
		return indexAt( newPoint.toPoint() );

	else
		return lastIndex();
};

QModelIndex NBIconView::indexPageAbove() {

	QModelIndex idx = currentIndex();

	QRectF curRect = viewportRectForRow( idx.row() );
	QPointF newPoint = curRect.topLeft() - QPointF( -myGridSize.width() / 2, viewport()->height() );

	if ( indexAt( newPoint.toPoint() ).isValid() )
		return indexAt( newPoint.toPoint() );

	else if ( indexAt( QPoint( 0, -myCategoryHeight - myCategorySpacing ) + newPoint.toPoint() ).isValid() )
		return indexAt( newPoint.toPoint() );

	else
		return firstIndex();
};

QModelIndex NBIconView::moveCursorNonCategorized( QAbstractItemView::CursorAction cursorAction ) {

	QModelIndex idx = currentIndex();
	if ( idx.isValid() ) {

		switch( cursorAction ) {
			case QAbstractItemView::MoveRight: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < cModel->rowCount() - 1 ) {
					persistentVCol = ( idx.row() + 1 ) % itemsPerRow;
					emit selectionChanged();
					return cModel->index( idx.row() + 1, 0, rootIndex() );
				}

				/* Current index is the last visible index */
				else {
					persistentVCol = 0;
					emit selectionChanged();
					return cModel->index( 0, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveLeft: {
				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < cModel->rowCount() ) {
					persistentVCol = ( idx.row() - 1 ) % itemsPerRow;
					emit selectionChanged();
					return cModel->index( idx.row() - 1, 0, rootIndex() );
				}

				/* The current index is the first one */
				else {
					persistentVCol = ( cModel->rowCount() - 1 ) % itemsPerRow;
					emit selectionChanged();
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveDown: {
				if ( currentViewMode == "Details" ) {
					if ( idx.row() == cModel->rowCount() - 1 ) {
						emit selectionChanged();
						return cModel->index( 0, 0, idx.parent() );
					}
					else {
						emit selectionChanged();
						return cModel->index( idx.row() + 1, 0, idx.parent() );
					}
				}

				int nextRow = idx.row() + itemsPerRow;
				if ( nextRow >= cModel->rowCount() ) {
					emit selectionChanged();
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
				}

				else {
					emit selectionChanged();
					return cModel->index( nextRow, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveUp: {
				if ( currentViewMode == "Details" ) {
					if ( idx.row() == 0 ) {
						emit selectionChanged();
						return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
					}
					else {
						emit selectionChanged();
						return cModel->index( idx.row() - 1, 0, idx.parent() );
					}
				}

				int prevRow = idx.row() - itemsPerRow;
				if ( prevRow < 0 ) {
					emit selectionChanged();
					return cModel->index( 0, 0, rootIndex() );
				}

				else {
					emit selectionChanged();
					return cModel->index( prevRow, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MoveHome: {

				persistentVCol = 0;
				verticalScrollBar()->setValue( 0 );
				emit selectionChanged();
				return cModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				persistentVCol = ( cModel->rowCount() - 1 ) % itemsPerRow;
				emit selectionChanged();
				return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
			}

			case QAbstractItemView::MoveNext: {
				/* If the current index is not the last visible index */
				if ( idx.row() >= 0 and idx.row() < cModel->rowCount() - 1 ) {
					emit selectionChanged();
					return cModel->index( idx.row() + 1, 0, rootIndex() );
				}

				/* Current index is the last visible index */
				else {
					emit selectionChanged();
					return cModel->index( 0, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MovePrevious: {

				/* The current index is anything but the first one */
				if ( idx.row() > 0 and idx.row() < cModel->rowCount() ) {
					emit selectionChanged();
					return cModel->index( idx.row() - 1, 0, rootIndex() );
				}

				/* The current index is the first one */
				else {
					emit selectionChanged();
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
				}
			}

			case QAbstractItemView::MovePageUp: {
				int viewportHeight = viewport()->height();
				int rowsInVisualArea = viewportHeight / myGridSize.height();
				int itemsInVisualArea = rowsInVisualArea * itemsPerRow;

				int prevIdx = idx.row() - itemsInVisualArea;
				if ( not cModel->index( prevIdx, 0, rootIndex() ).isValid() ) {
					emit selectionChanged();
					return cModel->index( 0, 0, rootIndex() );
				}

				emit selectionChanged();
				return cModel->index( prevIdx, 0, rootIndex() );
			}

			case QAbstractItemView::MovePageDown: {
				int viewportHeight = viewport()->height();
				int rowsInVisualArea = viewportHeight / myGridSize.height();
				int itemsInVisualArea = rowsInVisualArea * itemsPerRow;

				int nextIdx = idx.row() + itemsInVisualArea;
				if ( not cModel->index( nextIdx, 0, rootIndex() ).isValid() ) {
					emit selectionChanged();
					return cModel->index( cModel->rowCount() - 1, 0, rootIndex() );
				}

				emit selectionChanged();
				return cModel->index( nextIdx, 0, rootIndex() );
			}

			default: {
				emit selectionChanged();
				return cModel->index( 0, 0, rootIndex() );
			}
		}
	}

	else {
		switch( cursorAction ) {
			case QAbstractItemView::MoveHome: {

				persistentVCol = 0;
				emit selectionChanged();
				return cModel->index( 0, 0, idx.parent() );
			}

			case QAbstractItemView::MoveEnd: {

				persistentVCol = ( cModel->rowCount() - 1 ) % itemsPerRow;
				emit selectionChanged();
				return cModel->index( cModel->rowCount() - 1, 0, idx.parent() );
			}

			default: {

				emit selectionChanged();
				return cModel->index( 0, 0, rootIndex() );
			}
		}
	}

	emit selectionChanged();
	return idx;
};

void NBIconView::calculateRectsIfNecessary() const {

	if ( not hashIsDirty )
		return;

	if ( cModel->isRealLocation() ) {
		QSettings sett( cModel->currentDir() + ".directory", QSettings::NativeFormat );
		hiddenCategories = sett.value( "NewBreeze/HiddenCategories", QStringList() ).toStringList();
		foldedCategories = sett.value( "NewBreeze/FoldedCategories", QStringList() ).toStringList();
	}

	else {
		QString loc = cModel->currentDir().replace( "NB://", "" );
		if ( loc.endsWith( "/" ) )
			loc.chop( 1 );

		QStringList tokens = loc.split( "/", QString::SkipEmptyParts );

		QSettings sett( "NewBreeze", tokens.takeFirst() );
		hiddenCategories = sett.value( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", QStringList() ).toStringList();
		foldedCategories = sett.value( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", QStringList() ).toStringList();
	}

	computeRowsAndColumns();

	rectForRow.clear();
	rectForCategory.clear();
	expanderRects.clear();

	if ( cModel->isCategorizationEnabled() )
		calculateCategorizedRects();

	else
		calculateNonCategorizedRects();

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

void NBIconView::calculateCategorizedRects() const {

	if ( currentViewMode == QString( "Icons" ) )
		calculateCategorizedIconsRects();

	else if ( currentViewMode == QString( "Details" ) )
		calculateCategorizedDetailsRects();

	else
		calculateCategorizedTilesRects();
};

void NBIconView::calculateCategorizedIconsRects() const {

	int x = 0, y = 0, prevRows = 0, totalRows = 0;

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = cModel->indexListForCategory( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		int minX = myContentsMargins.left();
		int minY = myContentsMargins.top() + catIdx * myCategoryHeight + catIdx * myCategorySpacing + totalRows * myGridSize.height();

		/* Extra Spacing in SuperStart */
		if ( cModel->modelDataType() == NBItemViewModel::SuperStart )
			minY += catIdx * myGridSize.height() / 3;

		int categoryWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
		rectForCategory[ catIdx ] = QRect( minX, minY, categoryWidth, 24 );

		// We consider the space reserved for the category but not the indexes listed under it
		if ( hiddenCategories.contains( cModel->category( mList.value( 0 ) ) ) )
			continue;

		// We consider the space reserved for the category and one row of indexes
		else if ( foldedCategories.contains( cModel->category( mList.value( 0 ) ) ) ) {
			// Mimimum X and Y for indexes
			minX += myInlayMargins.left();
			minY += myCategoryHeight;

			totalRows++;
			int limit = mList.count() >= itemsPerRow ? itemsPerRow - 1 : mList.count();

			for( int lrow = 0; lrow < limit; lrow++ ) {
				int row = lrow / itemsPerRow;
				int col = lrow % itemsPerRow;

				x = minX + col * myGridSize.width();
				y = minY + row * myGridSize.height();

				rectForRow[ mList[ lrow ].row() ] = QPoint( x, y );
			}

			x = minX + ( limit % itemsPerRow ) * myGridSize.width();
			y = minY + ( limit / itemsPerRow ) * myGridSize.height();
			expanderRects[ cModel->categoryIndex( mList.value( 0 ) ) ] = QRect( x, y, myGridSize.width(), myGridSize.height() );
		}

		else {
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
	}

	idealHeight = myContentsMargins.top() + myContentsMargins.bottom() + categoryList.count() * ( myCategoryHeight + myCategorySpacing );
	idealHeight += totalRows * myGridSize.height();

	if ( cModel->modelDataType() == NBItemViewModel::SuperStart )
		idealHeight += categoryList.count() * myGridSize.height();

	hashIsDirty = false;
	viewport()->update();
};

void NBIconView::calculateCategorizedTilesRects() const {

	int x = 0, y = 0, prevRows = 0, totalRows = 0;

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = cModel->indexListForCategory( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		int minX = myContentsMargins.left();
		int minY = myContentsMargins.top() + catIdx * myCategoryHeight + catIdx * myCategorySpacing + totalRows * myGridSize.height();

		/* Extra Spacing in SuperStart */
		if ( cModel->modelDataType() == NBItemViewModel::SuperStart )
			minY += catIdx * myGridSize.height() / 3;

		int categoryWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
		rectForCategory[ catIdx ] = QRect( minX, minY, categoryWidth, 24 );

		// We consider the space reserved for the category but not the indexes listed under it
		if ( hiddenCategories.contains( cModel->category( mList.value( 0 ) ) ) )
			continue;

		else if ( foldedCategories.contains( cModel->category( mList.value( 0 ) ) ) ) {
			// Mimimum X and Y for indexes
			minX += myInlayMargins.left();
			minY += myCategoryHeight;

			totalRows++;
			int limit = mList.count() >= itemsPerRow ? itemsPerRow - 1 : mList.count();

			for( int lrow = 0; lrow < limit; lrow++ ) {
				int row = lrow / itemsPerRow;
				int col = lrow % itemsPerRow;

				x = minX + col * myGridSize.width();
				y = minY + row * myGridSize.height();

				rectForRow[ mList[ lrow ].row() ] = QPoint( x, y );
			}
		}

		else {
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

	int catX = myContentsMargins.left() ;
	int catW = myGridSize.width() + myInlayMargins.left() + myInlayMargins.right();
	int x = myContentsMargins.left() + myInlayMargins.left();

	for( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {
		QModelIndexList mList = cModel->indexListForCategory( categoryList.at( catIdx ) );

		// Minimum X and Y for Category Rectangle
		int minY = myContentsMargins.top() + catIdx * myCategoryHeight + catIdx * myCategorySpacing + totalRows * myGridSize.height();

		/* Extra Spacing in SuperStart */
		if ( cModel->modelDataType() == NBItemViewModel::SuperStart )
			minY += catIdx * myGridSize.height() / 3;

		rectForCategory[ catIdx ] = QRect( catX, minY, catW, myCategoryHeight );

		// We consider the space reserved for the category but not the indexes listed under it
		if ( hiddenCategories.contains( cModel->category( mList.value( 0 ) ) ) )
			continue;

		else {
			// Mimimum X and Y for indexes
			minY += myCategoryHeight;

			prevRows = cModel->indexListCountForCategory( categoryList.at( catIdx ) );
			totalRows += prevRows;

			for( int lrow = 0; lrow < mList.count(); lrow++ ) {
				y = minY + lrow * myGridSize.height();

				rectForRow[ mList[ lrow ].row() ] = QPoint( x, y );
			}
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

	if ( currentViewMode == QString( "Icons" ) )
		calculateNonCategorizedIconsRects();

	else if ( currentViewMode == QString( "Details" ) )
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

	if ( currentViewMode == QString( "Details" ) ) {
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

void NBIconView::showHideCategory( QString category ) {

	if ( cModel->currentDir().startsWith( "NB://" ) ) {
		QString loc = cModel->currentDir().replace( "NB://", "" );
		if ( loc.endsWith( "/" ) )
			loc.chop( 1 );

		QStringList tokens = loc.split( "/", QString::SkipEmptyParts );

		QSettings dirSett( "NewBreeze", tokens.takeFirst() );
		QStringList hidden = dirSett.value( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", QStringList() ).toStringList();
		QStringList folded = dirSett.value( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", QStringList() ).toStringList();

		if ( hiddenCategories.contains( category ) ) {
			hiddenCategories.removeAll( category );
			hidden.removeAll( category );
			folded.removeAll( category );
		}

		else{
			hiddenCategories.append( category );
			hidden.append( category );
		}

		hidden.removeDuplicates();
		dirSett.setValue( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", hidden );
		dirSett.setValue( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", folded );
		dirSett.sync();
	}

	else {
		QSettings dirSett( cModel->currentDir() + ".directory", QSettings::NativeFormat );
		QStringList hidden = dirSett.value( "NewBreeze/HiddenCategories", QStringList() ).toStringList();
		QStringList folded = dirSett.value( "NewBreeze/FoldedCategories", QStringList() ).toStringList();

		if ( hiddenCategories.contains( category ) ) {
			hiddenCategories.removeAll( category );
			hidden.removeAll( category );
			folded.removeAll( category );
		}

		else{
			hiddenCategories.append( category );
			hidden.append( category );
		}

		hidden.removeDuplicates();
		dirSett.setValue( "NewBreeze/HiddenCategories", hidden );
		dirSett.setValue( "NewBreeze/FoldedCategories", folded );
		dirSett.sync();
	}

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::toggleFoldCategory( QString category ) {

	if ( not cModel->isRealLocation() ) {
		QString loc = cModel->currentDir().replace( "NB://", "" );

		if ( loc.endsWith( "/" ) )
			loc.chop( 1 );

		QStringList tokens = loc.split( "/", QString::SkipEmptyParts );

		QSettings dirSett( "NewBreeze", tokens.takeFirst() );
		QStringList hidden = dirSett.value( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", QStringList() ).toStringList();
		QStringList folded = dirSett.value( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", QStringList() ).toStringList();

		/* If @category is hidden, show it folded */
		if ( hiddenCategories.contains( category ) ) {
			hidden.removeAll( category );
			hiddenCategories.removeAll( category );
			if ( cModel->indexListCountForCategory( category ) > itemsPerRow ) {
				foldedCategories.append( category );
				folded.append( category );
			}
		}

		else if ( foldedCategories.contains( category ) ) {
			hiddenCategories.removeAll( category );
			hidden.removeAll( category );

			foldedCategories.removeAll( category );
			folded.removeAll( category );
		}

		else{
			if ( cModel->indexListCountForCategory( category ) > itemsPerRow ) {
				foldedCategories.append( category );
				folded.append( category );
			}
		}

		folded.removeDuplicates();
		dirSett.setValue( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", hidden );
		dirSett.setValue( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", folded );
		dirSett.sync();
	}

	else {
		QSettings dirSett( cModel->currentDir() + ".directory", QSettings::NativeFormat );
		QStringList hidden = dirSett.value( "NewBreeze/HiddenCategories", QStringList() ).toStringList();
		QStringList folded = dirSett.value( "NewBreeze/FoldedCategories", QStringList() ).toStringList();

		/* If @category is hidden, show it folded */
		if ( hiddenCategories.contains( category ) ) {
			hidden.removeAll( category );
			hiddenCategories.removeAll( category );
			if ( cModel->indexListCountForCategory( category ) > itemsPerRow ) {
				foldedCategories.append( category );
				folded.append( category );
			}
		}

		else if ( foldedCategories.contains( category ) ) {
			hiddenCategories.removeAll( category );
			hidden.removeAll( category );

			foldedCategories.removeAll( category );
			folded.removeAll( category );
		}

		else{
			if ( cModel->indexListCountForCategory( category ) > itemsPerRow ) {
				foldedCategories.append( category );
				folded.append( category );
			}
		}

		folded.removeDuplicates();
		dirSett.setValue( "NewBreeze/HiddenCategories", hidden );
		dirSett.setValue( "NewBreeze/FoldedCategories", folded );
		dirSett.sync();
	}

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::showCategory( QString category ) {

	if ( not cModel->isRealLocation() ) {
		QString loc = cModel->currentDir().replace( "NB://", "" );
		if ( loc.endsWith( "/" ) )
			loc.chop( 1 );

		QStringList tokens = loc.split( "/", QString::SkipEmptyParts );

		QSettings dirSett( "NewBreeze", tokens.takeFirst() );
		QStringList hidden = dirSett.value( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", QStringList() ).toStringList();
		QStringList folded = dirSett.value( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", QStringList() ).toStringList();

		hiddenCategories.removeAll( category );
		foldedCategories.removeAll( category );
		hidden.removeAll( category );
		folded.removeAll( category );

		dirSett.setValue( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", hidden );
		dirSett.setValue( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", folded );
		dirSett.sync();
	}

	else {
		QSettings dirSett( cModel->currentDir() + ".directory", QSettings::NativeFormat );
		QStringList hidden = dirSett.value( "NewBreeze/HiddenCategories", QStringList() ).toStringList();
		QStringList folded = dirSett.value( "NewBreeze/FoldedCategories", QStringList() ).toStringList();

		hiddenCategories.removeAll( category );
		foldedCategories.removeAll( category );
		hidden.removeAll( category );
		folded.removeAll( category );

		dirSett.setValue( "NewBreeze/HiddenCategories", hidden );
		dirSett.setValue( "NewBreeze/FoldedCategories", folded );
		dirSett.sync();
	}

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::foldCategory( QString category ) {

	if ( cModel->indexListCountForCategory( category ) < itemsPerRow )
		return;

	if ( not cModel->isRealLocation() ) {
		QString loc = cModel->currentDir().replace( "NB://", "" );
		if ( loc.endsWith( "/" ) )
			loc.chop( 1 );

		QStringList tokens = loc.split( "/", QString::SkipEmptyParts );

		QSettings dirSett( "NewBreeze", tokens.takeFirst() );
		QStringList hidden = dirSett.value( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", QStringList() ).toStringList();
		QStringList folded = dirSett.value( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", QStringList() ).toStringList();

		hiddenCategories.removeAll( category );
		foldedCategories.removeAll( category );
		hidden.removeAll( category );
		folded.removeAll( category );

		foldedCategories.append( category );
		folded.append( category );

		dirSett.setValue( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", hidden );
		dirSett.setValue( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", folded );
		dirSett.sync();
	}

	else {
		QSettings dirSett( cModel->currentDir() + ".directory", QSettings::NativeFormat );
		QStringList hidden = dirSett.value( "NewBreeze/HiddenCategories", QStringList() ).toStringList();
		QStringList folded = dirSett.value( "NewBreeze/FoldedCategories", QStringList() ).toStringList();

		hiddenCategories.removeAll( category );
		foldedCategories.removeAll( category );
		hidden.removeAll( category );
		folded.removeAll( category );

		foldedCategories.append( category );
		folded.append( category );

		dirSett.setValue( "NewBreeze/HiddenCategories", hidden );
		dirSett.setValue( "NewBreeze/FoldedCategories", folded );
		dirSett.sync();
	}

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::hideCategory( QString category ) {

	if ( not cModel->isRealLocation() ) {
		QString loc = cModel->currentDir().replace( "NB://", "" );
		if ( loc.endsWith( "/" ) )
			loc.chop( 1 );

		QStringList tokens = loc.split( "/", QString::SkipEmptyParts );

		QSettings dirSett( "NewBreeze", tokens.takeFirst() );
		QStringList hidden = dirSett.value( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", QStringList() ).toStringList();
		QStringList folded = dirSett.value( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", QStringList() ).toStringList();

		hiddenCategories.removeAll( category );
		foldedCategories.removeAll( category );
		hidden.removeAll( category );
		folded.removeAll( category );

		hiddenCategories.append( category );
		hidden.append( category );

		dirSett.setValue( "NewBreeze/" + tokens.join( "/" ) + "/HiddenCategories", hidden );
		dirSett.setValue( "NewBreeze/" + tokens.join( "/" ) + "/FoldedCategories", folded );
		dirSett.sync();
	}

	else {
		QSettings dirSett( cModel->currentDir() + ".directory", QSettings::NativeFormat );
		QStringList hidden = dirSett.value( "NewBreeze/HiddenCategories", QStringList() ).toStringList();
		QStringList folded = dirSett.value( "NewBreeze/FoldedCategories", QStringList() ).toStringList();

		hiddenCategories.removeAll( category );
		foldedCategories.removeAll( category );
		hidden.removeAll( category );
		folded.removeAll( category );

		hiddenCategories.append( category );
		hidden.append( category );

		dirSett.setValue( "NewBreeze/HiddenCategories", hidden );
		dirSett.setValue( "NewBreeze/FoldedCategories", folded );
		dirSett.sync();
	}

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::zoomIn() {

	if ( currentViewMode == QString( "Details" ) ) {
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

	if ( cModel->isRealLocation() ) {

		QSettings sett( cModel->nodePath( ".directory" ), QSettings::NativeFormat );
		sett.setValue( "NewBreeze/" + Settings->View.ViewMode + "ImageSize", myIconSize.width() );
		sett.sync();
	}

	else {
		QString loc = cModel->currentDir().replace( "NB://", "" );
		QSettings sett( "NewBreeze", loc );
		sett.setValue( "NewBreeze/" + Settings->View.ViewMode + "ImageSize", myIconSize.width() );
		sett.sync();
	}
};

void NBIconView::zoomOut() {

	if ( myIconSize.width() <= 16 )
		setIconSize( QSize( 16, 16 ) );

	else
		setIconSize( myIconSize - QSize( 4, 4 ) );

	if ( cModel->isRealLocation() ) {

		QSettings sett( cModel->nodePath( ".directory" ), QSettings::NativeFormat );
		sett.setValue( "NewBreeze/" + Settings->View.ViewMode + "ImageSize", myIconSize.width() );
		sett.sync();
	}

	else {
		QString loc = cModel->currentDir().replace( "NB://", "" );
		QSettings sett( "NewBreeze", loc );
		sett.setValue( "NewBreeze/" + Settings->View.ViewMode + "ImageSize", myIconSize.width() );
		sett.sync();
	}
};

void NBIconView::emitCML() {

	QAction *act = qobject_cast<QAction*>( sender() );

	QStringList args = act->data().toStringList();
	QString mtpt = args.takeLast();

	if ( not mtpt.endsWith( "/" ) )
		mtpt += "/";

	if ( act->text().contains( "Copy" ) ) {

		NBProcess::Progress *progress = new NBProcess::Progress;
		progress->sourceDir = dirName( args.at( 0 ) );
		progress->targetDir = mtpt;

		QStringList srcList;
		foreach( QString path, args )
			srcList << path.replace( progress->sourceDir, "" );

		progress->type = NBProcess::Copy;

		NBIOProcess *proc = new NBIOProcess( srcList, progress );
		NBProcessManager::instance()->addProcess( progress, proc );

		progress->startTime = QTime::currentTime();

		proc->start();
	}

	else if ( act->text().contains( "Move" ) ) {

		NBProcess::Progress *progress = new NBProcess::Progress;
		progress->sourceDir = dirName( args.at( 0 ) );
		progress->targetDir = mtpt;

		QStringList srcList;
		foreach( QString path, args )
			srcList << path.replace( progress->sourceDir, "" );

		progress->type = NBProcess::Move;

		NBIOProcess *proc = new NBIOProcess( srcList, progress );
		NBProcessManager::instance()->addProcess( progress, proc );

		progress->startTime = QTime::currentTime();

		proc->start();
	}

	else {

		QStringList errorNodes;
		Q_FOREACH( QString path, args ) {
			if ( symlink( path.toLocal8Bit().data(), ( mtpt + baseName( path ) ).toLocal8Bit().data() ) != 0 ) {
				errorNodes << path;
				qDebug() << "Failed to create link for:" << baseName( path );
				qDebug() << "   " << strerror( errno );
			}
		}

		if ( errorNodes.count() ) {
			QString title = "NewBreeze - Error Creating links";
			QString text = QString(
				"Some errors were encountered while creating the symlinks you requested. "
				"They are listed in the table below. Please check the debug messages to determine the cause of the errors."
			);

			NBErrorsDialog *errDlg = new NBErrorsDialog( title, text, errorNodes, this );
			errDlg->exec();
		}
	}
};

int NBIconView::expanderAt( QPoint pt ) {

	Q_FOREACH( int key, expanderRects.keys() ) {
		if ( expanderRects.value( key ).contains( pt ) )
			return key;
	}

	return -1;
};

void NBIconView::currentChanged( const QModelIndex &cur, const QModelIndex &prev ) {

	QAbstractItemView::currentChanged( cur, prev );

	curIdx = qMakePair( categoryList.indexOf( cModel->category( cur ) ), cModel->indexInCategory( cur ) );
	persistentVCol = cModel->indexInCategory( cur ) % itemsPerRow;
};
