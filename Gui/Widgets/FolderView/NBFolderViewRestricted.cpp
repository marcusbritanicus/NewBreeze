/*
	*
	* NBFolderView.cpp - NewBreeze FolderView Class
	*
*/

#include <NBFolderViewRestricted.hpp>

NBFolderViewRestricted::NBFolderViewRestricted( QString path ) : QListView() {

	currentPath = QString( path );

	// Set Data Model
	model = new NBFileSystemModel();
	setModel( model );
	model->setReadOnly( true );
	model->setShowHidden( Settings->Session.ShowHidden );

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
	model->setRootPath( path );
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

	if ( Settings->General.FolderView == QString( "TilesView" ) ) {
		// View and Resize Modes
		setViewMode( QListView::ListMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( QSize( 256, 64 ) );
		setIconSize( QSize( 48, 48 ) );

		// Wrapping
		setWrapping( true );
	}

	else {
		// View and Resize Modes
		setViewMode( QListView::IconMode );
		setFlow( QListView::LeftToRight );

		// Sizes
		setGridSize( QSize( 120, 80 ) );
		setIconSize( QSize( 48, 48 ) );

		// Wrapping
		setWrapping( true );
	}
};
