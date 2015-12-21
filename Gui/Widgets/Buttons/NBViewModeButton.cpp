/*
	*
	* NBViewModeButton.cpp - ViewMode changer button for NewNreeze
	*
*/

#include <NBViewModeButton.hpp>

NBViewModeButton::NBViewModeButton( QWidget *parent ) : NBButton( parent ) {

	setIcon( QIcon( ":/icons/view-choose.png" ) );
	connect( this, SIGNAL( clicked() ), this, SIGNAL( switchToNextView() ) );

	QMenu *menu = new QMenu( this );

	tilesAct = new QAction( QIcon::fromTheme( "view-list-details" ), "&Tiles View", this );
	tilesAct->setCheckable( true );
	connect( tilesAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	iconsAct = new QAction( QIcon::fromTheme( "view-list-icons" ), "&Icons View", this );
	iconsAct->setCheckable( true );
	connect( iconsAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	detailsAct = new QAction( QIcon::fromTheme( "view-list-text" ), "&Details View", this );
	detailsAct->setCheckable( true );
	connect( detailsAct, SIGNAL( triggered() ), this, SIGNAL( changeViewMode() ) );

	viewsGroup = new QActionGroup( this );

	menu->addAction( viewsGroup->addAction( tilesAct ) );
	menu->addAction( viewsGroup->addAction( iconsAct ) );
	menu->addAction( viewsGroup->addAction( detailsAct ) );

	setMenu( menu );

	if ( Settings->General.ViewMode == QString( "TilesView" ) )
		tilesAct->setChecked( true );

	else if ( Settings->General.ViewMode == QString( "IconsView" ) )
		iconsAct->setChecked( true );

	else if ( Settings->General.ViewMode == QString( "DetailsView" ) )
		detailsAct->setChecked( true );
};

int NBViewModeButton::checkedAction() {

	if ( viewsGroup->checkedAction() == tilesAct )
		return 0;

	else if ( viewsGroup->checkedAction() == iconsAct )
		return 1;

	else
		return 2;
};

void NBViewModeButton::showMenu() {

	if ( Settings->General.ViewMode == QString( "TilesView" ) )
		tilesAct->setChecked( true );

	else if ( Settings->General.ViewMode == QString( "IconsView" ) )
		iconsAct->setChecked( true );

	else if ( Settings->General.ViewMode == QString( "DetailsView" ) )
		detailsAct->setChecked( true );

	NBButton::showMenu();
};
