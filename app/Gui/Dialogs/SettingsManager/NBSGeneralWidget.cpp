/*
	*
	* NBSGeneralWidget.cpp - General Settings Widget for NewBreeze
	*
*/

#include "NBSGeneralWidget.hpp"
#include "NBMessageDialog.hpp"
#include "NBFunctions.hpp"

QSettings sett( "NewBreeze", "NewBreeze" );

NBSGeneralWidget::NBSGeneralWidget( QWidget *parent ) : QWidget( parent ) {

	/* Grouping */

	perFolderEnabled = new QGroupBox( "&Enabled per-folder view", this );
	perFolderEnabled->setCheckable( true );
	perFolderEnabled->setChecked( sett.value( "PerFolderViews" ).toBool() );
	connect( perFolderEnabled, SIGNAL( toggled( bool ) ), this, SLOT( handlePerFolderChanged( bool ) ) );

	defaultViewModeCB = new QComboBox( this );
	defaultViewModeCB->addItems( QStringList() << "Icons" << "Tiles" << "Details" );
	if ( sett.value( "ViewMode" ).toString().isEmpty() or sett.value( "ViewMode" ).toString() == "Icons" )
		defaultViewModeCB->setCurrentIndex( 0 );

	else if ( sett.value( "ViewMode" ).toString() == "Tiles" )
		defaultViewModeCB->setCurrentIndex( 1 );

	else
		defaultViewModeCB->setCurrentIndex( 2 );

	connect( defaultViewModeCB, SIGNAL( currentIndexChanged( int ) ), this, SLOT( handleViewModeChanged( int ) ) );

	iconSizeL = new QLabel( this );
	iconSizeL->setText( QString( "%1 px" ).arg( sett.value( "IconSize" ).toSize().width() ) );

	defaultIconSizeS = new QSlider( Qt::Horizontal, this );
	defaultIconSizeS->setTickPosition( QSlider::TicksBelow );
	defaultIconSizeS->setRange( 16, 128 );
	defaultIconSizeS->setTickInterval( 16 );
	defaultIconSizeS->setSingleStep( 16 );
	defaultIconSizeS->setValue( sett.value( "IconSize" ).toSize().width() );
	connect( defaultIconSizeS, SIGNAL( valueChanged( int ) ), this, SLOT( handleIconSizeChanged( int ) ) );

	defaultGroupingCB = new QCheckBox( "&Group files and folders by default", this );
	defaultGroupingCB->setChecked( sett.value( "Grouping" ).toBool() );
	connect( defaultGroupingCB, SIGNAL( toggled( bool ) ), this, SLOT( handleGroupingChanged( bool ) ) );

	defaultSortCaseCB = new QCheckBox( "Enable c&ase sensitive file and folder sorting", this );
	defaultSortCaseCB->setChecked( sett.value( "SortCase" ).toBool() );
	connect( defaultSortCaseCB, SIGNAL( toggled( bool ) ), this, SLOT( handleSortCaseChanged( bool ) ) );

	defaultSortClmnCB = new QComboBox( this );
	defaultSortClmnCB->addItems( QStringList() << "Name" << "Size" << "Type" << "Date" );
	defaultSortClmnCB->setCurrentIndex( sett.value( "SortColumn" ).toInt() == 3 ? 4 : sett.value( "SortColumn" ).toInt() );
	connect( defaultSortClmnCB, SIGNAL( currentIndexChanged( int ) ), this, SLOT( handleSortColumnChanged( int ) ) );

	/* Side Panel */
	showSidePanelGB = new QGroupBox( "Show SidePanel", this );
	showSidePanelGB->setCheckable( true );
	showSidePanelGB->setChecked( sett.value( "SidePanel" ).toBool() );
	connect( showSidePanelGB, SIGNAL( toggled( bool ) ), this, SLOT( handleShowSidePanelToggled( bool ) ) );

	sidePanelRB = new QRadioButton( "Use the modern side &panel" );
	connect( sidePanelRB, SIGNAL( clicked() ), this, SLOT( handleSidePanelChoice() ) );
	sidePanelRB->setChecked( sett.value( "SidePanelType" ).toInt() == 0 );

	sideBarRB = new QRadioButton( "Use the classic side &bar" );
	connect( sideBarRB, SIGNAL( clicked() ), this, SLOT( handleSidePanelChoice() ) );
	sideBarRB->setChecked( sett.value( "SidePanelType" ).toInt() == 1 );

	QVBoxLayout *sidePanelLyt = new QVBoxLayout();
	sidePanelLyt->addWidget( sidePanelRB );
	sidePanelLyt->addWidget( sideBarRB );

	showSidePanelGB->setLayout( sidePanelLyt );

	/* Previews */

	filePreviewGB = new QGroupBox( "&Show Image Previews" );
	filePreviewGB->setCheckable( true );
	filePreviewGB->setChecked( Settings->View.FilePreviews );
	connect( filePreviewGB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	imagePreviewCB = new QCheckBox( "&Image files (.jpg .png .gif .svg .bmp etc)", this );
	imagePreviewCB->setChecked( Settings->View.ImagePreview );
	connect( imagePreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	pdfPreviewCB = new QCheckBox( "PDF files", this );
	pdfPreviewCB->setChecked( Settings->View.PdfPreview );
	connect( pdfPreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	djvuPreviewCB = new QCheckBox( "D&jVu files", this );
	djvuPreviewCB->setChecked( Settings->View.DjVuPreview );
	connect( djvuPreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	odfPreviewCB = new QCheckBox( "&ODF files (.odt .odg .odp .ods)", this );
	odfPreviewCB->setChecked( Settings->View.OdfPreview );
	connect( odfPreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	videoPreviewCB = new QCheckBox( "Video files (.mp4 .avi .webm .mkv .flv etc)", this );
	videoPreviewCB->setChecked( Settings->View.VideoPreview );
	connect( videoPreviewCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePreviewsChanged() ) );

	QVBoxLayout *previewLyt = new QVBoxLayout();
	previewLyt->addWidget( imagePreviewCB );
	previewLyt->addWidget( pdfPreviewCB );
	previewLyt->addWidget( djvuPreviewCB );
	previewLyt->addWidget( odfPreviewCB );
	previewLyt->addWidget( videoPreviewCB );

	filePreviewGB->setLayout( previewLyt );

	/* Others */

	filterFoldersCB = new QCheckBox( "&Filter folder and files", this );
	filterFoldersCB->setChecked( sett.value( "FilterFolders" ).toBool() );
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
	showTrayIconCB->setChecked( Settings->General.TrayIcon );
	connect( showTrayIconCB, SIGNAL( toggled( bool ) ), this, SLOT( handleTrayIconChanged( bool ) ) );

	openWithCB = new QCheckBox( "Open with SuperStart when NewBreeze starts?", this );
	openWithCB->setChecked( sett.value( "SuperStart" ).toBool() );
	connect( openWithCB, SIGNAL( toggled( bool ) ), this, SLOT( handleOpenWithToggled() ) );

	extendedIOCB = new QCheckBox( "Enable exten&ded IO" );
	extendedIOCB->setChecked( Settings->General.ExtendedIO );
	extendedIOCB->setToolTip(
		"Extended IO is a replacement for 'Copy/Move to' menu option. When enabled, "
		"a dialog opens where you can choose the target directory to which you want "
		"to copy/move."
	);
	connect( extendedIOCB, SIGNAL( toggled( bool ) ), this, SLOT( handleExtendedIOChanged( bool ) ) );

	paintOverlayCB = new QCheckBox( "Enable IconOverlay" );
	paintOverlayCB->setChecked( Settings->View.PaintOverlay );
	paintOverlayCB->setToolTip(
		"To quickly open or access the menu without double-click or right-click, enable this option."
	);
	connect( paintOverlayCB, SIGNAL( toggled( bool ) ), this, SLOT( handlePaintOverlayChanged( bool ) ) );

	logDebugCB = new QCheckBox( "Enable logging of debug messages" );
	logDebugCB->setChecked( Settings->General.LogDebug );
	logDebugCB->setToolTip( "Write the debug messages to the log file" );
	connect( logDebugCB, SIGNAL( toggled( bool ) ), this, SLOT( handleLogDebugChanged( bool ) ) );

	/* Terminal */
	termGB = new QGroupBox( "Terminals", this );

	termsLW = new QListWidget( termGB );
	termsLW->setMinimumHeight( 250 );
	QStringList terms = QStringList() << "Inbuilt";
	Q_FOREACH( QString key, sett.allKeys() ) {
		if ( key.startsWith( "Terminals/" ) and ( key != "Terminals/Default" ) ) {
			terms << key.replace( "Terminals/", "" );
		}
	}
	termsLW->addItems( terms );
	termsLW->setCurrentRow( terms.indexOf( sett.value( "Terminals/Default" ).toString() ) );

	defaultBtn = new QPushButton( "Defau&lt", termGB );
	connect( defaultBtn, SIGNAL( clicked() ), this, SLOT( handleDefaultClicked() ) );

	addBtn = new QPushButton( "Add &Terminal", termGB );
	connect( addBtn, SIGNAL( clicked() ), this, SLOT( handleAddTermClicked() ) );

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

	QGroupBox *otherOptionsGB = new QGroupBox( "Other Options", this );
	otherOptionsGB->setLayout( otherGBLyt );

	QVBoxLayout *termBtnLyt = new QVBoxLayout();
	termBtnLyt->addWidget( defaultBtn );
	termBtnLyt->addWidget( addBtn );
	termBtnLyt->addStretch();

	QHBoxLayout *termLyt = new QHBoxLayout();
	termLyt->addWidget( termsLW );
	termLyt->addLayout( termBtnLyt );

	termGB->setLayout( termLyt );

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->addWidget( perFolderEnabled );
	lyt->addWidget( showSidePanelGB );
	lyt->addWidget( filePreviewGB );
	lyt->addWidget( otherOptionsGB );
	lyt->addWidget( termGB );
	lyt->addStretch();

	setLayout( lyt );
};

void NBSGeneralWidget::handlePerFolderChanged( bool perFolder ) {

	Settings->setValue( "PerFolderViews", perFolder );
	Settings->General.PerFolderViews = perFolder;
};

void NBSGeneralWidget::handleViewModeChanged( int ) {

	Settings->setValue( "ViewMode", defaultViewModeCB->currentText() );
	Settings->General.ViewMode = defaultViewModeCB->currentText();
};

void NBSGeneralWidget::handleIconSizeChanged( int ) {

	int iconSize = defaultIconSizeS->value();
	Settings->setValue( "IconSize", QSize( iconSize, iconSize ) );
	Settings->General.IconSize = QSize( iconSize, iconSize );

	iconSizeL->setText( QString( "%1 px" ).arg( iconSize ) );
};

void NBSGeneralWidget::handleGroupingChanged( bool grouping ) {

	Settings->setValue( "Grouping", grouping );
	Settings->General.Grouping = grouping;
};

void NBSGeneralWidget::handleSortCaseChanged( bool sortCase ) {

	Settings->setValue( "SortCase", sortCase );
	Settings->General.SortCase = sortCase;
};

void NBSGeneralWidget::handleSortColumnChanged( int sortColumn ) {

	switch( sortColumn ) {
		case 0: {
			Settings->setValue( "SortColumn", 0 );
			Settings->General.SortColumn = 0;
			break;
		}

		case 1:{
			Settings->setValue( "SortColumn", 1 );
			Settings->General.SortColumn = 1;
			break;
		}

		case 2:{
			Settings->setValue( "SortColumn", 2 );
			Settings->General.SortColumn = 2;
			break;
		}

		case 3:{
			Settings->setValue( "SortColumn", 4 );
			Settings->General.SortColumn = 4;
			break;
		}

		default: {
			if ( Settings->General.Grouping ) {
				Settings->setValue( "SortColumn", 2 );
				Settings->General.SortColumn = 2;
			}

			else {
				Settings->setValue( "SortColumn", 0 );
				Settings->General.SortColumn = 0;
			}
			break;
		}
	}

};

void NBSGeneralWidget::handleShowSidePanelToggled( bool showPanel ) {

	Settings->setValue( "SidePanel", showPanel );
	Settings->General.SidePanel = showPanel;
};

void NBSGeneralWidget::handleSidePanelChoice() {
	/* Modern: 0; Classic: 1 */

	if ( qobject_cast<QRadioButton*>( sender() ) == sidePanelRB )
		Settings->setValue( "SidePanelType", 0 );

	else
		Settings->setValue( "SidePanelType", 1 );
};

void NBSGeneralWidget::handlePreviewsChanged() {

	Settings->setValue( "View/FilePreviews", filePreviewGB->isChecked() );
	Settings->setValue( "View/ImagePreview", imagePreviewCB->isChecked() );
	Settings->setValue( "View/PdfPreview", pdfPreviewCB->isChecked() );
	Settings->setValue( "View/DjVuPreview", djvuPreviewCB->isChecked() );
	Settings->setValue( "View/OdfPreview", odfPreviewCB->isChecked() );
	Settings->setValue( "View/VideoPreview", videoPreviewCB->isChecked() );

	Settings->reload();
};

void NBSGeneralWidget::handleFilterDirsChanged( bool filterFolders ) {

	Settings->setValue( "FilterFolders", filterFolders );
	Settings->General.FilterFolders = filterFolders;
};

void NBSGeneralWidget::handleOpenWithToggled() {

	if ( qobject_cast<QCheckBox*>( sender() ) == openWithCB ) {
		if ( openWithCB->isChecked() ) {
			Settings->setValue( "SuperStart", true );
			Settings->General.SuperStart = true;
		}

		else {
			Settings->setValue( "SuperStart", false );
			Settings->General.SuperStart = false;
		}
	}
};

void NBSGeneralWidget::handleTrayIconChanged( bool value ) {

	Settings->setValue( "TrayIcon", value );
	Settings->General.TrayIcon = value;
};

void NBSGeneralWidget::handleExtendedIOChanged( bool enabled ) {

	Settings->setValue( "ExtendedIO", enabled );
	Settings->General.ExtendedIO = enabled;
};

void NBSGeneralWidget::handlePaintOverlayChanged( bool enabled ) {

	Settings->setValue( "View/PaintOverlay", enabled );
	Settings->View.PaintOverlay = enabled;
};

void NBSGeneralWidget::handleLogDebugChanged( bool enabled ) {

	Settings->setValue( "General/LogDebug", enabled );
	Settings->General.LogDebug = enabled;
};

void NBSGeneralWidget::handleDefaultClicked() {

	QString defTerm = termsLW->currentItem()->text();

	sett.setValue( "Terminals/Default", defTerm );
	sett.sync();
};

void NBSGeneralWidget::handleAddTermClicked() {

	NBAddVTEDialog *vteDlg = new NBAddVTEDialog( this );
	vteDlg->exec();

	/* Reload the detected terminals list */
	termsLW->clear();
	QStringList terms = QStringList() << "Inbuilt";
	Q_FOREACH( QString key, sett.allKeys() ) {
		if ( key.startsWith( "Terminals/" ) and ( key != "Terminals/Default" ) ) {
			terms << key.replace( "Terminals/", "" );
		}
	}
	termsLW->addItems( terms );
	termsLW->setCurrentRow( terms.indexOf( sett.value( "Terminals/Default" ).toString() ) );
};

NBAddVTEDialog::NBAddVTEDialog( QWidget *parent ) : NBDialog( parent ) {

	setFixedWidth( 300 );

	QLabel *execLbl = new QLabel( "&Executable:" );
	QLabel *cmdLbl = new QLabel( "Co&mmand Switch:" );

	execLE = new QLineEdit( this );
	execLbl->setBuddy( execLE );

	wDirCB = new QCheckBox( "&Work Dir Switch:" );
	connect( wDirCB, SIGNAL( clicked() ), this, SLOT( handleCBPressed() ) );

	workLE = new QLineEdit( this );
	workLE->setDisabled( true );

	cmdLE = new QLineEdit( this );
	cmdLbl->setBuddy( cmdLE );

	btns = new NBSegmentButton( this );
	btns->setCount( 2 );

	btns->setSegmentIcon( 0, QIcon( ":/icons/list-add.png" ) );
	btns->setSegmentText( 0, "&Add" );
	btns->segment( 0 )->setObjectName( "okBtn" );
	connect( btns->segment( 0 ), SIGNAL( clicked() ), this, SLOT( addTerminal() ) );

	btns->setSegmentIcon( 1, QIcon( ":/icons/delete.png" ) );
	btns->setSegmentText( 1, "&Cancel" );
	btns->segment( 0 )->setObjectName( "cancelBtn" );
	connect( btns->segment( 1 ), SIGNAL( clicked() ), this, SLOT( reject() ) );

	QHBoxLayout *execLyt = new QHBoxLayout();
	execLyt->addWidget( execLbl );
	execLyt->addWidget( execLE );

	QHBoxLayout *workLyt = new QHBoxLayout();
	workLyt->addWidget( wDirCB );
	workLyt->addWidget( workLE );

	QHBoxLayout *cmdLyt = new QHBoxLayout();
	cmdLyt->addWidget( cmdLbl );
	cmdLyt->addWidget( cmdLE );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addStretch();
	btnLyt->addWidget( btns );

	QVBoxLayout *dlgLyt = new QVBoxLayout();
	dlgLyt->addLayout( execLyt );
	dlgLyt->addLayout( workLyt );
	dlgLyt->addLayout( cmdLyt );
	dlgLyt->addLayout( btnLyt );

	setLayout( dlgLyt );
};

void NBAddVTEDialog::addTerminal() {

	QString exec = execLE->text().simplified();
	QString wrkSwitch = workLE->text().simplified();
	QString cmdSwitch = cmdLE->text().simplified();

	if ( wDirCB->isChecked() )
		sett.setValue( "Terminals/" + execLE->text(), QStringList() << exec << wrkSwitch << "%1" << cmdSwitch << "%2" );

	else
		sett.setValue( "Terminals/" + execLE->text(), QStringList() << exec << cmdSwitch << "cd %1 && %2" );

	sett.sync();

	accept();
};

void NBAddVTEDialog::handleCBPressed() {

	if ( wDirCB->isChecked() ) {
		workLE->setEnabled( true );
		workLE->setFocus();
	}

	else {
		workLE->setDisabled( true );
		wDirCB->setFocus();
	}
};
