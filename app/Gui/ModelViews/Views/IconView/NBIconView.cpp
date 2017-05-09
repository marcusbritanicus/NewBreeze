/*
	*
	* NBIconView.cpp - NewBreeze IconView Class
	*
*/

#include "NBIconView.hpp"

static inline bool isExecutable( QString path ) {

	struct stat statbuf;
	if ( stat( path.toLocal8Bit().data(), &statbuf ) == 0 )
		return ( statbuf.st_mode & S_IXUSR );

	else
		return false;
};

NBIconView::NBIconView( NBItemViewModel *fsModel, QWidget *parent ) : QAbstractItemView( parent ) {

	/* Current folder viewMode */
	currentViewMode = Settings->General.ViewMode;

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
	setIconSize( Settings->General.IconSize );

	/* Selection */
	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setSelectionBehavior( QAbstractItemView::SelectRows );
	mSelectedIndexes = QModelIndexList();

	/* Internal Object Name */
	setObjectName( "mainList" );

	/* Styling */
	setStyleSheet( "#mainList{ border:none; }" );

	/* Minimum Size */
	setMinimumWidth( 640 );

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

	connect( cModel, SIGNAL( directoryLoading( QString ) ), this, SLOT( reload() ) );
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

	currentViewMode = Settings->General.ViewMode;
	computeGridSize( Settings->General.IconSize );

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

	/* Change view mode according to the .desktop file */
	QSettings sett( cModel->nodePath( ".directory" ), QSettings::NativeFormat );

	/* We have set per-folder settings */
	QString viewMode = sett.value( "NewBreeze/ViewMode", Settings->General.ViewMode ).toString();
	int iconSize = sett.value( "NewBreeze/IconSize", Settings->General.IconSize.width() ).toInt();

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

	// We consider the space reserved for the category but not the indexes listed under it
	if ( hiddenCategories.contains( cModel->category( idx ) ) )
		return false;

	QRect rect = viewportRectForRow( idx.row() );
	if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
		return false;

	else
		return true;
};

void NBIconView::paintEvent( QPaintEvent* event ) {

	QPainter painter( viewport() );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing );

	/* We need to draw the categories only if the model is categorization enabled */
	if ( cModel->isCategorizationEnabled() ) {
		for ( int catIdx = 0; catIdx < categoryList.count(); catIdx++ ) {

			QRect rect = categoryRect( catIdx );
			if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
				continue;

			paintCategory( &painter, rect, categoryList.at( catIdx ) );
		}
	}

	for ( int row = 0; row < cModel->rowCount( rootIndex() ); row++ ) {
		QModelIndex idx = cModel->index( row, 0, rootIndex() );
		if ( not canShowIndex( idx ) ) {
			if ( cModel->indexInCategory( idx ) == itemsPerRow - 1 ) {
				qDebug() << idx.data().toString() << cModel->indexInCategory( idx ) << itemsPerRow;
				qDebug() << "Drawing dummy";
				QRect rect = viewportRectForRow( row );
				if ( !rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height() )
					continue;

				qDebug() << rect;

				painter.save();
				painter.setPen( Qt::black );
				int n = cModel->indexListCountForCategory( cModel->category( idx ) ) - itemsPerRow + 1;
				painter.drawText( rect, Qt::AlignCenter | Qt::TextWordWrap, QString( "%1 more item%2" ).arg( n ).arg( n > 1 ? "s" : "" ) );
				painter.restore();
			}
			continue;
		}

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
		QFileInfo ftype = cModel->nodeInfo( idx );

		/* Dark text colors will suit here */
		if ( isBrightColor( pltt.color( QPalette::Base ), pltt.color( QPalette::Highlight ) ) ) {
			if ( ftype.isSymLink() )
				pltt.setColor( QPalette::Text, QColor( "darkslateblue" ) );

			#warning "Possible Qt bug: Registers all files on encfs mounted path as executable."
			else if ( isExecutable( ftype.absoluteFilePath().toLocal8Bit().data() ) && ftype.isFile() )
				pltt.setColor( QPalette::Text, QColor( "darkgreen" ) );

			else if ( ftype.isHidden() )
				pltt.setColor( QPalette::Text, pltt.color( QPalette::Text ).lighter() );

			else if ( !ftype.isReadable() )
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

			#warning "Possible Qt bug: Registers all files on encfs mounted path as executable."
			else if ( isExecutable( ftype.absoluteFilePath().toLocal8Bit().data() ) && ftype.isFile() )
				pltt.setColor( QPalette::Text, QColor( "darkgreen" ) );

			else if ( ftype.isHidden() )
				pltt.setColor( QPalette::Text, pltt.color( QPalette::Text ).darker( 125 ) );

			else if ( !ftype.isReadable() )
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
		if ( currentViewMode == QString( "Icons" ) )
			dlgt->paintIcons( &painter, option, idx );

		else if ( currentViewMode == QString( "Tiles" ) )
			dlgt->paintTiles( &painter, option, idx );

		else
			dlgt->paintDetails( &painter, option, idx );
	}

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
			QPoint topLeft = rct.topLeft();

			/* Category arrow clicked */
			if ( QRect( topLeft.x() + 4, topLeft.y() + 4, 16, 16 ).contains( mpEvent->pos() ) )
				toggleFoldCategory( categoryAt( mpEvent->pos() ) );

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
		QModelIndex cIdx = currentIndex();

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
			if ( mSelectedIndexes.contains( idx ) )
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

	else {

	}

	/* Set the clicked index as the current index */
	if( idx.isValid() )
		setCurrentIndex( idx );

	mpEvent->accept();
};

void NBIconView::mouseMoveEvent( QMouseEvent *mmEvent ) {

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

	viewport()->repaint();
	QAbstractItemView::mouseMoveEvent( mmEvent );
};

void NBIconView::mouseDoubleClickEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		QModelIndex idx = indexAt( mEvent->pos() );
		if ( idx.isValid() ) {
			switch( cModel->modelDataType() ) {
				case NBItemViewModel::SuperStart: {

					/* If we are showing a desktop file (application) */
					if ( idx.data( Qt::UserRole + 9 ).toString().count() ) {
						QStringList execList = idx.data( Qt::UserRole + 3 ).toStringList();
						bool runInTerminal = idx.data( Qt::UserRole + 7 ).toBool();

						if ( not runInTerminal ) {

							// Try to run this program
							QProcess::startDetached( execList.at( 0 ) );
						}

						else {
							QStringList terminalList = getTerminal().join( " " ).arg( QDir::homePath() ).arg( execList.at( 0 ) ).split( " " );
							QProcess::startDetached( terminalList.takeFirst(), terminalList );
						}
					}

					/* If we are showing a place */
					else {

						emit open( idx );
					}

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
						QStringList terminalList = getTerminal().join( " " ).arg( QDir::homePath() ).arg( execList.at( 0 ) ).split( " " );
						QProcess::startDetached( terminalList.takeFirst(), terminalList );
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
				if ( idx.data( Qt::UserRole + 9 ).toString().count() ) {
					QStringList execList = idx.data( Qt::UserRole + 3 ).toStringList();
					bool runInTerminal = idx.data( Qt::UserRole + 7 ).toBool();

					if ( not runInTerminal ) {

						// Try to run this program
						QProcess::startDetached( execList.at( 0 ) );
					}

					else {
						QStringList terminalList = getTerminal().join( " " ).arg( QDir::homePath() ).arg( execList.at( 0 ) ).split( " " );
						QProcess::startDetached( terminalList.takeFirst(), terminalList );
					}
				}

				else {

					emit open( idx );
				}

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
					QStringList terminalList = getTerminal().join( " " ).arg( QDir::homePath() ).arg( execList.at( 0 ) ).split( " " );
					QProcess::startDetached( terminalList.takeFirst(), terminalList );
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
				emit selectionChanged();
				return nextIndex();

			case QAbstractItemView::MoveRight:
				emit selectionChanged();
				return nextIndex();

			case QAbstractItemView::MovePrevious:
				emit selectionChanged();
				return prevIndex();

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
				emit selectionChanged();
				return firstIndex();

			case QAbstractItemView::MoveRight:
				emit selectionChanged();
				return firstIndex();

			case QAbstractItemView::MoveNext:
				emit selectionChanged();
				return firstIndex();

			case QAbstractItemView::MoveDown:
				emit selectionChanged();
				return firstIndex();

			case QAbstractItemView::MoveEnd:
				emit selectionChanged();
				return lastIndex();

			case QAbstractItemView::MoveLeft:
				emit selectionChanged();
				return lastIndex();

			case QAbstractItemView::MovePrevious:
				emit selectionChanged();
				return lastIndex();

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
};

QModelIndex NBIconView::nextIndex() {

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

QModelIndex NBIconView::prevIndex() {

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

QModelIndex NBIconView::belowIndex() {

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

	/* We have another row, just not an index below this */
	else if ( ( cRow < cRows ) and ( ( cRow * itemsPerRow + persistentVCol ) >= cIdxList.count() - 1 ) ) {

		return cIdxList.last();
	}

	/* There is an index below this one in the next category */
	else if ( nIdxList.count() > persistentVCol ) {

		return nIdxList.at( persistentVCol );
	}

	/* Last one in the next category */
	else {

		return nIdxList.last();
	}
};

QModelIndex NBIconView::aboveIndex() {

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
	if ( cRow > 1 ) {

		return cIdxList.at( ( cIdxList.indexOf( idx ) / itemsPerRow - 1 ) * itemsPerRow + persistentVCol );
	}

	/* We have to return from the previous category */
	else if ( persistentVCol < nLast ) {

		return pIdxList.at( ( pRows - 1 ) * itemsPerRow + persistentVCol );
	}

	/* The last index of the previous category */
	else {

		return pIdxList.last();
	}
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

	verticalScrollBar()->setValue( 0 );

	if ( firstVisibleCatIdx >= 0 )
		return cModel->indexListForCategory( categoryList.at( firstVisibleCatIdx ) ).first();

	else
		return QModelIndex();
};

QModelIndex NBIconView::lastIndex() {

	int lastVisibleCatIdx = -1;
	for( int i = categoryList.count() - 1; i >= 0; i-- ) {
		if ( not hiddenCategories.contains( categoryList.at( i ) ) ) {
			lastVisibleCatIdx = i;
			break;
		}
	}

	verticalScrollBar()->setValue( verticalScrollBar()->maximum() );

	persistentVCol = ( cModel->indexListCountForCategory( categoryList.at( lastVisibleCatIdx ) ) - 1 ) % itemsPerRow;

	if ( lastVisibleCatIdx >= 0 )
		return cModel->indexListForCategory( categoryList.at( lastVisibleCatIdx ) ).last();

	else
		return QModelIndex();
};

QModelIndex NBIconView::indexPageBelow() {

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

QModelIndex NBIconView::indexPageAbove() {

	return QModelIndex();
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

	QSettings sett( cModel->currentDir() + ".directory", QSettings::NativeFormat );
	hiddenCategories = sett.value( "NewBreeze/HiddenCategories", QStringList() ).toStringList();
	foldedCategories = sett.value( "NewBreeze/FoldedCategories", QStringList() ).toStringList();

	computeRowsAndColumns();

	rectForRow.clear();
	rectForCategory.clear();

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

		if ( cModel->modelDataType() == NBItemViewModel::SuperStart )
			minY += ( catIdx ? myGridSize.height() : 0 );

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
			int limit = mList.count() >= itemsPerRow ? itemsPerRow : mList.count();

			for( int lrow = 0; lrow < limit; lrow++ ) {
				int row = lrow / itemsPerRow;
				int col = lrow % itemsPerRow;

				x = minX + col * myGridSize.width();
				y = minY + row * myGridSize.height();

				rectForRow[ mList[ lrow ].row() ] = QPoint( x, y );
			}

			continue;
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

		int categoryWidth = viewport()->width() - myContentsMargins.left() - myContentsMargins.right();
		rectForCategory[ catIdx ] = QRect( minX, minY, categoryWidth, 24 );

		// We consider the space reserved for the category but not the indexes listed under it
		if ( hiddenCategories.contains( cModel->category( mList.value( 0 ) ) ) )
			continue;

		if ( foldedCategories.contains( cModel->category( mList.value( 0 ) ) ) ) {
			// Mimimum X and Y for indexes
			minX += myInlayMargins.left();
			minY += myCategoryHeight;

			totalRows++;

			for( int lrow = 0; lrow < mList.count(); lrow++ ) {
				int row = lrow / itemsPerRow;
				int col = lrow % itemsPerRow;

				x = minX + col * myGridSize.width();
				y = minY + row * myGridSize.height();

				rectForRow[ mList[ lrow ].row() ] = QPoint( x, y );
			}

			continue;
		}

		// Mimimum X and Y for indexes
		minX += myInlayMargins.left();
		minY += myCategoryHeight;

		prevRows = mList.count() / itemsPerRow;
		if ( mList.count() % itemsPerRow )
			prevRows++;

		totalRows += prevRows;
		int limit = mList.count() >= itemsPerRow ? itemsPerRow : mList.count();

		for( int lrow = 0; lrow < limit; lrow++ ) {
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

	QPixmap pix = cModel->pixmapForCategory( text );
	QPoint topLeft = rectangle.topLeft();

	painter->drawPixmap( topLeft.x() + 4, topLeft.y() + 4, 16, 16, pix );
	QFont categoryFont = qApp->font();
	categoryFont.setBold( true );
	painter->setFont( categoryFont );
	painter->drawText( topLeft.x() + 24, topLeft.y(), rectangle.width() - 48, rectangle.height(), Qt::AlignVCenter, text );

	painter->restore();
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
		sett.setValue( "NewBreeze/IconSize", myIconSize.width() );
	}
};

void NBIconView::zoomOut() {

	if ( myIconSize.width() <= 16 )
		setIconSize( QSize( 16, 16 ) );

	else
		setIconSize( myIconSize - QSize( 4, 4 ) );

	if ( cModel->isRealLocation() ) {

		QSettings sett( cModel->nodePath( ".directory" ), QSettings::NativeFormat );
		sett.setValue( "NewBreeze/IconSize", myIconSize.width() );
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

void NBIconView::showHideCategory( QString category ) {

	QSettings dirSett( cModel->currentDir() + ".directory", QSettings::NativeFormat );
	QStringList hidden = dirSett.value( "NewBreeze/HiddenCategories", QStringList() ).toStringList();
	QStringList folded = dirSett.value( "NewBreeze/FoldedCategories", QStringList() ).toStringList();

	if ( hiddenCategories.contains( category ) ) {
		hiddenCategories.removeAll( category );
		hidden.removeAll( category );
		folded.removeAll( category );
		cModel->openCategory( category );
	}

	else{
		hiddenCategories.append( category );
		hidden.append( category );
		cModel->foldCategory( category );
	}

	hidden.removeDuplicates();
	dirSett.setValue( "NewBreeze/HiddenCategories", hidden );
	dirSett.setValue( "NewBreeze/FoldedCategories", folded );
	dirSett.sync();

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

void NBIconView::toggleFoldCategory( QString category ) {

	QSettings dirSett( cModel->currentDir() + ".directory", QSettings::NativeFormat );
	QStringList hidden = dirSett.value( "NewBreeze/HiddenCategories", QStringList() ).toStringList();
	QStringList folded = dirSett.value( "NewBreeze/FoldedCategories", QStringList() ).toStringList();

	if ( foldedCategories.contains( category ) ) {
		foldedCategories.removeAll( category );
		folded.removeAll( category );
		hidden.removeAll( category );
		cModel->openCategory( category );
	}

	else{
		foldedCategories.append( category );
		folded.append( category );
		cModel->foldCategory( category );
	}

	folded.removeDuplicates();
	dirSett.setValue( "NewBreeze/HiddenCategories", hidden );
	dirSett.setValue( "NewBreeze/FoldedCategories", folded );
	dirSett.sync();

	hashIsDirty = true;
	calculateRectsIfNecessary();
};

bool NBIconView::canShowIndex( QModelIndex idx ) {

	QString category( cModel->category( idx ) );

	if ( hiddenCategories.contains( category ) )
		return false;

	else if ( foldedCategories.contains( category ) ) {
		QModelIndexList mList = cModel->indexListForCategory( category );
		if ( mList.indexOf( idx ) >= itemsPerRow - 1 )
			return false;

		else
			return true;
	}

	else
		return true;
};
