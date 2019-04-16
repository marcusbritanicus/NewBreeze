/*
	*
	* NBPSideBar.cpp - Properties of the selected object
	*
*/

#include "NBPSideBar.hpp"

static QString btnStyleB = QString(
	"NBPSideBarButton {"
		"border: 1px solid gray; "
		"border-top-left-radius: 3px; "
		"border-top-right-radius: 3px; "
		"color: black; "
	"} "
	"NBPSideBarButton:checked { "
		"background: darkgray; "
	"}"
);

static QString btnStyleM = QString(
	"NBPSideBarButton {"
		"border: 1px solid gray; "
		"border-top: none; "
		"color: black; "
	"} "
	"NBPSideBarButton:checked { "
		"background: darkgray; "
	"}"
);

static QString btnStyleE = QString(
	"NBPSideBarButton {"
		"border: 1px solid gray; "
		"border-top: none; "
		"border-bottom-left-radius: 3px; "
		"border-bottom-right-radius: 3px; "
		"color: black; "
	"} "
	"NBPSideBarButton:checked { "
		"background: darkgray; "
	"}"
);

NBPSideBarButton::NBPSideBarButton( QWidget *parent ) : QToolButton( parent ) {

	setFixedSize( QSize( 64, 64 ) );
	setIconSize( QSize( 32, 32 ) );
	setToolButtonStyle( Qt::ToolButtonIconOnly );
	setCheckable( true );
	setChecked( false );
	setFocusPolicy( Qt::NoFocus );
};

NBPSideBar::NBPSideBar( QWidget *parent ) : QWidget( parent ) {

	createGUI();
};

void NBPSideBar::setChecked( int id ) {

	qobject_cast<NBPSideBarButton *>( btnGrp->button( id ) )->setChecked( true );
};

int NBPSideBar::checked() {

	return btnGrp->checkedId();
};

void NBPSideBar::createGUI() {

	/* Properties */
	propsBtn = new NBPSideBarButton( this );
	propsBtn->setIcon( QIcon( ":/icons/props.png" ) );
	propsBtn->setToolTip( "Properties" );
	propsBtn->setStyleSheet( btnStyleB );

	/* Permissions */
	permsBtn = new NBPSideBarButton( this );
	permsBtn->setIcon( QIcon( ":/icons/perms.png" ) );
	permsBtn->setToolTip( "Permissions" );
	permsBtn->setStyleSheet( btnStyleM );

	/* Defaults */
	defaultBtn = new NBPSideBarButton( this );
	defaultBtn->setIcon( QIcon( ":/icons/star.png" ) );
	defaultBtn->setToolTip( "Defaults" );
	defaultBtn->setStyleSheet( btnStyleM );

	/* Preview */
	previewBtn = new NBPSideBarButton( this );
	previewBtn->setIcon( QIcon( ":/icons/peek.png" ) );
	previewBtn->setToolTip( "Preview" );
	previewBtn->setStyleSheet( btnStyleM );

	/* Details */
	detailsBtn = new NBPSideBarButton( this );
	detailsBtn->setIcon( QIcon( ":/icons/info.png" ) );
	detailsBtn->setToolTip( "Details" );
	detailsBtn->setStyleSheet( btnStyleE );

	btnGrp = new QButtonGroup();
	btnGrp->addButton( propsBtn, 0 );
	btnGrp->addButton( permsBtn, 1 );
	btnGrp->addButton( defaultBtn, 2 );
	btnGrp->addButton( previewBtn, 3 );
	btnGrp->addButton( detailsBtn, 4 );
	btnGrp->setExclusive( true );

	setContentsMargins( QMargins() );

	QVBoxLayout *btnLyt = new QVBoxLayout();
	btnLyt->setContentsMargins( QMargins() );
	btnLyt->setSpacing( 0 );

	btnLyt->addStretch();
	btnLyt->addWidget( propsBtn );
	btnLyt->addWidget( permsBtn );
	btnLyt->addWidget( defaultBtn );
	btnLyt->addWidget( previewBtn );
	btnLyt->addWidget( detailsBtn );
	btnLyt->addStretch();

	setLayout( btnLyt );

	connect( propsBtn, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
	connect( permsBtn, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
	connect( defaultBtn, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
	connect( previewBtn, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
	connect( detailsBtn, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
};

void NBPSideBar::buttonClicked() {

	NBPSideBarButton *btn = qobject_cast<NBPSideBarButton*>( sender() );
	emit clicked( btnGrp->id( btn ) );
};
