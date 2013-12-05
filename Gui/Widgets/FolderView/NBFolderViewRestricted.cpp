/*
	*
	* NBFolderView.cpp - NewBreeze FolderView Class
	*
*/

#include <NBFolderViewRestricted.hpp>

NBFolderViewRestricted::NBFolderViewRestricted( QString path ) : QListView() {

	currentPath = QString( path );

	// Set Data Model
	fsModel = new NBFileSystemModel();
	setModel( fsModel );
	fsModel->setReadOnly( true );

	// Item Delegate
	NBIconDelegate *iDelegate = new NBIconDelegate();
	setItemDelegate( iDelegate );

	// View, Sizes and Resize Modes
	updateViewMode();

	// DragAndDrop
	setDragDropMode( QAbstractItemView::NoDragDrop );

	// Selection
	setSelectionMode( QAbstractItemView::NoSelection );

	// Wrapping
	setWordWrap( true );

	setResizeMode( QListView::Adjust );
	setMovement( QListView::Static );

	// Context Menu
	setContextMenuPolicy( Qt::NoContextMenu );

	// Load items
	fsModel->setRootPath( path );
};

void NBFolderViewRestricted::updateViewMode() {

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

	if ( Settings->General.FolderView == QString( "SmallListView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::ListMode );
		setFlow( QListView::TopToBottom );

		// Sizes
		setGridSize( QSize( 256, 28 ) );
		setIconSize( QSize( 24, 24 ) );

		// Wrapping
		setWrapping( true );
	}

	else if ( Settings->General.FolderView == QString( "NormalListView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::ListMode );
		setFlow( QListView::TopToBottom );

		// Sizes
		setGridSize( QSize( 256, 48 ) );
		setIconSize( QSize( 48, 48 ) );

		// Wrapping
		setWrapping( false );
	}

	else if ( Settings->General.FolderView == QString( "TilesView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::ListMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( QSize( 256, 64 ) );
		setIconSize( QSize( 48, 48 ) );

		// Wrapping
		setWrapping( true );
	}

	else if ( Settings->General.FolderView == QString( "SmallIconsView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::IconMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( QSize( 80, 48 ) );
		setIconSize( QSize( 24, 24 ) );

		// Wrapping
		setWrapping( true );
	}

	else if ( Settings->General.FolderView == QString( "NormalIconsView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::IconMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( QSize( 120, 80 ) );
		setIconSize( QSize( 48, 48 ) );

		// Wrapping
		setWrapping( true );
	}

	else if ( Settings->General.FolderView == QString( "LargeIconsView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::IconMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( QSize( 160, 100 ) );
		setIconSize( QSize( 64, 64 ) );

		// Wrapping
		setWrapping( true );
	}

	else if ( Settings->General.FolderView == QString( "HugeIconsView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::IconMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( QSize( 300, 180 ) );
		setIconSize( QSize( 128, 128 ) );

		// Wrapping
		setWrapping( true );
	}
};
