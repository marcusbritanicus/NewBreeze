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
	setIcon( QIcon( ":/icons/menu.png" ) );

	/* Internal Object Name */
	setObjectName( "menuBtn" );

	/* Button Size */
	setFixedSize( 28, 28 );

	/* No Borders */
	setStyleSheet( "QToolButton#menuBtn{ border: none; } QToolButton#menuBtn:hover { border: none; background-color: #A1DFFF; border-radius: 3px; }" );

	menu = new NBSystemMenu( this );

	connect( this, SIGNAL( clicked() ), this, SLOT( showMenu() ) );
};

void NBMenuButton::showMenu() {

	setStyleSheet( "QToolButton#menuBtn{ background-color: darkgray; border: none; border-radius: 3px; }" );

	QPoint point = mapToGlobal( rect().bottomRight() - QPoint( menu->width(), 0 ) );
	menu->exec( point );

	setStyleSheet( "QToolButton#menuBtn{ border: none; } QToolButton#menuBtn:hover { border: none; background-color: #A1DFFF; border-radius: 3px; }" );
};

void NBMenuButton::updateViewMode( QString mode ) {
};
