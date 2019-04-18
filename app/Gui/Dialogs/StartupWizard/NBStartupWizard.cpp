/*
	*
	* NBStartupWizard.cpp - A simple startup wizard for NewBreeze
	*
*/

#include "NBStartupWizard.hpp"

static QSettings sett( "NewBreeze", "NewBreeze" );

NBStartupWizard::NBStartupWizard() : QWizard() {

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
	setButtonText( QWizard::NextButton, "Conti&nue" );

	setPixmap( QWizard::LogoPixmap, QPixmap( ":/icons/startup/nb3.png" ).scaled( 48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

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
	p1lbl2->setPixmap( QPixmap( ":/icons/startup/nb3.png" ).scaled( 128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
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
	p2gb1->setCheckable( true );
	p2gb1->setChecked( true );

	p2btn1 = new QCommandLinkButton( p2gb1 );
	p2btn1->setCheckable( true );
	p2btn1->setChecked( false );
	p2btn1->setText( "&Name" );
	p2btn1->setDescription( "Files having the same starting letter will be shown together." );

	p2btn2 = new QCommandLinkButton( p2gb1 );
	p2btn2->setCheckable( true );
	p2btn2->setChecked( true );
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

	p2cb1 = new QCheckBox( "&Remember settings of each folder separately (Per-folder view-settings)" );
	p2cb1->setChecked( true );

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
	p2gb1Lyt->addWidget( p2cb1, 3, 0, 1, 2 );

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
		"<p>Icons mode is the standard way almost all file browsers display the folder contents: A prominent icon, below which is the name. "
		"Tiles mode is reminiscent of MS Windows Style: A prominent icon, with the name and some details to the right of the icon. "
		"Details mode shows all the important information about the node in a single row.</p>"
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
	page6->setSubTitle( "Do you want to see a preview of images?" );

	QLabel *p6lbl1 = new QLabel(
		"<p>By default, NewBreeze shows an icon corresponding to the type of file in question. However in some cases, "
		"it is capable of showing the contents of the file, to a limited extent. These previews are called file thumbnails. "
		"Currently NewBreeze can show the preview of only images. Preview of other files may be enables in the future.</p>"
		"<p>Do you want to show image thumbnails by default?</p>"
	);
	p6lbl1->setWordWrap( true );

	p6cb1 = new QCheckBox( "Show &thumbnails of images in NewBreeze" );
	p6cb1->setChecked( true );

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
	//                                    Page 07                                    #
	//################################################################################

	QWizardPage *page7 = new QWizardPage();
	page7->setTitle( "Side Panel" );
	page7->setSubTitle( "Which SidePanel would you like?" );

	QLabel *p7lbl1 = new QLabel(
		"A Side Panel is a quick way to access some locations, specially, external devices, HDD partitions, CD/DVD etc. "
		"It can also show Catalogs, QuickFiles, Bookmarks and more.."
	);
	p7lbl1->setWordWrap( true );

	p7gb1 = new QGroupBox( "Yes, enable the &SidePanel by default" );
	p7gb1->setCheckable( true );
	p7gb1->setChecked( true );

	p7btn1 = new QCommandLinkButton( p7gb1 );
	p7btn1->setCheckable( true );
	p7btn1->setChecked( false );
	p7btn1->setText( "&Modern SidePanel" );
	p7btn1->setDescription( "A 48px side panel, showing Drives and Bookmarks using a popup menu" );
	p7btn1->setDisabled( false );

	p7btn2 = new QCommandLinkButton( p7gb1 );
	p7btn2->setCheckable( true );
	p7btn2->setChecked( true );
	p7btn2->setText( "Classic SideBar" );
	p7btn2->setDescription( "A Mac Style SideBar showing Drives, Bookmarks and QuickFiles" );

	QGridLayout *p7gb1Lyt = new QGridLayout();
	p7gb1Lyt->addWidget( p7btn1, 0, 0 );
	p7gb1Lyt->addWidget( p7btn2, 0, 1 );

	p7gb1->setLayout( p7gb1Lyt );

	p7btnGrp1 = new QButtonGroup();
	p7btnGrp1->setExclusive( true );
	p7btnGrp1->addButton( p7btn1, 0 );
	p7btnGrp1->addButton( p7btn2, 1 );

	QVBoxLayout *p7Lyt = new QVBoxLayout();
	p7Lyt->addWidget( p7lbl1 );
	p7Lyt->addWidget( p7gb1 );

	page7->setLayout( p7Lyt );

	addPage( page7 );

	//################################################################################
	//                                    Page 08                                    #
	//################################################################################

	QWizardPage *page8 = new QWizardPage();
	page8->setTitle( "InfoBar vs InfoPanel" );
	page8->setSubTitle( "Where do you want the details to be shown?" );

	QLabel *p8lbl1 = new QLabel(
		"A status bar is typically used to show details like, file size, permissions, modified date, etc. "
		"InfoBar shows these details at the bottom of the window, while the InfoPanel shows it on the right. "
		"Choose yours here!"
	);
	p8lbl1->setWordWrap( true );

	p8gb1 = new QGroupBox( "InfoBar or InfoPanel" );
	p8gb1->setCheckable( false );

	p8btn1 = new QCommandLinkButton( p8gb1 );
	p8btn1->setCheckable( true );
	p8btn1->setChecked( true );
	p8btn1->setText( "Info&Bar" );
	p8btn1->setDescription( "Standard StatusBar showing file, folder or selection details at the bottom of the window." );

	p8btn2 = new QCommandLinkButton( p8gb1 );
	p8btn2->setCheckable( true );
	p8btn2->setChecked( false );
	p8btn2->setText( "Info&Panel" );
	p8btn2->setDescription( "A panel showing file, folder or selection details at the right side of the window." );

	QGridLayout *p8gb1Lyt = new QGridLayout();
	p8gb1Lyt->addWidget( p8btn1, 0, 0 );
	p8gb1Lyt->addWidget( p8btn2, 0, 1 );

	p8gb1->setLayout( p8gb1Lyt );

	p8btnGrp1 = new QButtonGroup();
	p8btnGrp1->setExclusive( true );
	p8btnGrp1->addButton( p8btn1, 0 );
	p8btnGrp1->addButton( p8btn2, 1 );

	QVBoxLayout *p8Lyt = new QVBoxLayout();
	p8Lyt->addWidget( p8lbl1 );
	p8Lyt->addWidget( p8gb1 );

	page8->setLayout( p8Lyt );

	addPage( page8 );

	//################################################################################
	//                                    Page 09                                    #
	//################################################################################

	QWizardPage *page9 = new QWizardPage();
	page9->setTitle( "Startup Location" );
	page9->setSubTitle( "What should NewBreeze show you when it starts?" );

	QLabel *p9lbl1 = new QLabel(
		"<p>When you open a file manager, it usually show you the home folder. With NB3 you have great alternative: SuperStart. "
		"While Catalogs is the NewBreeze equivalent of Libraries in Windows, SuperStart is a feature unique to NewBreeze alone.</p>"
		"<p>Some file and folders are more accessed than others, and yet, it may not be suitable to put them in a particular location. "
		"Enter SuperStart: Put you frequently used files and folders into SuperStart and access them as soon as you start NewBreeze.</p>"
	);
	p9lbl1->setWordWrap( true );

	p9gb1 = new QGroupBox( "&Default Start Location" );

	p9btn1 = new QCommandLinkButton( p9gb1 );
	p9btn1->setCheckable( true );
	p9btn1->setChecked( true );
	p9btn1->setText( "&SuperStart" );
	p9btn1->setDescription( "Access you most used files and folders the moment you start NewBreeze" );

	p9btn2 = new QCommandLinkButton( p9gb1 );
	p9btn2->setCheckable( true );
	p9btn2->setChecked( false );
	p9btn2->setText( "&Home directory" );
	p9btn2->setDescription( "Open NewBreeze to find the contents of your home folder listed" );

	QGridLayout *p9gb1Lyt = new QGridLayout();
	p9gb1Lyt->addWidget( p9btn1, 0, 0 );
	p9gb1Lyt->addWidget( p9btn2, 0, 1 );

	p9gb1->setLayout( p9gb1Lyt );

	p9btnGrp1 = new QButtonGroup();
	p9btnGrp1->setExclusive( true );
	p9btnGrp1->addButton( p9btn1, 0 );
	p9btnGrp1->addButton( p9btn2, 1 );

	QVBoxLayout *p9Lyt = new QVBoxLayout();
	p9Lyt->addWidget( p9lbl1 );
	p9Lyt->addWidget( p9gb1 );

	page9->setLayout( p9Lyt );

	addPage( page9 );

	//################################################################################
	//                                    Page 10                                    #
	//################################################################################

	QWizardPage *page10 = new QWizardPage();
	page10->setTitle( "Close to tray" );
	page10->setSubTitle( "Do you want NewBreeze to stay in the SystemTray after closing?" );

	QLabel *p10lbl1 = new QLabel(
		"<p>Usually when you close the main window of an application, the program closes and memory is freed for other purposes. "
		"However, a file manager is one of the most used programs in an OS and need to be loaded often. Instead of closing it "
		"completely, we can close the main window and keep NewBreeze open in tray. This was it takes far much shorter time for it "
		"start up again.</p>"
		"<p>Do you want NewBreeze to stay in the SystemTray after close of the application?</p>"
	);
	p10lbl1->setWordWrap( true );

	p10cb1 = new QCheckBox( "&Close to System Tray" );
	p10cb1->setChecked( true );

	QHBoxLayout *p10gb1Lyt = new QHBoxLayout();
	p10gb1Lyt->addWidget( p10cb1 );

	p10gb1 = new QGroupBox( "Tray Icon" );
	p10gb1->setLayout( p10gb1Lyt );

	QVBoxLayout *p10Lyt = new QVBoxLayout();
	p10Lyt->addWidget( p10lbl1 );
	p10Lyt->addWidget( new QLabel( " " ) );
	p10Lyt->addWidget( p10gb1 );

	page10->setLayout( p10Lyt );

	addPage( page10 );

	//################################################################################
	//                                    Page 11                                    #
	//################################################################################

	QWizardPage *page11 = new QWizardPage();
	page11->setTitle( "Terminals" );
	page11->setSubTitle( "Which is your favorite terminal applicaiton?" );

	QLabel *p11lbl1 = new QLabel(
		"<p>Which browsing the file system it sometimes becomes necessary to open a terminal in the working directory. "
		"There may be multiple terminal applications installed in your system</p>"
		"<p>Which terminal application do you want to use?</p>"
	);
	p11lbl1->setWordWrap( true );

	QGroupBox *p11gb1 = new QGroupBox( "Terminals" );

	p11lw1 = new QListWidget( p11gb1 );
	loadTerminals();

	p11btn1	 = new QToolButton( p11gb1 );
	p11btn1->setIcon( QIcon( ":/icons/list-add.png" ) );
	p11btn1->setStyleSheet( "border: none; background-color: none;" );
	p11btn1->setToolTip( "Add a terminal" );
	connect( p11btn1, SIGNAL( clicked() ), this, SLOT( addNewTerminal() ) );

	QGridLayout *p11gb1Lyt = new QGridLayout();
	p11gb1Lyt->addWidget( p11lw1, 0, 0 );
	p11gb1Lyt->addWidget( p11btn1, 0, 0, Qt::AlignRight | Qt::AlignBottom );

	p11gb1->setLayout( p11gb1Lyt );

	QVBoxLayout *p11Lyt = new QVBoxLayout();
	p11Lyt->addWidget( p11lbl1 );
	p11Lyt->addWidget( p11gb1 );

	page11->setLayout( p11Lyt );

	addPage( page11 );

	//################################################################################
	//                                    Page 12                                    #
	//################################################################################

	QWizardPage *page12 = new QWizardPage();
	page12->setTitle( "NewBreeze Specials" );
	page12->setSubTitle( "Some settings that makes NewBreeze a pleasure to use" );

	QLabel *p12lbl1 = new QLabel(
		"<p>IconOverlays and ExtendedIO are two of NewBreeze's premium features. IconOverlays enable you to perfrom "
		"most actions that require a right click without one. ExtendedIO helps you copy/move files/directories "
		"to any other place without leaving the current folder.</p>"
	);
	p12lbl1->setWordWrap( true );

	QGroupBox *p12gb1 = new QGroupBox( "NewBreeze Specials" );
	p12cb1 = new QCheckBox( "Enable Icon&Overlays" );
	p12cb2 = new QCheckBox( "Enable E&xtendedIO" );
	p12cb2->setChecked( true );

	QVBoxLayout *p12gb1Lyt = new QVBoxLayout();
	p12gb1Lyt->addWidget( p12cb1 );
	p12gb1Lyt->addWidget( p12cb2 );
	p12gb1->setLayout( p12gb1Lyt );

	QVBoxLayout *p12Lyt = new QVBoxLayout();
	p12Lyt->addWidget( p12lbl1 );
	p12Lyt->addWidget( p12gb1 );

	page12->setLayout( p12Lyt );

	addPage( page12 );

	//################################################################################
	//                                    EndPage                                    #
	//################################################################################

	QWizardPage *pageL = new QWizardPage();

	QLabel *pLlbl1 = new QLabel(
		"<center><h3>Thank you for choosing NewBreeze v3.0</h3></center>"
	);

	QLabel *pLlbl2 = new QLabel();
	pLlbl2->setPixmap( QPixmap( ":/icons/startup/nb3.png" ).scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	pLlbl2->setAlignment( Qt::AlignCenter );
	pLlbl2->setMinimumHeight( 128 );

	QLabel *pLlbl3 = new QLabel(
		"<center><p>Hope your experience with NewBreeze is a pleasant one.</p><br style='line-height: 5%;'>"
		"<p><a href = 'mailto:marcusbritanicus@gmail.com'>Drop in</a> a word of appreciation if you enjoyed the experience, and "
		"feel free to distribute NewBreeze to all your family, friends and colleagues.</p><br style='line-height: 5%;'>"
		"<p>If you stumbled across an annoying bug which ruined your time, do not hesitate to file a bug report "
		"<a href = 'https://gitlab.com/marcusbritanicus/NewBreeze/issues/new'>here</a>.</p></center>"
	);
	pLlbl3->setWordWrap( true );
	pLlbl3->setOpenExternalLinks( true );

	QGroupBox *pLgb1 = new QGroupBox();
	pLgb1->setFixedWidth( 461 );
	QVBoxLayout *pLgb1lyt = new QVBoxLayout();
	pLgb1lyt->addWidget( pLlbl3 );
	pLgb1->setLayout( pLgb1lyt );

	QVBoxLayout *pLlyt = new QVBoxLayout();
	pLlyt->setAlignment( Qt::AlignCenter );
	pLlyt->addWidget( pLlbl1 );
	pLlyt->addStretch();
	pLlyt->addWidget( pLlbl2 );
	pLlyt->addWidget( pLgb1 );
	pLlyt->addStretch();

	pageL->setLayout( pLlyt );
	pageL->setFinalPage( true );

	addPage( pageL );
};

void NBStartupWizard::setupConnections() {
	/*setupConnections() -> None

	Setup the Signal Connections

	@return None
	*/

	connect( this, SIGNAL( currentIdChanged( int ) ), this, SLOT( disableCancelOnLastPage( int ) ) );
	connect( this, SIGNAL( finished( int ) ), this, SLOT( saveSettings() ) );
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

void NBStartupWizard::loadTerminals() {

	QStringList terms = QStringList() << "Inbuilt";

	/* NBTerminal */
	QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
	Q_FOREACH( QString path, paths ) {
		if ( QFileInfo( path + "/nbterminal" ).exists() ) {
			QStringList termInfo = QStringList() << "nbterminal" << "--workdir" << "%1" << "-e" << "%2";
			sett.setValue( "Terminals/nbterminal", QVariant( termInfo ) );
			terms << "nbterminal";
		}

		/* QTerminal */
		if ( QFileInfo( path + "/qterminal" ).exists() ) {
			QStringList termInfo = QStringList() << "qterminal" << "--workdir" << "%1" << "-e" << "%2";
			sett.setValue( "Terminals/qterminal", QVariant( termInfo ) );
			terms << "qterminal";
		}

		/* Konsole */
		if ( QFileInfo( path + "/konsole" ).exists() ) {
			QStringList termInfo = QStringList() << "konsole" << "--workdir" << "%1" << "-e" << "%2";
			sett.setValue( "Terminals/konsole", QVariant( termInfo ) );
			terms << "konsole";
		}

		/* Gnome Terminal */
		if ( QFileInfo( path + "/gnome-terminal" ).exists() ) {
			QStringList termInfo = QStringList() << "gnome-terminal" << "--working-directory" << "%1" << "-x" << "%2";
			sett.setValue( "Terminals/gnome-terminal", QVariant( termInfo ) );
			terms << "gnome-terminal";
		}

		/* Mate Terminal */
		if ( QFileInfo( path + "/mate-terminal" ).exists() ) {
			QStringList termInfo = QStringList() << "mate-terminal" << "--working-directory" << "%1" << "-x" << "%2";
			sett.setValue( "Terminals/mate-terminal", QVariant( termInfo ) );
			terms << "mate-terminal";
		}

		/* XFCE4 Terminal */
		if ( QFileInfo( path + "/xfce4-terminal" ).exists() ) {
			QStringList termInfo = QStringList() << "xfce4-terminal" << "--working-directory" << "%1" << "-x" << "%2";
			sett.setValue( "Terminals/xfce4-terminal", QVariant( termInfo ) );
			terms << "xfce4-terminal";
		}

		/* LXTerminal */
		if ( QFileInfo( path + "/lxterminal" ).exists() ) {
			QStringList termInfo = QStringList() << "lxterminal" << "--working-directory" << "%1" << "-e" << "%2";
			sett.setValue( "Terminals/lxterminal", QVariant( termInfo ) );
			terms << "lxterminal";
		}

		/* XTerm */
		if ( QFileInfo( path + "/xterm" ).exists() ) {
			sett.setValue( "Terminals/xterm", QVariant( QStringList() << "xterm" << "-e" << "cd %1 && %2" ) );
			terms << "xterm";
		}
	}

	terms.removeDuplicates();
	p11lw1->addItems( terms );
};

void NBStartupWizard::saveSettings() {

	/* Version flag */
	sett.setValue( "Version", "3.0.0" );

	/* Grouping */
	sett.setValue( "Grouping", p2gb1->isChecked() );
	if ( p2btnGrp1->checkedId() == -1 )
		sett.setValue( "SortColumn", 2 );

	else if ( p2btnGrp1->checkedId() < 3 )
		sett.setValue( "SortColumn", p2btnGrp1->checkedId() );

	else  // if ( p2btnGrp1->checkedId() == 0 )
		sett.setValue( "SortColumn", 4 );

	sett.setValue( "PerFolderViews", p2cb1->isChecked() );

	/* ViewMode and Icon Size */
	sett.setValue( "View/ViewMode", p3cb1->currentText() );
	if ( p3cb1->currentText() == "Icons" ) {
		sett.setValue( "View/IconsImageSize", QSize( p3sl1->value(), p3sl1->value() ) );
		sett.setValue( "View/TilesImageSize", QSize( p3sl1->value(), p3sl1->value() ) );
		sett.setValue( "View/DetailsImageSize", QSize( 24, 24 ) );
	}

	else if ( p3cb1->currentText() == "Tiles" ) {
		sett.setValue( "View/IconsImageSize", QSize( p3sl1->value(), p3sl1->value() ) );
		sett.setValue( "View/TilesImageSize", QSize( p3sl1->value(), p3sl1->value() ) );
		sett.setValue( "View/DetailsImageSize", QSize( 24, 24 ) );
	}

	else {
		sett.setValue( "View/IconsImageSize", QSize( 48, 48 ) );
		sett.setValue( "View/TilesImageSize", QSize( 48, 48 ) );
		sett.setValue( "View/DetailsImageSize", QSize( p3sl1->value(), p3sl1->value() ) );
	}

	/* Default icon theme */
	sett.setValue( "View/IconTheme", QIcon::themeName() );
	sett.setValue( "View/Style", "Fusion" );

	/* Folder Filtering */
	sett.setValue( "FilterFolders", p4cb1->isChecked() );

	/* Case-sensitive sorting */
	sett.setValue( "SortCase", p5cb1->isChecked() );

	/* Image previews */
	sett.setValue( "View/FilePreviews", p6cb1->isChecked() );
	sett.setValue( "View/ImagePreview", p6cb1->isChecked() );
	sett.setValue( "View/DjVuPreview", false );
	sett.setValue( "View/OdfPreview", false );
	sett.setValue( "View/PdfPreview", false );
	sett.setValue( "View/VideoPreview", false );
	sett.setValue( "View/ePubPreview", false );

	/* SidePanel */
	sett.setValue( "SidePanel", p7gb1->isChecked() );
	sett.setValue( "SidePanelType", p7btnGrp1->checkedId() );

	/* InfoBar/InfoPanel */
	if ( p8btnGrp1->checkedId() )
		sett.setValue( "InfoPanel", true );
	else
		sett.setValue( "InfoPanel", false );

	/* InfoBar/InfoPanel */
	if ( p9btn1->isChecked() ) {
		sett.setValue( "SpecialOpen", true );
		sett.setValue( "OpenWithCatalog", false );
		sett.setValue( "SuperStart", true );
	}

	else {
		sett.setValue( "SpecialOpen", false );
		sett.setValue( "OpenWithCatalog", false );
		sett.setValue( "SuperStart", false );
	}

	/* TrayIcon */
	sett.setValue( "TrayIcon", p10cb1->isChecked() );

	/* Terminals */
	sett.setValue( "Terminals/Default", p11lw1->currentIndex().data() );

	/* NB3 Specials */
	sett.setValue( "View/PaintOverlay", p12cb1->isChecked() );
	sett.setValue( "ExtendedIO", p12cb2->isChecked() );

	/* Misc features */
	sett.setValue( "AutoMount", false );
	sett.setValue( "LogDebug", false );

	/* Generate default KeyBindings */
	saveDefaultKeys();

	/* Sync the settings */
	sett.sync();

	close();
};

void NBStartupWizard::saveDefaultKeys() {

	QSettings keys( ":/data/NewBreeze.conf", QSettings::IniFormat );

	keys.beginGroup( "Shortcuts" );
	sett.beginGroup( "Shortcuts" );

	Q_FOREACH( QString key, keys.childKeys() )
		sett.setValue( key, keys.value( key ) );

	sett.endGroup();
	keys.endGroup();
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

void NBStartupWizard::disableCancelOnLastPage( int id ) {

	if ( ( id >= 0 ) and page( id )->isFinalPage() )
		button( QWizard::CancelButton )->setDisabled( true );

	else
		button( QWizard::CancelButton )->setEnabled( true );
};

void NBStartupWizard::addNewTerminal() {

	NBAddVTEDialog *vteDlg = new NBAddVTEDialog( this );
	vteDlg->exec();

	/* Reload the detected terminals list */
	p11lw1->clear();
	QStringList terms = QStringList() << "Inbuilt";
	Q_FOREACH( QString key, sett.allKeys() ) {
		if ( key.startsWith( "Terminals/" ) and ( key != "Terminals/Default" ) ) {
			terms << key.replace( "Terminals/", "" );
		}
	}
	p11lw1->addItems( terms );
	p11lw1->setCurrentRow( terms.indexOf( sett.value( "Terminals/Default", "Inbuilt" ).toString() ) );
};
