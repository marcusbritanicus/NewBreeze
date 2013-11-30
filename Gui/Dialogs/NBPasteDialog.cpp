/*
	*
	* NBPasteDialog.cpp - NewBreeze PasteDialog Class
	*
*/

#include <NBPasteDialog.hpp>
#include <sys/syscall.h>

#define COPY_BUF_SIZE ( 64 * 1024 )

NBPasteDialog::NBPasteDialog() : NBDialog( QString( "n" ) ) {

	wasCanceled = false;
	isPaused = false;

	totalSize = 0;
	copiedSize = 0;

	createGUI();
	createAndSetupActions();
	setWindowProperties();
}

QStringList NBPasteDialog::sources() {

	return srcList;
};

void NBPasteDialog::setSources( QStringList sourceList ) {

	srcList.clear();
	srcList << sourceList;
};

QString NBPasteDialog::target() {

	return tgtDir;
};

void NBPasteDialog::setTarget( QString target ) {

	tgtDir = QString( target );
};

NBIOMode::Mode NBPasteDialog::ioMode() {

	return mode;
};

void NBPasteDialog::setIOMode( NBIOMode::Mode iomode ) {

	mode = iomode;
	if ( mode == NBIOMode::Copy )
		nameLbl->setText( "Moving files... ( Computing sizes )" );

	else
		nameLbl->setText( "Copying files... ( Computing sizes )" );
};

qreal NBPasteDialog::progress() {

	return totalPB->value();
};

QStringList NBPasteDialog::errorNodes() {

	return errors;
};

void NBPasteDialog::createGUI() {

	progressCheckTimer = new QTimer( this );

	QVBoxLayout *mainLyt = new QVBoxLayout();
	mainLyt->setContentsMargins( QMargins() );
	QHBoxLayout *btnLyt = new QHBoxLayout();

	nameLbl = new QLabel();
	fromToLbl = new QLabel();

	totalPB = new QProgressBar();
	totalPB->setRange( 0, 100 );
	filePB = new QProgressBar();
	filePB->setRange( 0, 100 );

	pauseBtn = new QPushButton( QIcon::fromTheme( "media-playback-pause" ), "&Pause" );
	pauseBtn->setFocusPolicy( Qt::ClickFocus );

	cancelBtn = new QPushButton( QIcon( ":/icons/cancel.png" ), "&Cancel" );
	cancelBtn->setObjectName( "cancelBtn" );
	cancelBtn->setFocusPolicy( Qt::ClickFocus );

	btnLyt->addWidget( pauseBtn );
	btnLyt->addStretch( 0 );
	btnLyt->addWidget( cancelBtn );

	mainLyt->addWidget( nameLbl );
	mainLyt->addWidget( new QLabel( "Total progress:" ) );
	mainLyt->addWidget( totalPB );
	mainLyt->addWidget( fromToLbl );
	mainLyt->addWidget( filePB );
	mainLyt->addLayout( btnLyt );

	setLayout( mainLyt );
}

void NBPasteDialog::createAndSetupActions() {

	connect( progressCheckTimer, SIGNAL( timeout() ), this, SLOT( setProgress() ) );

	connect( pauseBtn, SIGNAL( clicked() ), this, SLOT( togglePauseResume() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( setCanceled() ) );
}

void NBPasteDialog::setWindowProperties() {

	if ( mode == NBIOMode::Copy )
		setDialogTitle( "NewBreeze - Copying files" );

	else
		setDialogTitle( "NewBreeze - Moving files" );

	setDialogIcon( QIcon( ":/icons/newbreeze.png" ) );
	setWindowModality( Qt::NonModal);

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 450 ) / 2 );
	int vpos = ( int )( ( dw.height() - 240 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 450, 240 );
}

void NBPasteDialog::startWork() {

	qApp->processEvents();

	preCopy();

	if ( mode == NBIOMode::Copy )
		nameLbl->setText( tr( "Copying files (%1)" ).arg( formatSize( totalSize ) ) );

	else
		nameLbl->setText( tr( "Moving files (%1)" ).arg( formatSize( totalSize ) ) );
}

void NBPasteDialog::togglePauseResume() {

	if ( isPaused ) {
		isPaused = false;
		pauseBtn->setText( "Pause" );
		pauseBtn->setIcon( QIcon::fromTheme( "media-playback-pause" ) );
	}

	else {
		isPaused = true;
		pauseBtn->setText( "Resume" );
		pauseBtn->setIcon( QIcon::fromTheme( "media-playback-start" ) );
	}
};

void NBPasteDialog::setCanceled() {

	nameLbl->setText( "Cancelling...." );
	fromToLbl->setText( "" );

	totalPB->reset();
	filePB->reset();

	update();
	qApp->processEvents();

	wasCanceled = true;
};

void NBPasteDialog::handleFinished( int exitCode, QProcess::ExitStatus exitStatus ) {

	if ( exitStatus == QProcess::NormalExit ) {
		if ( exitCode ) {
			QString title = QString( "Access Error" );
			QString text = tr( "<html><body><p>An error was encountered while copy the file<br>" \
				"<b><tt>%1</tt></b><br><center>to</center><br><b><tt>%1</tt></b><br>" \
				"This may be because of unsufficient permissions. Please check that " \
				"the folder has write permissions and the disk is not full.</p>" \
				"</body></html>" ).arg( srcFile).arg( tgtFile );

			NBMessageDialog::error( title, text, QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok );
			return;
		}
	}
};

void NBPasteDialog::preCopy() {

	qApp->processEvents();
};

void NBPasteDialog::performIO() {

	emit IOComplete();
};

void NBPasteDialog::createDirTree() {

};

void NBPasteDialog::copyFile() {

};

void NBPasteDialog::setProgress() {

	qApp->processEvents();
};
