/*
	*
	* NBSidePanel.cpp - The side panel showing My Computer ( All the mounted drives )
	*
*/

#include <NBSidePanelView.hpp>
#include <NBTools.hpp>

/*
	if ( devWidth > bmkWidth )
		sidePanelWidth = devWidth > 200 ? 200 : devWidth;

	else
		sidePanelWidth = bmkWidth > 200 ? 200 : bmkWidth;
*/

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
	setStyleSheet( getStyleSheet( "NBSidePanel", Settings.General.Style ) );

	// No Header
	setHeaderHidden( true );

	// By default expand both Devices and Bookmarks
	setExpanded( spModel->index( 0, 0 ), true );
	setExpanded( spModel->index( 1, 0 ), true );

	// Fixed Width
	setFixedWidth( sizeHintForColumn( 0 ) );
};

void NBSidePanel::updateDevices() {

	setExpanded( spModel->index( 0, 0 ), false );
	spModel->updateDeviceData();
	setExpanded( spModel->index( 0, 0 ), true );

	setFixedWidth( sizeHintForColumn( 0 ) );
};

void NBSidePanel::updateBookmarks() {

	setExpanded( spModel->index( 1, 0 ), false );
	spModel->updateBookmarkData();
	setExpanded( spModel->index( 1, 0 ), true );

	setFixedWidth( sizeHintForColumn( 0 ) );
};

void NBSidePanel::handleClick( const QModelIndex clickedIndex ) {

	if ( clickedIndex.parent().data() == "Devices" ) {
		QVariant devVar = clickedIndex.data( Qt::UserRole + 1 );
		NBDeviceInfo info = devVar.value<NBDeviceInfo>();
		emit driveClicked( info.mountPoint() );
	}

	else if ( clickedIndex.parent().data() == "Bookmarks" ) {

		emit driveClicked( clickedIndex.data( Qt::UserRole + 1 ).toString() );
	}
};
