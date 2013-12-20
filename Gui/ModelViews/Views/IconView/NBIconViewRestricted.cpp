/*
	*
	* NBIconViewRestricted.cpp - NewBreeze IconView Class
	*
*/

#include <NBIconViewRestricted.hpp>

NBIconViewRestricted::NBIconViewRestricted( NBFileSystemModel *fsModel ) : NBIconView( fsModel ) {

	mdl = fsModel;

	setSelectionMode( QAbstractItemView::NoSelection );
	setContextMenuPolicy( Qt::NoContextMenu );

	viewport()->setAcceptDrops( false );
	setDragEnabled( false );
	setAcceptDrops( false );
	setDragDropMode( QListView::NoDragDrop );
	setDropIndicatorShown( false );
};

void NBIconViewRestricted::mousePressEvent( QMouseEvent *mpEvent ) {

	/* Disable all the mouse press events */
	QAbstractItemView::mousePressEvent( mpEvent );
};

void NBIconViewRestricted::mouseMoveEvent( QMouseEvent *mmEvent ) {

	/* Disable all the mouse move events */
	mmEvent->accept();
};

void NBIconViewRestricted::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	/* Disable all the mouse release events */
	mrEvent->accept();
};

void NBIconViewRestricted::mouseDoubleClickEvent( QMouseEvent *mEvent ) {

	mEvent->accept();
};

void NBIconViewRestricted::dragEnterEvent( QDragEnterEvent *deEvent ) {

	deEvent->ignore();
};

void NBIconViewRestricted::dragMoveEvent( QDragMoveEvent *dmEvent ) {

	dmEvent->ignore();
};

void NBIconViewRestricted::dropEvent( QDropEvent *dpEvent ) {

	dpEvent->ignore();
};
