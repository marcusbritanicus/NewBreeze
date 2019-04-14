/*
	*
	* NBSGeneralWidget.cpp - General Settings Widget for NewBreeze
	*
*/

#include "NBSGeneralWidget.hpp"
#include "NBMessageDialog.hpp"
#include "NBFunctions.hpp"

NBSGeneralWidget::NBSGeneralWidget( QWidget *parent ) : QWidget( parent ) {

	/* Grouping */

	perFolderEnabled = new QGroupBox( "&Enabled per-folder view", this );
	perFolderEnabled->setCheckable( true );
	perFolderEnabled->setChecked( Settings->value( "PerFolderViews", NBSettings::GlobalScope ) );
	connect( perFolderEnabled, SIGNAL( toggled( bool ) ), this, SLOT( handlePerFolderChanged( bool ) ) );

	defaultViewModeCB = new QComboBox( this );
	defaultViewModeCB->addItems( QStringList() << "Icons" << "Tiles" << "Details" );

	iconSizeL = new QLabel( this );

	defaultIconSizeS = new QSlider( Qt::Horizontal, this );
	defaultIconSizeS->setTickPosition( QSlider::TicksBelow );
	defaultIconSizeS->setRange( 16, 128 );
	defaultIconSizeS->setTickInterval( 16 );
	defaultIconSizeS->setSingleStep( 16 );

	if ( ( QString )Settings->value( "View/ViewMode", NBSettings::GlobalScope ) == "Icons" ) {
		defaultViewModeCB->setCurrentIndex( 0 );
		iconSizeL->setText( QString( "%1 px" ).arg( ( ( QSize )Settings->value( "View/IconsImageSize", NBSettings::GlobalScope ) ).width() ) );
		defaultIconSizeS->setValue( QSize( Settings->value( "View/IconsImageSize", NBSettings::GlobalScope ) ).width() );
	}

	else if ( ( QString )Settings->value( "View/ViewMode", NBSettings::GlobalScope ) == "Tiles" ) {
		defaultViewModeCB->setCurrentIndex( 1 );
		iconSizeL->setText( QString( "%1 px" ).arg( ( ( QSize )Settings->value( "View/TilesImageSize", NBSettings::GlobalScope ) ).width() ) );
		defaultIconSizeS->setValue( QSize( Settings->value( "View/TilesImageSize", NBSettings::GlobalScope ) ).width() );
	}

	else {
		defaultViewModeCB->setCurrentIndex( 2 );
		iconSizeL->setText( QString( "%1 px" ).arg( ( ( QSize )Settings->value( "View/DetailsImageSize", NBSettings::GlobalScope ) ).width() ) );
		defaultIconSizeS->setValue( QSize( Settings->value( "View/DetailsImageSize", NBSettings::GlobalScope ) ).width() );
	}

	connect( defaultViewModeCB, SIGNAL( currentIndexChanged( int ) ), this, SLOT( handleViewModeChanged( int ) ) );
	connect( defaultIconSizeS, SIGNAL( valueChanged( int ) ), this, SLOT( handleIconSizeChanged( int ) ) );

	defaultGroupingCB = new QCheckBox( "&Group files and folders by default", this );
	defaultGroupingCB->setChecked( Settings->value( "Grouping", NBSettings::GlobalScope ) );
	connect( defaultGroupingCB, SIGNAL( toggled( bool ) ), this, SLOT( handleGroupingChanged( bool ) ) );

	defaultSortCaseCB = new QCheckBox( "Enable c&ase sensitive file and folder sorting", this );
	defaultSortCaseCB->setChecked( Settings->value( "SortCase", NBSettings::GlobalScope ) );
	connect( defaultSortCaseCB, SIGNAL( toggled( bool ) ), this, SLOT( handleSortCaseChanged( bool ) ) );

	defaultSortClmnCB = new QComboBox( this );
	defaultSortClmnCB->addItems( QStringList() << "Name" << "Size" << "Type" << "Date" );
	defaultSortClmnCB->setCurrentIndex( ( int )Settings->value( "SortColumn", NBSettings::GlobalScope ) == 3 ? 4 : ( int )Settings->value( "SortColumn", NBSettings::GlobalScope ) );
	connect( defaultSortClmnCB, SIGNAL( currentIndexChanged( int ) ), this, SLOT( handleSortColumnChanged( int ) ) );

	/* Side Panel */
	showSidePanelGB = new QGroupBox( "Show SidePanel", this );
	showSidePanelGB->setCheckable( true );
	showSidePanelGB->setChecked( Settings->value( "SidePanel", NBSettings::GlobalScope ) );
	connect( showSidePanelGB, SIGNAL( toggled( bool ) ), this, SLOT( handleShowSidePanelToggled( bool ) ) );

	sideBarRB = new QRadioButton( "Use the modern side &bar" );
	connect( sideBarRB, SIGNAL( clicked() ), this, SLOT( handleSidePanelChoice() ) );
	sideBarRB->setChecked( ( int )Settings->value( "SidePanelType", NBSettings::GlobalScope ) == 0 );

	sidePanelRB = new QRadioButton( "Use the classic side &panel" );
	connect( sidePanelRB, SIGNAL( clicked() ), this, SLOT( handleSidePanelChoice() ) );
	sidePanelRB->setChecked( ( int )Settings->value( "SidePanelType", NBSettings::GlobalScope ) == 1 );

	QVBoxLayout *sidePanelLyt = new QVBoxLayout();
	sidePanelLyt->addWidget( sideBarRB );
	sidePanelLyt->addWidget( sidePanelRB );

	showSidePanelGB->setLayout( sidePanelLyt );

	/* Previews */

	filePreviewGB = new QGroupBox( "&Show Image Previews" );
	filePreviewGB->setCheckable( true );
	filePreviewGB->setChecked( Settings->value( "View/FilePreviews", NBSettings::GlobalScope ) );
	connect( filePreviewGB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	imagePreviewCB = new QCheckBox( "&Image files (.jpg .png .gif .svg .bmp etc)", this );
	imagePreviewCB->setChecked( Settings->value( "View/ImagePreview", NBSettings::GlobalScope ) );
	connect( imagePreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	pdfPreviewCB = new QCheckBox( "PDF files", this );
	pdfPreviewCB->setChecked( Settings->value( "View/PdfPreview", NBSettings::GlobalScope ) );
	connect( pdfPreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	djvuPreviewCB = new QCheckBox( "D&jVu files", this );
	djvuPreviewCB->setChecked( Settings->value( "View/DjVuPreview", NBSettings::GlobalScope ) );
	connect( djvuPreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	epubPreviewCB = new QCheckBox( "eP&ub files", this );
	epubPreviewCB->setChecked( Settings->value( "View/ePubPreview", NBSettings::GlobalScope ) );
	connect( epubPreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	odfPreviewCB = new QCheckBox( "&ODF files (.odt .odg .odp .ods)", this );
	odfPreviewCB->setChecked( Settings->value( "View/OdfPreview", NBSettings::GlobalScope ) );
	connect( odfPreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	videoPreviewCB = new QCheckBox( "Video files (.mp4 .avi .webm .mkv .flv etc)", this );
	videoPreviewCB->setChecked( Settings->value( "View/VideoPreview", NBSettings::GlobalScope ) );
	connect( videoPreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	QVBoxLayout *previewLyt = new QVBoxLayout();
	previewLyt->addWidget( imagePreviewCB );
	previewLyt->addWidget( pdfPreviewCB );
	previewLyt->addWidget( djvuPreviewCB );
	previewLyt->addWidget( epubPreviewCB );
	previewLyt->addWidget( odfPreviewCB );
	previewLyt->addWidget( videoPreviewCB );

	filePreviewGB->setLayout( previewLyt );

	/* Others */

	filterFoldersCB = new QCheckBox( "&Filter folder and files", this );
	filterFoldersCB->setChecked( Settings->value( "FilterFolders", NBSettings::GlobalScope ) );
	connect( filterFoldersCB, SIGNAL( toggled( bool ) ), this, SLOT( handleFilterDirsChanged( bool ) ) );

	QHBoxLayout *comboBoxLyt1 = new QHBoxLayout();
	QLabel *lbl1 = new QLabel( "Choose the default &view mode:" );
	lbl1->setBuddy( defaultViewModeCB );
	comboBoxLyt1->addWidget( lbl1 );
	comboBoxLyt1->addWidget( defaultViewModeCB );

	QHBoxLayout *comboBoxLyt2 = new QHBoxLayout();

	QLabel *lbl2 = new QLabel( "Choose the default icon &size:" );
	lbl2->setBuddy( defaultIconSizeS );

	comboBoxLyt2->addWidget( lbl2 );
	comboBoxLyt2->addStretch();
	comboBoxLyt2->addWidget( defaultIconSizeS );
	comboBoxLyt2->addWidget( iconSizeL );

	QHBoxLayout *comboBoxLyt3 = new QHBoxLayout();
	QLabel *lbl3 = new QLabel( "&Sort file and folders by:" );
	lbl3->setBuddy( defaultSortClmnCB );
	comboBoxLyt3->addWidget( lbl3 );
	comboBoxLyt3->addWidget( defaultSortClmnCB );

	showTrayIconCB = new QCheckBox( "&Minimize to tray" );
	showTrayIconCB->setChecked( Settings->value( "TrayIcon", NBSettings::GlobalScope ) );
	connect( showTrayIconCB, SIGNAL( toggled( bool ) ), this, SLOT( handleTrayIconChanged( bool ) ) );

	openWithCB = new QCheckBox( "Open with SuperStart when NewBreeze starts?", this );
	openWithCB->setChecked( Settings->value( "SuperStart", NBSettings::GlobalScope ) );
	connect( openWithCB, SIGNAL( toggled( bool ) ), this, SLOT( handleOpenWithToggled() ) );

	extendedIOCB = new QCheckBox( "Enable exten&ded IO" );
	extendedIOCB->setChecked( Settings->value( "ExtendedIO", NBSettings::GlobalScope ) );
	extendedIOCB->setToolTip(
		"Extended IO is a replacement for 'Copy/Move to' menu option. When enabled, "
		"a dialog opens where you can choose the target directory to which you want "
		"to copy/move."
	);
	connect( extendedIOCB, SIGNAL( toggled( bool ) ), this, SLOT( handleExtendedIOChanged( bool ) ) );

	paintOverlayCB = new QCheckBox( "Enable IconOverlay" );
	paintOverlayCB->setChecked( Settings->value( "View/PaintOverlay", NBSettings::GlobalScope ) );
	paintOverlayCB->setToolTip(
		"To quickly open or access the menu without double-click or right-click, enable this option."
	);
	connect( paintOverlayCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePaintOverlayChanged( bool ) ) );

	logDebugCB = new QCheckBox( "Enable logging of debug messages" );
	logDebugCB->setChecked( Settings->value( "LogDebug", NBSettings::GlobalScope ) );
	logDebugCB->setToolTip( "Write the debug messages to the log file" );
	connect( logDebugCB, SIGNAL( toggled( bool ) ), this, SLOT( handleLogDebugChanged( bool ) ) );

	enableAutoMountCB = new QCheckBox( "Enable auto mounting of volumes" );
	enableAutoMountCB->setChecked( Settings->value( "AutoMount", NBSettings::GlobalScope ) );
	enableAutoMountCB->setToolTip( "Write the debug messages to the log file" );
	connect( enableAutoMountCB, SIGNAL( toggled( bool ) ), this, SLOT( handleAutoMountChanged( bool ) ) );

	QVBoxLayout *grpLyt = new QVBoxLayout( this );
	grpLyt->addLayout( comboBoxLyt1 );
	grpLyt->addLayout( comboBoxLyt2 );
	grpLyt->addWidget( defaultGroupingCB );
	grpLyt->addWidget( defaultSortCaseCB );
	grpLyt->addLayout( comboBoxLyt3 );

	perFolderEnabled->setLayout( grpLyt );

	QVBoxLayout *otherGBLyt = new QVBoxLayout();
	otherGBLyt->addWidget( filterFoldersCB );
	otherGBLyt->addWidget( showTrayIconCB );
	otherGBLyt->addWidget( openWithCB );
	otherGBLyt->addWidget( extendedIOCB );
	otherGBLyt->addWidget( paintOverlayCB );
	otherGBLyt->addWidget( logDebugCB );
	otherGBLyt->addWidget( enableAutoMountCB );

	QGroupBox *otherOptionsGB = new QGroupBox( "Other Options", this );
	otherOptionsGB->setLayout( otherGBLyt );

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->addWidget( perFolderEnabled );
	lyt->addWidget( showSidePanelGB );
	lyt->addWidget( filePreviewGB );
	lyt->addWidget( otherOptionsGB );
	lyt->addStretch();

	setLayout( lyt );
};

void NBSGeneralWidget::handlePerFolderChanged( bool perFolder ) {

	Settings->setValue( "PerFolderViews", perFolder, NBSettings::GlobalScope );
};

void NBSGeneralWidget::handleViewModeChanged( int ) {

	Settings->setValue( "View/ViewMode", defaultViewModeCB->currentText(), NBSettings::GlobalScope );

	if ( ( QString )Settings->value( "View/ViewMode", NBSettings::GlobalScope ) == "Icons" )
		defaultIconSizeS->setValue( QSize( Settings->value( "View/IconsImageSize", NBSettings::GlobalScope ) ).width() );

	else if ( ( QString )Settings->value( "View/ViewMode", NBSettings::GlobalScope ) == "Tiles" )
		defaultIconSizeS->setValue( ( ( QSize )Settings->value( "View/TilesImageSize", NBSettings::GlobalScope ) ).width() );

	else
		defaultIconSizeS->setValue( ( ( QSize )Settings->value( "View/DetailsImageSize", NBSettings::GlobalScope ) ).width() );
};

void NBSGeneralWidget::handleIconSizeChanged( int ) {

	int iconSize = defaultIconSizeS->value();
	if ( ( QString )Settings->value( "View/ViewMode", NBSettings::GlobalScope ) == "Icons" )
		Settings->setValue( "View/IconsImageSize", QSize( iconSize, iconSize ), NBSettings::GlobalScope );

	else if ( ( QString )Settings->value( "View/ViewMode", NBSettings::GlobalScope ) == "Tiles" )
		Settings->setValue( "View/TilesImageSize", QSize( iconSize, iconSize ), NBSettings::GlobalScope );

	else
		Settings->setValue( "View/DetailsImageSize", QSize( iconSize, iconSize ), NBSettings::GlobalScope );

	iconSizeL->setText( QString( "%1 px" ).arg( iconSize ) );
};

void NBSGeneralWidget::handleGroupingChanged( bool grouping ) {

	Settings->setValue( "Grouping", grouping, NBSettings::GlobalScope );
};

void NBSGeneralWidget::handleSortCaseChanged( bool sortCase ) {

	Settings->setValue( "SortCase", sortCase, NBSettings::GlobalScope );
};

void NBSGeneralWidget::handleSortColumnChanged( int sortColumn ) {

	switch( sortColumn ) {
		case 0: {
			Settings->setValue( "SortColumn", 0, NBSettings::GlobalScope );
			break;
		}

		case 1:{
			Settings->setValue( "SortColumn", 1, NBSettings::GlobalScope );
			break;
		}

		case 2:{
			Settings->setValue( "SortColumn", 2, NBSettings::GlobalScope );
			break;
		}

		case 3:{
			Settings->setValue( "SortColumn", 4, NBSettings::GlobalScope );
			break;
		}

		default: {
			if ( Settings->value( "Grouping", NBSettings::GlobalScope ) )
				Settings->setValue( "SortColumn", 2, NBSettings::GlobalScope );

			else
				Settings->setValue( "SortColumn", 0, NBSettings::GlobalScope );

			break;
		}
	}
};

void NBSGeneralWidget::handleShowSidePanelToggled( bool showPanel ) {

	Settings->setValue( "SidePanel", showPanel, NBSettings::GlobalScope );
};

void NBSGeneralWidget::handleSidePanelChoice() {
	/* Modern: 0; Classic: 1 */

	if ( qobject_cast<QRadioButton*>( sender() ) == sideBarRB )
		Settings->setValue( "SidePanelType", 0, NBSettings::GlobalScope );

	else
		Settings->setValue( "SidePanelType", 1, NBSettings::GlobalScope );
};

void NBSGeneralWidget::handlePreviewsChanged() {

	Settings->setValue( "View/FilePreviews", filePreviewGB->isChecked(), NBSettings::GlobalScope );
	Settings->setValue( "View/ImagePreview", imagePreviewCB->isChecked(), NBSettings::GlobalScope );
	Settings->setValue( "View/PdfPreview", pdfPreviewCB->isChecked(), NBSettings::GlobalScope );
	Settings->setValue( "View/DjVuPreview", djvuPreviewCB->isChecked(), NBSettings::GlobalScope );
	Settings->setValue( "View/OdfPreview", odfPreviewCB->isChecked(), NBSettings::GlobalScope );
	Settings->setValue( "View/VideoPreview", videoPreviewCB->isChecked(), NBSettings::GlobalScope );
};

void NBSGeneralWidget::handleFilterDirsChanged( bool filterFolders ) {

	Settings->setValue( "FilterFolders", filterFolders, NBSettings::GlobalScope );
};

void NBSGeneralWidget::handleOpenWithToggled() {

	if ( qobject_cast<QCheckBox*>( sender() ) == openWithCB ) {
		if ( openWithCB->isChecked() )
			Settings->setValue( "SuperStart", true, NBSettings::GlobalScope );

		else
			Settings->setValue( "SuperStart", false, NBSettings::GlobalScope );
	}
};

void NBSGeneralWidget::handleTrayIconChanged( bool value ) {

	Settings->setValue( "TrayIcon", value, NBSettings::GlobalScope );
};

void NBSGeneralWidget::handleExtendedIOChanged( bool enabled ) {

	Settings->setValue( "ExtendedIO", enabled, NBSettings::GlobalScope );
};

void NBSGeneralWidget::handlePaintOverlayChanged( bool enabled ) {

	Settings->setValue( "View/PaintOverlay", enabled, NBSettings::GlobalScope );
};

void NBSGeneralWidget::handleLogDebugChanged( bool enabled ) {

	Settings->setValue( "LogDebug", enabled, NBSettings::GlobalScope );
};

void NBSGeneralWidget::handleAutoMountChanged( bool enabled ) {

	Settings->setValue( "AutoMount", enabled, NBSettings::GlobalScope );
};
