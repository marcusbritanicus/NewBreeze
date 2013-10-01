/*
	*
	* NBSettingsManager.cpp - NewBreeze Settings
	*
*/

#include <NBSettingsManager.hpp>
#include <NBTools.hpp>

NBSettingsManager::NBSettingsManager() : NBDialog() {

	createGUI();
	setWindowProperties();
};

void NBSettingsManager::createGUI() {

	// Widget and Layout Base
	QVBoxLayout *guiLyt = new QVBoxLayout();
	if ( not Settings.General.NativeTitleBar )
		guiLyt->addWidget( Separator::horizontal() );
	guiLyt->addLayout( createBodyLayout() );
	guiLyt->addWidget( Separator::horizontal() );
	guiLyt->addLayout( createFooterLayout() );

	setLayout( guiLyt );
};

QHBoxLayout* NBSettingsManager::createBodyLayout() {

	QHBoxLayout *splitLyt = new QHBoxLayout();
	widgetLyt = new QStackedLayout();

	chooserWidget = new NBSettingChooserWidget();
	chooserWidget->addItem( new NBSCWidgetItem( QIcon( ":/icons/appearance.png" ), "Appearance" ) );
	chooserWidget->addItem( new NBSCWidgetItem( QIcon( ":/icons/iconthemes.png" ), "Icons" ) );
	chooserWidget->addItem( new NBSCWidgetItem( QIcon( ":/icons/keybindings.png" ), "KeyBindings" ) );
	chooserWidget->item( 0 )->setSelected( true );

	connect( chooserWidget, SIGNAL( currentRowChanged( int ) ), this, SLOT( settingCategoryChosen( int ) ) );

	viewWidget = new NBViewsWidget();
	iconWidget = new NBIconThemeWidget();
	keysWidget = new NBKeyBindingsWidget();

	widgetLyt->addWidget( viewWidget );
	widgetLyt->addWidget( iconWidget );
	widgetLyt->addWidget( keysWidget );

	splitLyt->addWidget( chooserWidget );
	splitLyt->addWidget( Separator::vertical() );
	splitLyt->addLayout( widgetLyt );

	return splitLyt;
};

QHBoxLayout* NBSettingsManager::createFooterLayout() {

	QHBoxLayout *btnLyt = new QHBoxLayout();

	QPushButton *closeBtn = new QPushButton();
	closeBtn->setObjectName( "cancelBtn" );
	closeBtn->setText( "&Close" );
	closeBtn->setIcon( QIcon( ":/icons/close.png" ) );

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
	setMinimumHeight( 480 );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 810 ) / 2 );
	int vpos = ( int )( ( dw.height() - 480 ) / 2 );
	move( hpos, vpos );
};

NBViewsWidget::NBViewsWidget() {

	createGUI();
};

void NBViewsWidget::createGUI() {

	QGridLayout *themeLyt = new QGridLayout();

	tlRB = new QRadioButton( "Light Transparent" );
	connect( tlRB, SIGNAL( clicked() ), this, SLOT( handleThemeChanged() ) );

	tdRB = new QRadioButton( "Dark Transparent" );
	connect( tdRB, SIGNAL( clicked() ), this, SLOT( handleThemeChanged() ) );

	dbRB = new QRadioButton( "Dull Black" );
	connect( dbRB, SIGNAL( clicked() ), this, SLOT( handleThemeChanged() ) );

	ntRB = new QRadioButton( "Light Gray" );
	connect( ntRB, SIGNAL( clicked() ), this, SLOT( handleThemeChanged() ) );

	imageLbl = new QLabel();
	imageLbl->setFixedSize( 540, 290 );
	imageLbl->setAlignment( Qt::AlignCenter );

	nativeTitleBarCB = new QCheckBox( "&Use native title bar" );
	nativeTitleBarCB->setChecked( Settings.General.NativeTitleBar );
	connect( nativeTitleBarCB, SIGNAL( toggled( bool ) ), this, SLOT( handleNativeTitleBarToggle( bool ) ) );

	maxIOJobsSB = new QSpinBox();
	maxIOJobsSB->setRange( 2, 100 );
	maxIOJobsSB->setSingleStep( 1 );
	maxIOJobsSB->setValue( Settings.General.MaxIOJobs );
	connect( maxIOJobsSB, SIGNAL( valueChanged( int ) ), this, SLOT( handleMaxIOJobsChanged( int ) ) );

	if ( Settings.General.Style == QString( "TransLight" ) ) {
		tlRB->setChecked( true );
		imageLbl->setPixmap( QPixmap( ":/icons/TransLight.png" ).scaled( 540, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	}

	else if ( Settings.General.Style == QString( "TransDark" ) ) {
		tdRB->setChecked( true );
		imageLbl->setPixmap( QPixmap( ":/icons/TransDark.png" ).scaled( 540, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	}

	else if ( Settings.General.Style == QString( "DullBlack" ) ) {
		dbRB->setChecked( true );
		imageLbl->setPixmap( QPixmap( ":/icons/DullBlack.png" ).scaled( 540, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	}

	else {
		ntRB->setChecked( true );
		imageLbl->setPixmap( QPixmap( ":/icons/LightGray.png" ).scaled( 540, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	}

	themeLyt->addWidget( tlRB, 0, 0 );
	themeLyt->addWidget( tdRB, 0, 1 );
	themeLyt->addWidget( dbRB, 0, 2 );
	themeLyt->addWidget( ntRB, 0, 3 );
	themeLyt->addWidget( imageLbl, 1, 0, 1, 4, Qt::AlignCenter );
	themeLyt->addWidget( nativeTitleBarCB, 2, 0, 1, 4, Qt::AlignLeft );
	themeLyt->addWidget( new QLabel( "Maximum number of cuncurrent IO Jobs:" ), 3, 0, 1, 3 );
	themeLyt->addWidget( maxIOJobsSB, 3, 3 );

	setLayout( themeLyt );
};

void NBViewsWidget::handleThemeChanged() {

	if ( tlRB->isChecked() ) {
		settings.setValue( "Style", QVariant( QString( "TransLight" ) ) );
		imageLbl->setPixmap( QPixmap( ":/icons/TransLight.png" ).scaled( 540, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
		Settings.General.Style = QString( "TransLight" );
	}

	else if ( tdRB->isChecked() ) {
		settings.setValue( "Style", QVariant( QString( "TransDark" ) ) );
		imageLbl->setPixmap( QPixmap( ":/icons/TransDark.png" ).scaled( 540, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
		Settings.General.Style = QString( "TransDark" );
	}

	else if ( dbRB->isChecked() ) {
		settings.setValue( "Style", QVariant( QString( "DullBlack" ) ) );
		imageLbl->setPixmap( QPixmap( ":/icons/DullBlack.png" ).scaled( 540, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
		Settings.General.Style = QString( "DullBlack" );
	}

	else {
		settings.setValue( "Style", QVariant( QString( "LightGray" ) ) );
		imageLbl->setPixmap( QPixmap( ":/icons/LightGray.png" ).scaled( 540, 290, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
		Settings.General.Style = QString( "LightGray" );
	}
};

void NBViewsWidget::handleNativeTitleBarToggle( bool useNative ) {

	settings.setValue( "NativeTitleBar", useNative );
	Settings.General.NativeTitleBar = useNative;
};

void NBViewsWidget::handleMaxIOJobsChanged( int value ) {

	settings.setValue( "MaxIOJobs", value );
	Settings.General.MaxIOJobs = value;
};

NBIconThemeWidget::NBIconThemeWidget() {

	createGUI();
};

void NBIconThemeWidget::createGUI() {

	iconThemesWidget = new NBIconThemeChooserWidget();
	folderViewWidget = new NBIconThemeViewerWidget();

	imagePreviewCB = new QCheckBox( "&Show Image Previews" );
	imagePreviewCB->setChecked( Settings.General.ImagePreviews );

	connect( iconThemesWidget, SIGNAL( reloadIcons() ), folderViewWidget, SLOT( loadIcons() ) );
	connect( imagePreviewCB, SIGNAL( stateChanged( int ) ), this, SLOT( handleCheckStateChanged( int ) ) );

	QVBoxLayout *wLyt = new QVBoxLayout();
	wLyt->addWidget( iconThemesWidget );
	wLyt->addWidget( folderViewWidget );
	wLyt->addWidget( imagePreviewCB );

	setLayout( wLyt );
};

void NBIconThemeWidget::setIconTheme() {

};

void NBIconThemeWidget::handleCheckStateChanged( int state ) {

	switch( state ) {
		case Qt::Unchecked :
			settings.setValue( "ImagePreviews", false );
			Settings.General.ImagePreviews = false;
			break;

		case Qt::PartiallyChecked :
		case Qt::Checked :
			settings.setValue( "ImagePreviews", true );
			Settings.General.ImagePreviews = true;
			break;
	}
}

NBKeyBindingsWidget::NBKeyBindingsWidget() {

	QScrollArea *scroller = new QScrollArea();
	scroller->setStyleSheet( scroller->styleSheet() + "\nQScrollArea { border: none; }" );
	scroller->setWidgetResizable( true );

	QGridLayout *keysLyt = new QGridLayout();
	keysLyt->setSpacing( 0 );

	keysLyt->addWidget( new QLabel( "AboutNB" ), 0, 0 );
	keysLyt->addWidget( new QLabel( "Toggle CrumbsBar" ), 1, 0 );
	keysLyt->addWidget( new QLabel( "ViewMode" ), 2, 0 );
	keysLyt->addWidget( new QLabel( "Add Custom Action" ), 3, 0 );
	keysLyt->addWidget( new QLabel( "Go Home" ), 4, 0 );
	keysLyt->addWidget( new QLabel( "Go Up" ), 5, 0 );
	keysLyt->addWidget( new QLabel( "Go Prev Dir" ), 6, 0 );
	keysLyt->addWidget( new QLabel( "Go Next Dir" ), 7, 0 );
	keysLyt->addWidget( new QLabel( "New Folder" ), 8, 0 );
	keysLyt->addWidget( new QLabel( "New File" ), 9, 0 );
	keysLyt->addWidget( new QLabel( "Peek" ), 10, 0 );
	keysLyt->addWidget( new QLabel( "Reload" ), 11, 0 );
	keysLyt->addWidget( new QLabel( "Toggle Hidden Files" ), 12, 0 );
	keysLyt->addWidget( new QLabel( "Select All" ), 13, 0 );
	keysLyt->addWidget( new QLabel( "Toggle SideBar" ), 14, 0 );
	keysLyt->addWidget( new QLabel( "Cut" ), 15, 0 );
	keysLyt->addWidget( new QLabel( "Copy" ), 16, 0 );
	keysLyt->addWidget( new QLabel( "Paste" ), 17, 0 );
	keysLyt->addWidget( new QLabel( "Rename" ), 18, 0 );
	keysLyt->addWidget( new QLabel( "Delete" ), 19, 0 );
	keysLyt->addWidget( new QLabel( "Trash" ), 20, 0 );
	keysLyt->addWidget( new QLabel( "Properties" ), 21, 0 );
	keysLyt->addWidget( new QLabel( "Terminal" ), 22, 0 );
	keysLyt->addWidget( new QLabel( "Show Inline Terminal" ), 23, 0 );
	keysLyt->addWidget( new QLabel( "Show NBInfo" ), 24, 0 );
	keysLyt->addWidget( new QLabel( "Settings" ), 25, 0 );
	keysLyt->addWidget( new QLabel( "Custom Actions" ), 26, 0 );
	keysLyt->addWidget( new QLabel( "Focus AddressBar" ), 27, 0 );
	keysLyt->addWidget( new QLabel( "New Window" ), 28, 0 );
	keysLyt->addWidget( new QLabel( "Add Bookmark" ), 29, 0 );
	keysLyt->addWidget( new QLabel( "Focus SearchBar" ), 30, 0 );
	keysLyt->addWidget( new QLabel( "Clear SearchBar" ), 31, 0 );

	keysLyt->addWidget( new NBShortcutsWidget( "AboutNB" ), 0, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "ToggleCrumbLE" ), 1, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "ViewMode" ), 2, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "AddCustomAction" ), 3, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "GoHome" ), 4, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "GoUp" ), 5, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "GoLeft" ), 6, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "GoRight" ), 7, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "NewFolder" ), 8, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "NewFile" ), 9, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Peek" ), 10, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Reload" ), 11, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "ToggleHidden" ), 12, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "SelectAll" ), 13, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "ToggleSideBar" ), 14, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Cut" ), 15, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Copy" ), 16, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Paste" ), 17, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Rename" ), 18, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Delete" ), 19, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Trash" ), 20, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Properties" ), 21, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Terminal" ), 22, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "InlineTerminal" ), 23, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "NBInfo" ), 24, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "Settings" ), 25, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "CustomActions" ), 26, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "FocusAddressBar" ), 27, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "NewWindow" ), 28, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "AddBookmark" ), 29, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "FocusSearchBar" ), 30, 1 );
	keysLyt->addWidget( new NBShortcutsWidget( "ClearSearchBar" ), 31, 1 );

	QWidget *scrollWidget = new QWidget();
	scrollWidget->setLayout( keysLyt );

	scroller->setWidget( scrollWidget );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->addWidget( scroller );

	setLayout( baseLyt );
};
