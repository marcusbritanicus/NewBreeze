/*
	*
	* NBMenuButton.cpp - Alternate PushButton/ToolButton for NewBreeze
	*
*/

#include "NBMenuButton.hpp"
#include "NBIconManager.hpp"
#include "NBGuiFunctions.hpp"

NBMenuButton::NBMenuButton( QWidget *parent ) : QToolButton( parent ) {

	/* Button Icon */
	setIcon( QIcon::fromTheme( "application-menu" ) );

	/* Internal Object Name */
	setObjectName( "menuBtn" );

	/* Button Size */
	setFixedSize( 28, 28 );

	/* Click to show menu */
	connect( this, SIGNAL( clicked() ), this, SLOT( showMenu() ) );

	/* No Borders */
	setStyleSheet( "QToolButton#menuBtn{ border: none; } QToolButton#menuBtn:hover { border: none; background-color: #A1DFFF; border-radius: 3px; }" );
};

void NBMenuButton::showMenu() {

	setStyleSheet( "QToolButton#menuBtn{ background-color: darkgray; border: none; border-radius: 3px; }" );

	NBSystemMenu *menu = new NBSystemMenu( this );
	QPoint point = mapToGlobal( rect().bottomRight() - QPoint( menu->width(), 0 ) );

	connect( menu, SIGNAL( newWindow() ), this, SIGNAL( newWindow() ) );
	connect( menu, SIGNAL( zoomIn() ), this, SIGNAL( zoomIn() ) );
	connect( menu, SIGNAL( zoomOut() ), this, SIGNAL( zoomOut() ) );
	connect( menu, SIGNAL( cut() ), this, SIGNAL( cut() ) );
	connect( menu, SIGNAL( copy() ), this, SIGNAL( copy() ) );
	connect( menu, SIGNAL( paste() ), this, SIGNAL( paste() ) );
	connect( menu, SIGNAL( openVTE() ), this, SIGNAL( openVTE() ) );
	connect( menu, SIGNAL( changeViewMode( int ) ), this, SIGNAL( changeViewMode( int ) ) );
	connect( menu, SIGNAL( sortByName() ), this, SIGNAL( sortByName() ) );
	connect( menu, SIGNAL( sortByType() ), this, SIGNAL( sortByType() ) );
	connect( menu, SIGNAL( sortBySize() ), this, SIGNAL( sortBySize() ) );
	connect( menu, SIGNAL( sortByDate() ), this, SIGNAL( sortByDate() ) );
	connect( menu, SIGNAL( toggleHidden() ), this, SIGNAL( toggleHidden() ) );
	connect( menu, SIGNAL( toggleGrouping() ), this, SIGNAL( toggleGrouping() ) );
	connect( menu, SIGNAL( showSettings() ), this, SIGNAL( showSettings() ) );
	connect( menu, SIGNAL( closeWindow() ), this, SIGNAL( closeWindow() ) );
	connect( menu, SIGNAL( quit() ), this, SIGNAL( quit() ) );

	menu->exec( point );

	setStyleSheet( "QToolButton#menuBtn{ border: none; } QToolButton#menuBtn:hover { border: none; background-color: #A1DFFF; border-radius: 3px; }" );
};
