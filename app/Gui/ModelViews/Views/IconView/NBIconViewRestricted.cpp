/*
	*
	* NBIconViewRestricted.cpp - NewBreeze IconView Class
	*
*/

#include "NBIconViewRestricted.hpp"

NBIconViewRestricted::NBIconViewRestricted( NBItemViewModel *fsModel, QWidget *parent ) : NBIconView( fsModel, parent ) {

	mdl = fsModel;

	setSelectionMode( QAbstractItemView::NoSelection );
	setContextMenuPolicy( Qt::NoContextMenu );

	viewport()->setAcceptDrops( false );
	setDragEnabled( false );
	setAcceptDrops( false );
	setDragDropMode( QListView::NoDragDrop );
	setDropIndicatorShown( false );

	QPalette pltt( palette() );
	pltt.setColor( QPalette::Base, Qt::transparent );
	setPalette( pltt );
};

void NBIconViewRestricted::mousePressEvent( QMouseEvent *mpEvent ) {

	/* Disable all the special mouse press events */
	QAbstractItemView::mousePressEvent( mpEvent );
};

void NBIconViewRestricted::mouseMoveEvent( QMouseEvent *mmEvent ) {

	mmEvent->accept();
};

void NBIconViewRestricted::mouseReleaseEvent( QMouseEvent *mrEvent ) {

	mrEvent->accept();
};

void NBIconViewRestricted::mouseDoubleClickEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		QModelIndex idx = indexAt( mEvent->pos() );
		if ( idx.isValid() ) {
			switch( mdl->modelDataType() ) {
				case NBItemViewModel::SuperStart:
				case NBItemViewModel::Applications:
				case NBItemViewModel::Catalogs:
					break;

				case NBItemViewModel::FileSystem: {

					emit open( idx );
					break;
				}
			}
		}
	}

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
