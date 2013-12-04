/*
	*
	* NBTreeView.cpp - NewBreeze FolderView Class
	*
*/

#include <NBTreeView.hpp>

NBTreeView::NBTreeView( NBFileSystemModel *fsm ) : QTreeView(), fsModel( fsm ) {

	// Set Data Model
	setModel( fsModel );

	// Item Delegate
	// iDelegate = new NBTreeDelegate();
	// setItemDelegate( iDelegate );

	// No Editing
	setEditTriggers( NoEditTriggers );

	// View, Sizes and Resize Modes
	updateViewMode();

	// Wrapping
	setWordWrap( true );

	// Uniform Row Height
	setUniformRowHeights( true );

	// No arrow for the root
	setRootIsDecorated( false );

	// Row-wise focus
	setAllColumnsShowFocus( true );

	// Alternate row colors
	setAlternatingRowColors( true );

	// Sorting
	setSortingEnabled( true );

	// Mouse Tracking
	setMouseTracking( true );

	// DragAndDrop
	viewport()->setAcceptDrops(true);
	setDragDropMode( QTreeView::DragDrop );
	setDropIndicatorShown( true );
	setDragEnabled( true );
	setAcceptDrops( true );

	// Selection
	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setSelectionBehavior( QAbstractItemView::SelectRows );

	// Header bg
	header()->setAttribute( Qt::WA_TranslucentBackground );

	// Default section width
	header()->setMinimumSectionSize( 75 );

	// Column Sizes
	header()->setStretchLastSection( false );
	header()->setResizeMode( 0, QHeaderView::Stretch );

	// Hide 2 and 5: File type verbose and Permissions
	header()->setSectionHidden( 2, true );
	header()->setSectionHidden( 5, true );

	header()->resizeSection( 1, 75 );
	header()->resizeSection( 3, 150 );
	header()->resizeSection( 4, 150 );
	header()->resizeSection( 6, 75 );

	// Context Menu
	setContextMenuPolicy( Qt::CustomContextMenu );

	// Create and setup actions like rename, etc, etc
	createAndSetupActions();

	// Give this object a name
	setObjectName( "mainList" );
};

void NBTreeView::updateViewMode() {

	if ( Settings->General.FolderView == QString( "SDetailsView" ) ) {

		setIconSize( QSize( 24, 24 ) );
	}

	else {

		setIconSize( QSize( 48, 48 ) );
	}
};


void NBTreeView::createAndSetupActions() {

	connect(
		this, SIGNAL( activated( QModelIndex ) ),
		this, SIGNAL( open( QModelIndex ) )
	);

	connect(
		this, SIGNAL( customContextMenuRequested( QPoint ) ),
		this, SIGNAL( contextMenuRequested( QPoint ) )
	);
};
void NBTreeView::mousePressEvent( QMouseEvent *mpEvent ) {

	if ( ( mpEvent->button() == Qt::LeftButton ) )
		dragStartPosition = mpEvent->pos();

	else
		dragStartPosition = QPoint();

	QTreeView::mousePressEvent( mpEvent );
};

void NBTreeView::mouseMoveEvent( QMouseEvent *mmEvent ) {

	if ( not ( mmEvent->buttons() & Qt::LeftButton ) ) {

		QTreeView::mouseMoveEvent( mmEvent );
		return;
	}

	else if ( dragStartPosition.isNull() ) {

		QTreeView::mouseMoveEvent( mmEvent );
		return;
	}

	else if ( ( mmEvent->pos() - dragStartPosition ).manhattanLength() < QApplication::startDragDistance() ) {

		QTreeView::mouseMoveEvent( mmEvent );
		return;
	}

	if ( selectionModel()->hasSelection() ) {

		QDrag *drag = new QDrag( this );

		QList<QUrl> urlList;
		foreach( QModelIndex item, selectionModel()->selectedRows() )
			urlList << QUrl( fsModel->nodePath( item.data().toString() ) );

		QMimeData *mimedata = new QMimeData();
		mimedata->setUrls( urlList );

		drag->setMimeData( mimedata );

		drag->exec( Qt::CopyAction | Qt::MoveAction | Qt::LinkAction );
	}

	mmEvent->accept();
};

void NBTreeView::mouseDoubleClickEvent( QMouseEvent *mouseEvent) {

	QModelIndex idx = indexAt( mouseEvent->pos() );
	if ( idx.isValid() )
		emit doubleClicked( idx );

	qDebug() << idx.data().toString();

	mouseEvent->accept();
};

void NBTreeView::keyPressEvent( QKeyEvent *keyEvent ) {

	QModelIndex curIndex( currentIndex() );

	if ( keyEvent->key() == Qt::Key_Down ) {
		// If the current selected item is the last in the visual row
		if ( ( curIndex.row() + 1 ) == fsModel->rowCount() ) {
			// Then go to the first one
			QModelIndex newIndex( fsModel->index( 0, 0, curIndex.parent() ) );
			// If the user is using control key, meaning using extended selection, then no update
			if ( qApp->queryKeyboardModifiers() & Qt::ControlModifier )
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::NoUpdate );

			else if ( qApp->queryKeyboardModifiers() & Qt::ShiftModifier )
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::Select );

			// otherwise, select it
			else
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::ClearAndSelect );
		}

		else {
			// then, move the selection/current index to the next item
			QModelIndex newIndex( fsModel->index( curIndex.row() + 1, 0, curIndex.parent() ) );
			// If the user is using control key, meaning using extended selection, then no update
			if ( qApp->queryKeyboardModifiers() & Qt::ControlModifier )
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::NoUpdate );

			else if ( qApp->queryKeyboardModifiers() & Qt::ShiftModifier )
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::Select );

			// otherwise, select it
			else
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::ClearAndSelect );
		}
	}

	else if ( keyEvent->key() == Qt::Key_Up ) {

		// If its the first item in the whole list
		if ( ( curIndex.row() ) == 0 ) {
			// Then go to the last one
			QModelIndex newIndex( fsModel->index( fsModel->rowCount() - 1, 0, curIndex.parent() ) );
			// If the user is using control key, meaning using extended selection, then no update
			if ( qApp->queryKeyboardModifiers() & Qt::ControlModifier )
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::NoUpdate );

			else if ( qApp->queryKeyboardModifiers() & Qt::ShiftModifier )
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::Select );

			// otherwise, select it
			else
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::ClearAndSelect );
		}

		// If the current selected item is the first in the visual row
		else {
			// then, move the cursor to the last item of the previous visual row
			QModelIndex newIndex( fsModel->index( curIndex.row() - 1, 0, curIndex.parent() ) );
			// If the user is using control key, meaning using extended selection, then no update
			if ( qApp->queryKeyboardModifiers() & Qt::ControlModifier )
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::NoUpdate );

			else if ( qApp->queryKeyboardModifiers() & Qt::ShiftModifier )
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::Select );

			// otherwise, select it
			else
				selectionModel()->setCurrentIndex( newIndex, QItemSelectionModel::ClearAndSelect );
		}
	}

	else {
		QTreeView::keyPressEvent( keyEvent );
	}

	keyEvent->accept();
};

void NBTreeView::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBTreeView::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	if ( indexAt( dmEvent->pos() ).isValid() ) {
		QModelIndex idx = indexAt( dmEvent->pos() );
		QString mtpt = fsModel->nodePath( fsModel->data( idx ).toString() );

		if ( QFileInfo( mtpt ).isWritable() ) {
			dmEvent->setDropAction( Qt::CopyAction );
			dmEvent->accept();
		}

		else
			dmEvent->ignore();
	}

	else
		dmEvent->ignore();
};

void NBTreeView::dropEvent( QDropEvent *dpEvent ) {

	QModelIndex idx = indexAt( dpEvent->pos() );
	QString mtpt = fsModel->nodePath( fsModel->data( idx ).toString() );

	const QMimeData *mData = dpEvent->mimeData();
	if ( mData->hasUrls() ) {

		QStringList args;
		foreach( QUrl url, mData->urls() )
			args << url.toLocalFile();

		// Move -> Shift
		// Copy -> Ctrl
		if ( dpEvent->keyboardModifiers() == Qt::ShiftModifier ) {

			emit move( args, mtpt );
		}

		else if ( ( dpEvent->keyboardModifiers() == Qt::ControlModifier ) or ( dpEvent->keyboardModifiers() == Qt::NoModifier ) ) {

			emit copy( args, mtpt );
		}

		else if ( dpEvent->keyboardModifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) ) {

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
