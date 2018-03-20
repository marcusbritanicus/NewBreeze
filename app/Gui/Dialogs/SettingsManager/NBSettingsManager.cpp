/*
	*
	* NBSettingsManager.cpp - NewBreeze Settings
	*
*/

#include "NBSettingsManager.hpp"
#include "NBSPluginsWidget.hpp"

NBSettingsManager::NBSettingsManager( QWidget *parent ) : NBDialog( parent ) {

	createGUI();
	setWindowProperties();
};

void NBSettingsManager::createGUI() {

	// Widget and Layout Base
	QVBoxLayout *guiLyt = new QVBoxLayout();
	guiLyt->addLayout( createBodyLayout() );
	guiLyt->addWidget( Separator::horizontal() );
	guiLyt->addLayout( createFooterLayout() );

	setLayout( guiLyt );
};

QHBoxLayout* NBSettingsManager::createBodyLayout() {

	QHBoxLayout *splitLyt = new QHBoxLayout();
	widgetLyt = new QStackedLayout();

	chooserWidget = new NBSSideBar( this );
	connect( chooserWidget, SIGNAL( loadSettingsCategory( int ) ), this, SLOT( settingCategoryChosen( int ) ) );

	QScrollArea *genScroll = new QScrollArea();
	genScroll->setFrameShape( QFrame::NoFrame );
	genScroll->setWidgetResizable( true );

	NBSGeneralWidget *generalWidget = new NBSGeneralWidget( this );
	genScroll->setWidget( generalWidget );

	iconWidget = new NBIconThemeWidget( this );
	styleWidget = new NBSStyleWidget( this );
	keysWidget = new NBKeyBindingsWidget( this );

	widgetLyt->addWidget( genScroll );
	widgetLyt->addWidget( iconWidget );
	widgetLyt->addWidget( styleWidget );
	widgetLyt->addWidget( new NBSPluginsWidget( this ) );
	widgetLyt->addWidget( keysWidget );

	splitLyt->addWidget( chooserWidget );
	splitLyt->addWidget( Separator::vertical( this ) );
	splitLyt->addLayout( widgetLyt );

	return splitLyt;
};

QHBoxLayout* NBSettingsManager::createFooterLayout() {

	QHBoxLayout *btnLyt = new QHBoxLayout();

	NBButton *closeBtn = new NBButton( QIcon( ":/icons/close.png" ), "&Close", this );

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( closeBtn );
	btnLyt->addStretch( 0 );

	connect( closeBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	return btnLyt;
};

void NBSettingsManager::settingCategoryChosen( int row ) {

	widgetLyt->setCurrentIndex( row );
};

void NBSettingsManager::keyPressEvent( QKeyEvent *kEvent ) {

	if ( kEvent->key() == Qt::Key_Escape )
		close();

	else
		QWidget::keyPressEvent( kEvent );
};

void NBSettingsManager::setWindowProperties() {

	setDialogTitle( tr( "NewBreeze - Settings" ) );
	setDialogIcon( QIcon( ":/icons/openWith.png" ) );

	setWindowModality( Qt::ApplicationModal );

	setFixedWidth( 810 );
	setMinimumHeight( 550 );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 810 ) / 2 );
	int vpos = ( int )( ( dw.height() - 480 ) / 2 );
	move( hpos, vpos );
};
