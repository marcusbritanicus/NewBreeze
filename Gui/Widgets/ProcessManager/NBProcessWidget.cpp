/*
	*
	* NBProcessManagerGUI.cpp - The NBProcessManagerGUI class for NewBreeze
	*
*/

#include <NBProcessWidget.hpp>

/*
    *
    * ┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
    * │ ┌──────┐  Copying source directory name                                                                 [<] [>] [@] [-] [x] │
    * │ │ ☒ ☐ │  Source: /home/cosmos/.....                                                                                        │
    * │ │ ☐ ☒ │  Target: /home/cosmos/.....                                                                                        │
    * │ └─────-┘  Speed:  31 MiB/s                ETA:  1 hr 24 min 36 sec                                                          │
    * │ [ ═══════════════════════════════════════>                                                                                ] │
    * │ Current File: /home/cosmos/......                                                                                           │
    * │ [ ════════════════════════════════════════════════════════════════════════════════>                                       ] │
    * └─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
    *
*/

NBProcessWidget::NBProcessWidget( QWidget *parent ) : QWidget( parent ) {

	paused = false;
	detailsShown = true;

	iconLbl = new QLabel( this );
	iconLbl->setPixmap( QIcon::fromTheme( NBIconProvider::icon( "abc.iso" ) ).pixmap( 48 ) );
	iconLbl->setFixedSize( QSize( 48, 48 ) );

	titleLbl = new QLabel( "Initializing... ", this );
	titleLbl->setFont( QFont( font().family(), font().pointSize() + 1, QFont::Bold, false ) );

	srcLbl = new QLabel( this );
	srcLbl->setText( "Source: /home/cosmos/Downloads/Others/Iso/" );

	tgtLbl = new QLabel( this );
	tgtLbl->setText( "Target: /home/cosmos" );

	speedLbl = new QLabel( this );
	speedLbl->setText( "Speed: 54 MiB/s" );

	cFileLbl = new QLabel( this );
	cFileLbl->setText( "Current File: /home/cosmos/Downloads/Others/Iso/Mac-SnowLeopard.iso" );

	etcLbl = new QLabel( this );
	etcLbl->setText( "ETC: 10 minutes, 36 seconds" );

	totalPB = new NBProgressBar( this );
	totalPB->setValue( 0.36 );

	cfilePB = new NBProgressBar( this );
	cfilePB->setValue( 0.72 );

	detailsBtn = new QToolButton( this );
	detailsBtn->setIcon( QIcon( ":/icons/arrow-up.png" ) );
	detailsBtn->setIconSize( QSize( 16, 16 ) );
	detailsBtn->setFixedSize( QSize( 20, 20 ) );
	connect( detailsBtn, SIGNAL( clicked() ), this, SLOT( toggleDetails() ) );

	pauseBtn = new QToolButton( this );
	pauseBtn->setIcon( QIcon::fromTheme( "media-playback-pause" ) );
	pauseBtn->setIconSize( QSize( 16, 16 ) );
	pauseBtn->setFixedSize( QSize( 20, 20 ) );
	connect( pauseBtn, SIGNAL( clicked() ), this, SLOT( togglePauseResume() ) );

	closeBtn = new QToolButton( this );
	closeBtn->setIcon( QIcon( ":/icons/delete.png" ) );
	closeBtn->setIconSize( QSize( 16, 16 ) );
	closeBtn->setFixedSize( QSize( 20, 20 ) );
	connect( closeBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addWidget( titleLbl );
	btnLyt->addStretch();
	btnLyt->addWidget( detailsBtn );
	btnLyt->addWidget( pauseBtn );
	btnLyt->addWidget( closeBtn );

	QHBoxLayout *speedLyt = new QHBoxLayout();
	speedLyt->addWidget( speedLbl );
	speedLyt->addWidget( etcLbl );

	QVBoxLayout *lblLyt = new QVBoxLayout();
	lblLyt->addLayout( btnLyt );
	lblLyt->addWidget( srcLbl );
	lblLyt->addWidget( tgtLbl );
	lblLyt->addLayout( speedLyt );

	QHBoxLayout *icnLyt = new QHBoxLayout();
	icnLyt->addWidget( iconLbl );
	icnLyt->addLayout( lblLyt );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addLayout( icnLyt );
	baseLyt->addWidget( totalPB );
	baseLyt->addWidget( cFileLbl );
	baseLyt->addWidget( cfilePB );

	setLayout( baseLyt );
};

void NBProcessWidget::update( NBProgress progress ) {

	s
};

void NBProcessWidget::toggleDetails() {

	if ( detailsShown ) {
		detailsBtn->setIcon( QIcon( ":/icons/arrow-down.png" ) );

		srcLbl->show();
		tgtLbl->hide();
		speedLbl->hide();
		etcLbl->hide();
		cFileLbl->hide();
		cfilePB->hide();

		detailsShown = false;

		setFixedHeight( 95 );
	}

	else {
		detailsBtn->setIcon( QIcon( ":/icons/arrow-up.png" ) );

		srcLbl->show();
		tgtLbl->show();
		speedLbl->show();
		etcLbl->show();
		cFileLbl->show();
		cfilePB->show();

		detailsShown = true;
		setFixedHeight( 180 );
	}
};

void NBProcessWidget::togglePauseResume() {

	if ( paused ) {
		pauseBtn->setIcon( QIcon::fromTheme( "media-playback-pause" ) );
		pauseBtn->setToolTip( "Pause" );

		paused = false;
	}

	else {
		pauseBtn->setIcon( QIcon::fromTheme( "media-playback-start" ) );
		pauseBtn->setToolTip( "Resume" );

		paused = true;
	}
};
