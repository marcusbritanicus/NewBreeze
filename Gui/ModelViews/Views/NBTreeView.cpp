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
	setDragDropMode( QListView::DragDrop );
	setDropIndicatorShown( true );
	setDragEnabled( true );
	setAcceptDrops( true );

	// Selection
	setSelectionMode( QAbstractItemView::ExtendedSelection );

	// Column Sizes
	header()->setStretchLastSection( false );
	header()->setResizeMode( 0, QHeaderView::Stretch );
	header()->setResizeMode( 1, QHeaderView::Fixed );
	header()->setResizeMode( 2, QHeaderView::Fixed );
	header()->setResizeMode( 3, QHeaderView::Fixed );
	header()->setResizeMode( 4, QHeaderView::Fixed );
	header()->setResizeMode( 5, QHeaderView::Fixed );
	header()->setResizeMode( 6, QHeaderView::Fixed );

	header()->resizeSection( 1, 50 );
	header()->resizeSection( 2, 100 );
	header()->resizeSection( 3, 100 );
	header()->resizeSection( 4, 150 );
	header()->resizeSection( 5, 150 );
	header()->resizeSection( 6, 150 );

	// Context Menu
	setContextMenuPolicy( Qt::CustomContextMenu );

	// Create and setup actions like rename, etc, etc
	createAndSetupActions();

	// Give this object a name
	setObjectName( "mainList" );

	// StyleSheet
	setStyleSheet(  getStyleSheet( "NewBreeze", Settings.General.Style ) );
};

void NBTreeView::updateViewMode() {

	if ( Settings.General.FolderView == QString( "SDetailsView" ) ) {

		setIconSize( QSize( 24, 24 ) );
	}

	else {

		setIconSize( QSize( 48, 48 ) );
	}
};

void NBTreeView::mouseDoubleClickEvent( QMouseEvent *mouseEvent) {

	QModelIndex idx = indexAt( mouseEvent->pos() );
	if ( idx.isValid() )
		emit doubleClicked( idx );

	qDebug() << idx.data().toString();

	mouseEvent->accept();
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

		else if ( ( dpEvent->keyboardModifiers() == Qt::ControlModifier ) or ( dpEvent->keyboardModifiers() == Qt::ControlModifier ) ) {

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
