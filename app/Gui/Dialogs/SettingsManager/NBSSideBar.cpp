/*
	*
	* NBSSideBar.hpp - SideBar for Settings Dialog
	*
*/

#include "NBSSideBar.hpp"

NBSSideBar::NBSSideBar( QWidget *parent ) : QWidget( parent ) {

	setFixedWidth( 81 );

	QToolButton *generalBtn = new QToolButton();
	generalBtn->setFixedSize( 72, 72 );
	generalBtn->setIconSize( QSize( 36, 36 ) );
	generalBtn->setIcon( QIcon( ":/icons/newbreeze.png" ) );
	generalBtn->setText( "General" );
	generalBtn->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	generalBtn->setCheckable( true );
	generalBtn->setFocusPolicy( Qt::NoFocus );
	generalBtn->setShortcut( tr( "Ctrl+1" ) );

	QToolButton *iconsBtn = new QToolButton();
	iconsBtn->setFixedSize( 72, 72 );
	iconsBtn->setIconSize( QSize( 36, 36 ) );
	iconsBtn->setIcon( QIcon( ":/icons/iconthemes.png" ) );
	iconsBtn->setText( "Icons" );
	iconsBtn->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	iconsBtn->setCheckable( true );
	iconsBtn->setFocusPolicy( Qt::NoFocus );
	iconsBtn->setShortcut( tr( "Ctrl+2" ) );

	QToolButton *styleBtn = new QToolButton();
	styleBtn->setFixedSize( 72, 72 );
	styleBtn->setIconSize( QSize( 36, 36 ) );
	styleBtn->setIcon( QIcon( ":/icons/appearance.png" ) );
	styleBtn->setText( "Style" );
	styleBtn->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	styleBtn->setCheckable( true );
	styleBtn->setFocusPolicy( Qt::NoFocus );
	styleBtn->setShortcut( tr( "Ctrl+3" ) );

	QToolButton *pluginsBtn = new QToolButton();
	pluginsBtn->setFixedSize( 72, 72 );
	pluginsBtn->setIconSize( QSize( 36, 36 ) );
	pluginsBtn->setIcon( QIcon( ":/icons/plugins.png" ) );
	pluginsBtn->setText( "Plugins" );
	pluginsBtn->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	pluginsBtn->setCheckable( true );
	pluginsBtn->setFocusPolicy( Qt::NoFocus );
	pluginsBtn->setShortcut( tr( "Ctrl+4" ) );

	QToolButton *keyBindingsBtn = new QToolButton();
	keyBindingsBtn->setFixedSize( 72, 72 );
	keyBindingsBtn->setIconSize( QSize( 36, 36 ) );
	keyBindingsBtn->setIcon( QIcon( ":/icons/keybindings.png" ) );
	keyBindingsBtn->setText( "Key Bindings" );
	keyBindingsBtn->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	keyBindingsBtn->setCheckable( true );
	keyBindingsBtn->setFocusPolicy( Qt::NoFocus );
	keyBindingsBtn->setShortcut( tr( "Ctrl+5" ) );

	QToolButton *terminalBtn = new QToolButton();
	terminalBtn->setFixedSize( 72, 72 );
	terminalBtn->setIconSize( QSize( 36, 36 ) );
	terminalBtn->setIcon( QIcon( ":/icons/vte.png" ) );
	terminalBtn->setText( "Terminal" );
	terminalBtn->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	terminalBtn->setCheckable( true );
	terminalBtn->setFocusPolicy( Qt::NoFocus );
	terminalBtn->setShortcut( tr( "Ctrl+6" ) );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addWidget( generalBtn );
	baseLyt->addWidget( iconsBtn );
	baseLyt->addWidget( styleBtn );
	baseLyt->addWidget( pluginsBtn );
	baseLyt->addWidget( keyBindingsBtn );
	baseLyt->addWidget( terminalBtn );

	setLayout( baseLyt );

	QButtonGroup *btnGrp = new QButtonGroup();
	btnGrp->setExclusive( true );
	btnGrp->addButton( generalBtn, 0 );
	btnGrp->addButton( iconsBtn, 1 );
	btnGrp->addButton( styleBtn, 2 );
	btnGrp->addButton( pluginsBtn, 3 );
	btnGrp->addButton( keyBindingsBtn, 4 );
	btnGrp->addButton( terminalBtn, 5 );

	connect( btnGrp, SIGNAL( buttonClicked( int ) ), this, SIGNAL( loadSettingsCategory( int ) ) );

	generalBtn->setChecked( true );
};
