/*
	*
	* NBSidePanel.cpp - The side panel showing My Computer ( All the mounted drives )
	*
*/

#include <NBSidePanelView.hpp>
#include <NBTools.hpp>

NBSidePanel::NBSidePanel() {

	spModel = new NBSidePanelModel();
	setModel( spModel );

	setRootIndex( spModel->rootIndex() );
	connect( this, SIGNAL( clicked( const QModelIndex ) ), this, SLOT( handleClick( const QModelIndex ) ) );

	iDelegate = new NBSidePanelDelegate();
	setItemDelegate( iDelegate );

	setupView();
}

void NBSidePanel::setupView() {

	// No Focus
	setFocusPolicy( Qt::NoFocus );

	// StyleSheet
	setStyleSheet( getStyleSheet( "NBSidePanel", Settings->General.Style ) );

	// No Header
	setHeaderHidden( true );

	// Selection
	setSelectionMode( QTreeView::NoSelection );

	// By default set both Devices and Bookmarks to previous state
	setExpanded( spModel->index( 0, 0 ), Settings->Session.ExpandDevices );
	setExpanded( spModel->index( 1, 0 ), Settings->Session.ExpandBookmarks );

	// Fixed Width
	setFixedWidth( sizeHintForColumn( 0 ) );

	// Mouse Tracking
	setMouseTracking( true );

	// DragAndDrop
	viewport()->setAcceptDrops(true);
	setDragDropMode( QListView::DragDrop );
	setDropIndicatorShown( true );
	setDragEnabled( true );
	setAcceptDrops( true );
};

void NBSidePanel::updateDevices() {

	setExpanded( spModel->index( 0, 0 ), false );
	spModel->updateDeviceData();
	setExpanded( spModel->index( 0, 0 ), true );

	resizeColumnToContents( 0 );
	setFixedWidth( sizeHintForColumn( 0 ) );
};

void NBSidePanel::updateBookmarks() {

	setExpanded( spModel->index( 1, 0 ), false );
	spModel->updateBookmarkData();
	setExpanded( spModel->index( 1, 0 ), true );

	resizeColumnToContents( 0 );
	setFixedWidth( sizeHintForColumn( 0 ) );
};

void NBSidePanel::handleClick( const QModelIndex clickedIndex ) {

	if ( ( clickedIndex.parent() == spModel->parent() ) and ( clickedIndex.row() == 2 ) )
		emit specialUrl( clickedIndex.data( Qt::UserRole + 1 ).toString() );

	else if ( ( clickedIndex.parent() == spModel->parent() ) and ( clickedIndex.row() == 3 ) )
		emit specialUrl( clickedIndex.data( Qt::UserRole + 1 ).toString() );

	else if ( clickedIndex.parent() != spModel->parent() )
		emit driveClicked( clickedIndex.data( Qt::UserRole + 1 ).toString() );
};

void NBSidePanel::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->acceptProposedAction();
};

void NBSidePanel::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	if ( indexAt( dmEvent->pos() ).isValid() ) {
		QModelIndex idx = indexAt( dmEvent->pos() );
		QString mtpt = spModel->data( idx, Qt::UserRole + 1 ).toString();

		if ( idx.parent() == spModel->parent() ) {
			if ( not isExpanded( idx ) ) {
				setExpanded( idx, true );
			}
		}

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

void NBSidePanel::dropEvent( QDropEvent *dpEvent ) {

	QModelIndex idx = indexAt( dpEvent->pos() );
	QString mtpt = spModel->data( idx, Qt::UserRole + 1 ).toString();

	const QMimeData *mData = dpEvent->mimeData();
	if ( mData->hasUrls() ) {

		QStringList args;
		foreach( QUrl url, mData->urls() )
			args << url.toLocalFile();

		// Copy -> Ctrl
		// Move -> Shift
		// Link -> Ctrl + Shift
		if ( dpEvent->keyboardModifiers() == Qt::ShiftModifier ) {

			emit move( args, mtpt, NBIOMode::Move );
		}

		else if ( ( dpEvent->keyboardModifiers() == Qt::ControlModifier ) or ( dpEvent->keyboardModifiers() == Qt::NoModifier ) ) {

			emit copy( args, mtpt, NBIOMode::Copy );
		}

		else if ( dpEvent->keyboardModifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) ) {

			foreach( QString node, args )
				QFile::link( node, QDir( mtpt ).filePath( baseName( node ) ) );
		}

		else {

			dpEvent->ignore();
			return;
		}
	}

	dpEvent->accept();
};
