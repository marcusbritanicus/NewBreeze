/*
	*
	* NBStartUpWizard.cpp - A simple startup wizard for NewBreeze
	*
*/

#include "NBStartupWizard.hpp"

NBStartupWizard::NBStartupWizard( QWidget *parent ) : QWizard( parent ) {

	createGui();
	setupConnections();
	setWindowProperties();
};

void NBStartupWizard::createGui() {
	/*createGui() -> None

	Create a GUI

	@return None
	*/

	setOptions( QWizard::NoBackButtonOnStartPage | QWizard::IndependentPages );
	setButtonLayout( QList<QWizard::WizardButton>() << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton << QWizard::NextButton << QWizard::FinishButton );

	setButtonText( QWizard::CancelButton, "&Skip" );
	setButtonText( QWizard::BackButton, "Go &Back" );
	setButtonText( QWizard::NextButton, "&Continue" );

	setPixmap( QWizard::LogoPixmap, QPixmap( "../../../icons/startup/nb3.png" ).scaled( 48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	createPages();
};

void NBStartupWizard::createPages() {
	/*createPages() -> None

	Create various pages of the wizard

	@return None
	*/

	//################################################################################
	//                                    Page 01                                    #
	//################################################################################

	QWizardPage *page1 = new QWizardPage();

	QLabel *p1lbl1 = new QLabel( "<center><h3>Welcome to NewBreeze v3.0</h3><h4>The super fast file browser</h4></center>" );
	p1lbl1->setWordWrap( true );

	QLabel *p1lbl2 = new QLabel( this );
	p1lbl2->setPixmap( QPixmap( "../../../icons/startup/nb3.png" ).scaled( 128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	p1lbl2->setAlignment( Qt::AlignCenter );
	p1lbl2->setMinimumHeight( 256 );

	QLabel *p1lbl3 = new QLabel(
		"<p>Press <tt><b>Next</b></tt> to see what new features NewBreeze v3 brings to you and if you want, configure NewBreeze.</p>"
		"<p>If you would like to be surprised by the new features, you may press <tt><b>Skip</b></tt> and open NewBreeze with preset default settings.</p>"
		"<p><small>Please note that you can press the <tt><b>Skip</b></tt> button anytime you want, and the customizations done thus far will be saved.</small></p>"
	);
	p1lbl3->setWordWrap( true );

	QVBoxLayout *p1Lyt = new QVBoxLayout();
	p1Lyt->addWidget( p1lbl1 );
	p1Lyt->addWidget( p1lbl2 );
	p1Lyt->addWidget( p1lbl3 );

	page1->setLayout( p1Lyt );

	addPage( page1 );

	//################################################################################
	//                                    Page 02                                    #
	//################################################################################

	QWizardPage *page2 = new QWizardPage();
	page2->setTitle( "File/Directory Grouping" );
	page2->setSubTitle( "Would you like to enable file/directory grouping by default?" );

	QLabel *p2lbl1 = new QLabel(
		"Grouping is a way to visually organize directories, files and other nodes. If you choose to enable grouping, "
		"you will get to see similar files together based on preset criteria: Name, Type, Size, Date."
	);
	p2lbl1->setWordWrap( true );

	p2gb1 = new QGroupBox( "Yes, enable &grouping by default" );

	p2btn1 = new QCommandLinkButton( p2gb1 );
	p2btn1->setCheckable( true );
	p2btn1->setChecked( false );
	p2btn1->setText( "&Name" );
	p2btn1->setDescription( "Files having the same starting letter will be shown together." );

	p2btn2 = new QCommandLinkButton( p2gb1 );
	p2btn2->setCheckable( true );
	p2btn2->setChecked( false );
	p2btn2->setText( "&Type" );
	p2btn2->setDescription( "Files with the same extensions will be shown together." );

	p2btn3 = new QCommandLinkButton( p2gb1 );
	p2btn3->setCheckable( true );
	p2btn3->setChecked( false );
	p2btn3->setText( "&Size" );
	p2btn3->setDescription( "Files having comparable sizes will be shown together." );

	p2btn4 = new QCommandLinkButton( p2gb1 );
	p2btn4->setCheckable( true );
	p2btn4->setChecked( false );
	p2btn4->setText( "&Date" );
	p2btn4->setDescription( "Files created around the same time will be shown together." );

	QLabel *p2lbl2 = new QLabel(
		"<small>Note that inside a group all the nodes will be sorted alphabetically, except for <tt>Size</tt> grouping where they will be sorted "
		"by size. Also, in all the cases, directories will be shown at the beginning of the group, sorted alphabetically.</small>"
	);
	p2lbl2->setWordWrap( true );

	QGridLayout *p2gb1Lyt = new QGridLayout();
	p2gb1Lyt->addWidget( p2btn1, 0, 0 );
	p2gb1Lyt->addWidget( p2btn2, 0, 1 );
	p2gb1Lyt->addWidget( p2btn3, 1, 0 );
	p2gb1Lyt->addWidget( p2btn4, 1, 1 );
	p2gb1Lyt->addWidget( p2lbl2, 2, 0, 1, 2 );

	p2gb1->setCheckable( true );
	p2gb1->setChecked( false );
	p2gb1->setLayout( p2gb1Lyt );

	p2btnGrp1 = new QButtonGroup();
	p2btnGrp1->setExclusive( true );
	p2btnGrp1->addButton( p2btn1, 1 );
	p2btnGrp1->addButton( p2btn2, 2 );
	p2btnGrp1->addButton( p2btn3, 3 );
	p2btnGrp1->addButton( p2btn4, 4 );

	QLabel *p2lbl3 = new QLabel(
		"Please note that grouping comes at a small premium. While the grouped file layout looks beautiful, it takes some extra effort and "
		"time to sort the nodes into groups. With slow processors, this can give you significant performance lag. Also, you may expreience "
		"delays if the number of files is large."
	);
	p2lbl3->setWordWrap( true );

	QVBoxLayout *p2Lyt = new QVBoxLayout();
	p2Lyt->addWidget( p2lbl1 );
	p2Lyt->addWidget( p2gb1 );
	p2Lyt->addWidget( p2lbl3 );

	page2->setLayout( p2Lyt );

	addPage( page2 );

	//################################################################################
	//                                    Page 03                                    #
	//################################################################################

	QWizardPage *page3 = new QWizardPage();
	page3->setTitle( "View Mode and Icon Size" );
	page3->setSubTitle( "How would you like to see the icons when you open a folder?" );

	QLabel *p3lbl1 = new QLabel(
		"<p>In NewBreeze, folder contents can be shown in three ways as Icons, Tiles or Details.</p>"
		"<p>Icons mode is the standard way almost all file browsers display the folder contents: A prominent icon, below which is the name.</p>"
		"<p>Tiles mode is reminiscent of MS Windows Style: A prominent icon, with the name and some details to the right of the icon.</p>"
		"<p>Details mode shows all the inmportant information about the node in a single row.</p>"
	);
	p3lbl1->setWordWrap( true );

	QLabel *p3lbl2 = new QLabel( "Default &View Mode" );

	p3cb1 = new QComboBox( this );
	p3cb1->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	p3cb1->addItems( QStringList() << "Icons" << "Tiles" << "Details" );

	p3lbl2->setBuddy( p3cb1 );

	QLabel *p3lbl3 = new QLabel( "Default &Icon Size" );

	p3sl1 = new QSlider( Qt::Horizontal );
	p3sl1->setTickPosition( QSlider::TicksBelow );
	p3sl1->setRange( 16, 128 );
	p3sl1->setTickInterval( 16 );
	p3sl1->setSingleStep( 16 );

	QLabel *p3lbl4 = new QLabel( this );
	p3lbl4->setMinimumWidth( 45 );

	connect( p3sl1, SIGNAL( valueChanged( int ) ), p3lbl4, SLOT( setNum( int ) ) );
	p3lbl3->setBuddy( p3sl1 );
	p3sl1->setValue( 48 );

	QGridLayout *p3gb1Lyt = new QGridLayout();
	p3gb1Lyt->addWidget( p3lbl2, 0, 0 );
	p3gb1Lyt->addWidget( p3cb1, 0, 1 );
	p3gb1Lyt->addWidget( p3lbl3, 1, 0 );
	p3gb1Lyt->addWidget( p3sl1, 1, 1 );
	p3gb1Lyt->addWidget( p3lbl4, 1, 2 );

	p3gb1 = new QGroupBox( "View mode and Icon Size" );
	p3gb1->setLayout( p3gb1Lyt );

	QVBoxLayout *p3Lyt = new QVBoxLayout();
	p3Lyt->addWidget( p3lbl1 );
	p3Lyt->addWidget( new QLabel( " " ) );
	p3Lyt->addWidget( p3gb1 );

	page3->setLayout( p3Lyt );

	addPage( page3 );

	//################################################################################
	//                                    Page 04                                    #
	//################################################################################

	QWizardPage *page4 = new QWizardPage();
	page4->setTitle( "Filters" );
	page4->setSubTitle( "Would you like to filter folders?" );

	QLabel *p4lbl1 = new QLabel(
		"<p>The folder contents can be filtered based on a user defnied string. This is called Name filtering.</p>"
		"<p>For example, if the name filter is 'the', then by default, all the folders will be visible, and only files "
		"that have 'the' in their name will be visible. However, if you enable folder filtering as well, then only files "
		"and folders that contains 'the' in their name will be visible.</p>"
		"<p>While filtering the folder contents, do you want the sub-folders to be filtered as well, or just the files?</p>"
	);
	p4lbl1->setWordWrap( true );

	p4cb1 = new QCheckBox( "Enable fi&ltering of folders along with files" );
	p4cb1->setChecked( false );

	QHBoxLayout *p4gb1Lyt = new QHBoxLayout();
	p4gb1Lyt->addWidget( p4cb1 );

	p4gb1 = new QGroupBox( "Filter files  and folders" );
	p4gb1->setLayout( p4gb1Lyt );

	QVBoxLayout *p4Lyt = new QVBoxLayout();
	p4Lyt->addWidget( p4lbl1 );
	p4Lyt->addWidget( new QLabel( " " ) );
	p4Lyt->addWidget( p4gb1 );

	page4->setLayout( p4Lyt );

	addPage( page4 );

	//################################################################################
	//                                    Page 05                                    #
	//################################################################################

	QWizardPage *page5 = new QWizardPage();
	page5->setTitle( "Case-sensitive sorting" );
	page5->setSubTitle( "Enable case sensitive sorting?" );

	QLabel *p5lbl1 = new QLabel(
		"<p>By default, the sorting of files and folders are case-insensitive. This means that 'a' is same as 'A'.</p>"
		"<p>If enabled, all the files/folders starting with upper case letters will be sorted before the ones starting "
		"with lower case.</p>"
		"<p>Do you want to sort the files and folders case-sensitively?</p>"
	);
	p5lbl1->setWordWrap( true );

	p5cb1 = new QCheckBox( "Yes, enable case-sensitive sorting of files and folders" );
	p5cb1->setChecked( false );

	QHBoxLayout *p5gb1Lyt = new QHBoxLayout();
	p5gb1Lyt->addWidget( p5cb1 );

	p5gb1 = new QGroupBox( "Case-sensitive sorting" );
	p5gb1->setLayout( p5gb1Lyt );

	QVBoxLayout *p5Lyt = new QVBoxLayout();
	p5Lyt->addWidget( p5lbl1 );
	p5Lyt->addWidget( new QLabel( " " ) );
	p5Lyt->addWidget( p5gb1 );

	page5->setLayout( p5Lyt );

	addPage( page5 );

	//################################################################################
	//                                    Page 06                                    #
	//################################################################################

	QWizardPage *page6 = new QWizardPage();
	page6->setTitle( "File previews" );
	page6->setSubTitle( "Do you want to ?" );

	QLabel *p6lbl1 = new QLabel(
		"<p>By default, NewBreeze shows an icon corresponding to the type of file in question However in some cases, "
		"it is capable of showing the contents of the file, to a limited extent. These previews are called file thumbnails. "
		"Currently NewBreeze can show the preview of only images. Preview of other files may be enables in the future.</p>"
		"<p>Do you want to show image thumbnails by default?</p>"
	);
	p6lbl1->setWordWrap( true );

	p6cb1 = new QCheckBox( "Show thumbnails of images in NewBreeze" );
	p6cb1->setChecked( false );

	QHBoxLayout *p6gb1Lyt = new QHBoxLayout();
	p6gb1Lyt->addWidget( p6cb1 );

	p6gb1 = new QGroupBox( "Image thumbnails" );
	p6gb1->setLayout( p6gb1Lyt );

	QLabel *p6lbl2 = new QLabel(
		"<p>Note: Obtaining and loading thumbnails is a slow process, that involes reading the image files and then storing a "
		"copy of the resized image. This may cause performance issues during the loading of image previews. In order to ensure "
		"faster loading of thumbnails, NewBreeze maintains a cache of these generated thumbnails. This consumes a typical "
		"space of 20 kiB per image.</p>"
		"<p>If your hard disk space is at a premium, or you have a slow processor, it may be advisable <b>not</b> to enable "
		"file previews.</p>"
	);
	p6lbl2->setWordWrap( true );

	QVBoxLayout *p6Lyt = new QVBoxLayout();
	p6Lyt->addWidget( p6lbl1 );
	p6Lyt->addWidget( new QLabel( " " ) );
	p6Lyt->addWidget( p6gb1 );
	p6Lyt->addWidget( p6lbl2 );

	page6->setLayout( p6Lyt );

	addPage( page6 );

	//################################################################################
	//                                    EndPage                                    #
	//################################################################################

	QWizardPage *pageL = new QWizardPage();

	QLabel *pLlbl1 = new QLabel(
		"<center><h3>Thank you for choosing NewBreeze v3.0</h3></center>"
	);

	QLabel *pLlbl2 = new QLabel(
		"<center><p>Hope your experience with NewBreeze is a pleasant one.</p><br style='line-height: 5%;'>"
		"<p><a href = 'mailto:marcusbritanicus@gmail.com'>Drop in</a> a word of appreciation if you enjoyed the experience, and "
		"feel free to distribute NewBreeze to all your family, friends and colleagues.</p><br style='line-height: 5%;'>"
		"<p>If you stumbled across an annoying bug which ruined your time, do not hesitate to file a bug report "
		"<a href = 'https://github.com/marcusbritanicus/NewBreeze/issues/new'>here</a>.</p></center>"
	);
	pLlbl2->setWordWrap( true );
	pLlbl2->setOpenExternalLinks( true );

	QGroupBox *pLgb1 = new QGroupBox();
	pLgb1->setFixedWidth( 461 );
	QVBoxLayout *pLgb1lyt = new QVBoxLayout();
	pLgb1lyt->addWidget( pLlbl2 );
	pLgb1->setLayout( pLgb1lyt );

	QVBoxLayout *pLlyt = new QVBoxLayout();
	pLlyt->setAlignment( Qt::AlignCenter );
	pLlyt->addWidget( pLlbl1 );
	pLlyt->addStretch();
	pLlyt->addWidget( pLgb1 );
	pLlyt->addStretch();

	pageL->setLayout( pLlyt );

	addPage( pageL );
};

void NBStartupWizard::setupConnections() {
	/*setupConnections() -> None

	Setup the Signal Connections

	@return None
	*/

	connect( p2gb1, SIGNAL(), this, SLOT( saveSettings() ) );
	connect( p2btnGrp1, SIGNAL(), this, SLOT( saveSettings() ) );
};

void NBStartupWizard::setWindowProperties() {
	/*setWindowProperties() -> None

	Set the window properties like title, size, position, etc...

	@param Param: Description

	@return None
	*/

	setWindowTitle( "NewBreeze Startup Wizard" );
	setWindowIcon( QIcon::fromTheme( "newbreeze" ) );

	setFixedSize( 640, 480 );

	QSize sSize = QDesktopWidget().size();
	int hpos = ( sSize.width() - 640 ) / 2;
	int vpos = ( sSize.height() - 480 ) / 2;

	move( hpos, vpos );

	setWindowFlags( windowFlags() | Qt::FramelessWindowHint );
};

void NBStartupWizard::saveSettings() {

	close();
};

void NBStartupWizard::paintEvent( QPaintEvent *pEvent ) {
	/*paintEvent( QPaintEvent ) -> None

	Override QWizard::paintEvent(...);

	@return None
	*/

	QWizard::paintEvent( pEvent );

	QPainter painter( this );
	painter.setPen( QPen( palette().color( QPalette::Window ).darker(), 2 ) );

	painter.drawRect( rect() );
	painter.end();

	pEvent->accept();
};
