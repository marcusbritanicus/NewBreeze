/*
	*
	* NBQuickMenuBar.cpp - NewBreeze AddressBar Class
	*
*/

#include <NBQuickMenuBar.hpp>

using namespace std;

NBQuickMenuBar::NBQuickMenuBar() : QWidget() {

	openWithBtn = new NBPushButton( QIcon( ":/icons/open.png" ), tr( "&Open With" ) );
	moveToBtn = new NBPushButton( QIcon( ":/icons/move.png" ), tr( "&Move To" ) );
	deleteBtn = new NBPushButton( QIcon( ":/icons/delete.png" ), tr( "&Delete" ) );
	propsBtn = new NBPushButton( QIcon( ":/icons/props.png" ), tr( "&Properties" ) );
	permsBtn = new NBPushButton( QIcon( ":/icons/perms.png" ), tr( "P&ermissions" ) );

	openWithBtn->setFocusPolicy( Qt::NoFocus );
	moveToBtn->setFocusPolicy( Qt::NoFocus );
	deleteBtn->setFocusPolicy( Qt::NoFocus );
	propsBtn->setFocusPolicy( Qt::NoFocus );
	permsBtn->setFocusPolicy( Qt::NoFocus );

	openWithBtn->setDisabled( true );
	moveToBtn->setDisabled( true );
	deleteBtn->setDisabled( true );
	permsBtn->setDisabled( true );

	openWithBtn->setStyleSheet( getStyleSheet( "NBToolButton", settings.value( "Style" ).toString() ) );
	moveToBtn->setStyleSheet( getStyleSheet( "NBToolButton", settings.value( "Style" ).toString() ) );
	deleteBtn->setStyleSheet( getStyleSheet( "NBToolButton", settings.value( "Style" ).toString() ) );
	propsBtn->setStyleSheet( getStyleSheet( "NBToolButton", settings.value( "Style" ).toString() ) );
	permsBtn->setStyleSheet( getStyleSheet( "NBToolButton", settings.value( "Style" ).toString() ) );

	setContentsMargins( QMargins() );
	QHBoxLayout *toolsLyt = new QHBoxLayout();
	toolsLyt->setContentsMargins( QMargins() );

	toolsLyt->addWidget( openWithBtn );
	toolsLyt->addWidget( moveToBtn );
	toolsLyt->addWidget( deleteBtn );
	toolsLyt->addWidget( propsBtn );
	toolsLyt->addWidget( permsBtn );
	toolsLyt->addStretch( 0 );

	setFixedHeight( 24 );
	setLayout( toolsLyt );
};
