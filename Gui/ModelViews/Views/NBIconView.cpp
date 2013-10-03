/*
	*
	* NBIconView.cpp - NewBreeze FolderView Class
	*
*/

#include <NBIconView.hpp>

NBIconView::NBIconView( NBFileSystemModel *fsm ) : QListView(), fsModel( fsm ) {

	contentsWidth = 640;
	itemsPerRow = 0;
	numberOfRows = 0;

	// Set Data Model
	setModel( fsModel );

	// Item Delegate
	iDelegate = new NBIconDelegate();
	setItemDelegate( iDelegate );

	// View, Sizes and Resize Modes
	updateViewMode();

	// All the items have uniform sizes
	setUniformItemSizes( true );

	// Wrapping
	setWordWrap( true );

	// Minimum Width - 320px
	setMinimumWidth( 640 );

	setResizeMode( QListView::Adjust );
	setMovement( QListView::Static );

	// Mouse Tracking
	setMouseTracking( true );

	// DragAndDrop
	viewport()->setAcceptDrops(true);
	setDragDropMode( QListView::DragDrop );
	setDropIndicatorShown( true );
	setDragEnabled( true );
	setAcceptDrops( true );

	// Selection
	setSelectionMode( QAbstractItemView::ExtendedSelection );

	// Context Menu
	setContextMenuPolicy( Qt::CustomContextMenu );

	// Create and setup actions
	createAndSetupActions();

	// Give this object a name
	setObjectName( "mainList" );
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

	if ( Settings.General.FolderView == QString( "SmallListView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::ListMode );
		setFlow( QListView::TopToBottom );

		// Sizes
		setGridSize( getGridSize( QSize( 256, 28 ) ) );
		setIconSize( QSize( 24, 24 ) );

		// Wrapping
		setWrapping( true );
	}

	else if ( Settings.General.FolderView == QString( "NormalListView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::ListMode );
		setFlow( QListView::TopToBottom );

		// Sizes
		setGridSize( QSize( 256, 52 ) );
		setIconSize( QSize( 48, 48 ) );

		// Wrapping
		setWrapping( false );
	}

	else if ( Settings.General.FolderView == QString( "TilesView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::ListMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( getGridSize( QSize( 256, 64 ) ) );
		setIconSize( QSize( 48, 48 ) );

		// Wrapping
		setWrapping( true );
	}

	else if ( Settings.General.FolderView == QString( "SmallIconsView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::IconMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( getGridSize( QSize( 80, 48 ) ) );
		setIconSize( QSize( 24, 24 ) );

		// Wrapping
		setWrapping( true );
	}

	else if ( Settings.General.FolderView == QString( "NormalIconsView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::IconMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( getGridSize( QSize( 120, 80 ) ) );
		setIconSize( QSize( 48, 48 ) );

		// Wrapping
		setWrapping( true );
	}

	else if ( Settings.General.FolderView == QString( "LargeIconsView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::IconMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( getGridSize( QSize( 160, 100 ) ) );
		setIconSize( QSize( 64, 64 ) );

		// Wrapping
		setWrapping( true );
	}

	else if ( Settings.General.FolderView == QString( "HugeIconsView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::IconMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( getGridSize( QSize( 300, 180 ) ) );
		setIconSize( QSize( 128, 128 ) );

		// Wrapping
		setWrapping( true );
	}
};

void NBIconView::setContentsWidth( int cWidth ) {

	contentsWidth = cWidth;
};

void NBIconView::createAndSetupActions() {

	connect(
		this, SIGNAL( activated( QModelIndex ) ),
		this, SIGNAL( open( QModelIndex ) )
	);

	connect(
		this, SIGNAL( customContextMenuRequested( QPoint ) ),
		this, SIGNAL( contextMenuRequested( QPoint ) )
	);
};
QSize NBIconView::getGridSize( QSize mSize ) {

	// These are the items per row
	itemsPerRow = ( int )( contentsWidth / mSize.width() );

	// These are the number of visual rows
	numberOfRows = ( int )( fsModel->rowCount() / itemsPerRow );
	if ( ( fsModel->rowCount() % itemsPerRow ) > 0 )
		numberOfRows++;

	// Extended Grid Size
	int gWidth = ( int )( contentsWidth / itemsPerRow );

	return QSize( gWidth, mSize.height() );
};

void NBIconView::mouseDoubleClickEvent( QMouseEvent *mouseEvent) {

	QModelIndex idx = indexAt( mouseEvent->pos() );
	if ( idx.isValid() )
		emit open( idx );

	mouseEvent->accept();
};

void NBIconView::keyPressEvent( QKeyEvent *keyEvent) {

	QModelIndex curIndex( currentIndex() );

	if ( keyEvent->key() == Qt::Key_Right ) {
		// If the current selected item is the last in the visual row
		if ( ( curIndex.row() + 1 ) % itemsPerRow == 0 ) {
			// then, move the cursor to the first item of the next visual row
			selectionModel()->setCurrentIndex( fsModel->index( curIndex.row() + 1, 0, curIndex.parent() ), QItemSelectionModel::NoUpdate );
		}

		// If its the last item in the whole list
		else if ( ( curIndex.row() + 1 ) == fsModel->rowCount() ) {
			// Then go to the first one
			selectionModel()->setCurrentIndex( fsModel->index( 0, 0, curIndex.parent() ), QItemSelectionModel::NoUpdate );
		}

		else {
			// Otherwise do the usual moves
			QListView::keyPressEvent( keyEvent );
		}
	}

	else if ( keyEvent->key() == Qt::Key_Left ) {

		// If its the first item in the whole list
		if ( ( curIndex.row() ) == 0 ) {
			// Then go to the last one
			selectionModel()->setCurrentIndex( fsModel->index( fsModel->rowCount() - 1, 0, curIndex.parent() ), QItemSelectionModel::NoUpdate );
		}

		// If the current selected item is the first in the visual row
		else if ( ( curIndex.row() ) % itemsPerRow == 0 ) {
			// then, move the cursor to the last item of the previous visual row
			selectionModel()->setCurrentIndex( fsModel->index( curIndex.row() - 1, 0, curIndex.parent() ), QItemSelectionModel::NoUpdate );
		}

		else {
			// Otherwise do the usual moves
			QListView::keyPressEvent( keyEvent );
		}
	}

	else {
		QListView::keyPressEvent( keyEvent );
	}

	keyEvent->accept();
};

void NBIconView::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBIconView::dragMoveEvent( QDragMoveEvent *dmEvent ) {

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

void NBIconView::dropEvent( QDropEvent *dpEvent ) {

	QModelIndex idx = indexAt( dpEvent->pos() );
	QString mtpt = fsModel->nodePath( fsModel->data( idx ).toString() );

	const QMimeData *mData = dpEvent->mimeData();
	if ( mData->hasUrls() ) {

		QStringList args;
		foreach( QUrl url, mData->urls() )
			args << url.toLocalFile();

		if ( dpEvent->keyboardModifiers() == Qt::ShiftModifier ) {

			qDebug() << "DnD Move";
			emit move( args, mtpt );
		}

		else if ( ( dpEvent->keyboardModifiers() == Qt::ControlModifier ) or ( dpEvent->keyboardModifiers() == Qt::ControlModifier ) ) {

			qDebug() << "DnD Copy";
			emit copy( args, mtpt );
		}

		else if ( dpEvent->keyboardModifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) ) {

			qDebug() << "DnD Link";
			emit link( args, mtpt );
		}

		else {

			dpEvent->ignore();
			return;
		}
	}

	dpEvent->accept();
};
