/*
	*
	* NBQuickMenuBar.cpp - NewBreeze AddressBar Class
	*
*/

#include <NBQuickMenuBar.hpp>

NBQuickMenuBar::NBQuickMenuBar() : QWidget() {

	QList<QIcon> icons;
	QStringList labels;

	icons << QIcon( ":/icons/open.png" ) << QIcon( ":/icons/move.png" ) << QIcon( ":/icons/delete.png" ) << QIcon( ":/icons/props.png" );
	icons << QIcon( ":/icons/perms.png" );

	labels << QString( "&Open With" ) << QString( "&Move To" ) << QString( "&Delete" ) << QString( "&Properties" ) << QString( "P&ermissions" );

	quickBtns = new NBButtons( labels, icons );
	quickBtns->setSegmentWidth( 128 );

	quickBtns->setSegmentDisabled( 0 );
	quickBtns->setSegmentDisabled( 1 );
	quickBtns->setSegmentDisabled( 2 );
	quickBtns->setSegmentEnabled( 3 );
	quickBtns->setSegmentEnabled( 4 );

	connect( quickBtns->segment( 0 ), SIGNAL( clicked() ), this, SIGNAL( openWithClicked() ) );
	connect( quickBtns->segment( 2 ), SIGNAL( clicked() ), this, SIGNAL( deleteClicked() ) );
	connect( quickBtns->segment( 3 ), SIGNAL( clicked() ), this, SIGNAL( propsClicked() ) );
	connect( quickBtns->segment( 4 ), SIGNAL( clicked() ), this, SIGNAL( permsClicked() ) );

	setContentsMargins( QMargins() );
	QHBoxLayout *toolsLyt = new QHBoxLayout();
	toolsLyt->setContentsMargins( QMargins() );

	toolsLyt->addWidget( quickBtns );
	toolsLyt->addStretch( 0 );

	setFixedHeight( 24 );
	setLayout( toolsLyt );
};
