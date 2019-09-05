/*
	*
	* NBTrashView.cpp - NewBreeze TrashView Class
	*
*/

#include "NBTrashView.hpp"

NBTrashView::NBTrashView( NBTrashModel *model, QWidget *parent ) : QTreeView( parent ) {

	setRootIsDecorated( false );
	setIconSize( QSize( 24, 24 ) );

	setModel( model );

	setSelectionMode( QAbstractItemView::ExtendedSelection );
	setSelectionBehavior( QAbstractItemView::SelectRows );

	#if QT_VERSION >= 0x050000
		header()->setStretchLastSection( false );
		header()->setSectionResizeMode( 1, QHeaderView::Stretch );
		header()->resizeSection( 0, 250 );
		header()->resizeSection( 2, 100 );
		header()->resizeSection( 3, 150 );
	#else
		header()->setStretchLastSection( false );
		header()->setResizeMode( 1, QHeaderView::Stretch );
		header()->resizeSection( 0, 250 );
		header()->resizeSection( 2, 100 );
		header()->resizeSection( 3, 150 );
	#endif

	/* Styling: Borderless */
	setFrameStyle( QFrame::NoFrame );

	QPalette pltt( palette() );
	pltt.setColor( QPalette::Base, Qt::transparent );
	setPalette( pltt );

	setAllColumnsShowFocus( true );
};

int NBTrashView::sizeHintForColumn( int column ) {

	switch( column ) {
		case 0:
			return 250;
		//case 1:
			//return viewport()->width() - 400;
		case 2:
			return 100;
		case 3:
			return 200;
	}

	return 100;
};
