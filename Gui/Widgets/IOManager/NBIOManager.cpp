/*
	*
	* NBIOManager.cpp - The NBIOManager class for NewBreeze
	*
*/

#include <NBIOManager.hpp>

NBIOWidget::NBIOWidget( NBFileIO *ioProc ) {

	io = ioProc;
	connect( io, SIGNAL( IOComplete() ), this, SLOT( close() ) );
	connect( io, SIGNAL( IOComplete() ), this, SLOT( signalRemove() ) );

	paused = false;

	timer = new QTimer();
	timer->setInterval( 500 );
	connect( timer, SIGNAL( timeout() ), this, SLOT( update() ) );
	timer->start();

	speedTimer = new QTimer();
	speedTimer->setInterval( 1000 );
	connect( speedTimer, SIGNAL( timeout() ), this, SLOT( speedCalculator() ) );
	speedTimer->start();

	ttlLbl = new QLabel( ( io->ioMode() == NBIOMode::Copy ? "Copying Files" : "Moving Files" ) );
	srcLbl = new QLabel(  "Source: " + dirName( io->sources().at( 0 ) ) );
	tgtLbl = new QLabel( "Target: " + io->target() );
	speedLbl = new QLabel( "Speed: 0 kBps" );
	etcLbl = new QLabel( "ETC: 00:00:00" );
	cfileLbl = new QLabel( "Current file: " + io->ioTarget );

	toggleDetailsLbl = new NBClickLabel( QPixmap( ":/icons/arrow-up.png" ).scaled( QSize( 16, 16 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	toggleDetailsLbl->setClickable( true );
	connect( toggleDetailsLbl, SIGNAL( clicked() ), this, SLOT( toggleDetails() ) );

	togglePauseResumeLbl = new NBClickLabel( QIcon::fromTheme( "media-playback-pause" ).pixmap( 16 ) );
	togglePauseResumeLbl->setClickable( true );
	connect( togglePauseResumeLbl, SIGNAL( clicked() ), this, SLOT( togglePauseResume() ) );

	closeLbl = new NBClickLabel( QPixmap( ":/icons/delete.png" ).scaled( QSize( 16, 16 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	closeLbl->setClickable( true );
	connect( closeLbl, SIGNAL( clicked() ), this, SLOT( cancelIO() ) );

	totalBar = new NBProgressBar();
	totalBar->setRange( 0, io->totalSize );

	cfileBar = new NBProgressBar();
	cfileBar->setRange( 0, io->fTotalBytes );

	QHBoxLayout *ttlLyt = new QHBoxLayout();
	ttlLyt->addWidget( ttlLbl );
	ttlLyt->addStretch( 0 );
	ttlLyt->addWidget( toggleDetailsLbl );
	ttlLyt->addWidget( togglePauseResumeLbl );
	ttlLyt->addWidget( closeLbl );

	QHBoxLayout *spdLyt = new QHBoxLayout();
	spdLyt->addWidget( speedLbl );
	spdLyt->addWidget( etcLbl );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addLayout( ttlLyt );
	baseLyt->addWidget( srcLbl );
	baseLyt->addWidget( tgtLbl );
	baseLyt->addLayout( spdLyt );
	baseLyt->addWidget( new QLabel( "Total progress" ) );
	baseLyt->addWidget( totalBar );
	baseLyt->addWidget( cfileLbl );
	baseLyt->addWidget( cfileBar );

	QWidget *baseWidget = new QWidget();
	baseWidget->setObjectName( "guiBase" );
	baseWidget->setLayout( baseLyt );

	QHBoxLayout *lyt = new QHBoxLayout( this );
	lyt->addWidget( baseWidget );

	setLayout( lyt );
};

void NBIOWidget::toggleDetails() {

	if ( detailsAreSeen ) {
		toggleDetailsLbl->setPixmap( QPixmap( ":/icons/arrow-down.png" ).scaled( QSize( 16, 16 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

		srcLbl->hide();
		tgtLbl->hide();
		speedLbl->hide();
		etcLbl->hide();
		cfileLbl->hide();
		cfileBar->hide();

		detailsAreSeen = false;

		setFixedHeight( 70 );
	}

	else {
		toggleDetailsLbl->setPixmap( QPixmap( ":/icons/arrow-up.png" ).scaled( QSize( 16, 16 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

		srcLbl->show();
		tgtLbl->show();
		speedLbl->show();
		etcLbl->show();
		cfileLbl->show();
		cfileBar->show();

		detailsAreSeen = true;
		setFixedHeight( 180 );
	}
};

void NBIOWidget::togglePauseResume() {

	if ( paused ) {
		io->resume();
		togglePauseResumeLbl->setPixmap( QIcon::fromTheme( "media-playback-pause" ).pixmap( 16 ) );
		togglePauseResumeLbl->setToolTip( "Pause" );

		paused = false;
	}

	else {
		io->pause();
		togglePauseResumeLbl->setPixmap( QIcon::fromTheme( "media-playback-start" ).pixmap( 16 ) );
		togglePauseResumeLbl->setToolTip( "Resume" );

		paused = true;
	}
};

void NBIOWidget::update() {

	cfileLbl->setText( "Current file: " + io->ioTarget );

	totalBar->setMaximum( io->totalSize );
	cfileBar->setMaximum( io->fTotalBytes );
	totalBar->setValue( io->copiedSize );
	cfileBar->setValue( io->fWritten );
};

void NBIOWidget::speedCalculator() {

	if ( io->totalSize == io->copiedSize )
		return;

	previousSize = currentSize;
	currentSize = io->copiedSize;

	quint64 speed = currentSize - previousSize;
	quint64 rTime = ceil( ( io->totalSize - io->copiedSize ) / speed );
	QString hrs = ( rTime / 3600 ? QString( "%1 hours, " ).arg( rTime / 3600 ) : QString() );
	QString mins = ( ( rTime % 3600 ) / 60 ? QString( "%1 minutes, " ).arg( ( rTime % 3600 ) / 60 ) : QString() );
	QString secs = ( ( rTime % 3600 ) % 60 ? QString( "%1 seconds" ).arg( ( rTime % 3600 ) % 60 ) : QString() );

	speedLbl->setText( QString( "Speed: %1/s" ).arg( formatSize( speed ) ) );
	etcLbl->setText( QString( "ETC: %1%2%3" ).arg( hrs ).arg( mins ).arg( secs ) );
};

void NBIOWidget::cancelIO() {

	/* There is a chance that IO has been paused. We make sure to resume it before cancelling it. */
	io->resume();

	io->cancel();
	close();
};

void NBIOWidget::signalRemove() {

	speedTimer->stop();
	timer->stop();

	emit removeIO( io );
};

NBIOManager::NBIOManager( QList<NBFileIO*> jobList ) : NBDialog( "nxc" ) {

	ioList = jobList;

	QScrollArea *scroller = new QScrollArea();
	scroller->setWidgetResizable( true );
	// scroller->setStyleSheet( "border-top: 1px solid gray;" );

	QWidget *baseWidget = new QWidget();
	baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( QMargins() );

	foreach( NBFileIO *io, jobList ) {
		NBIOWidget *iow = new NBIOWidget( io );
		connect( iow, SIGNAL( removeIO( NBFileIO* ) ), this, SLOT( removeIO( NBFileIO* ) ) );
		baseLyt->addWidget( iow );
	}
	baseLyt->addStretch( 0 );

	baseWidget->setLayout( baseLyt );
	scroller->setWidget( baseWidget );

	QHBoxLayout *scrollLyt = new QHBoxLayout();
	scrollLyt->setContentsMargins( QMargins() );
	scrollLyt->addWidget( scroller );

	setLayout( scrollLyt );
	setWindowModality( Qt::NonModal );

	setDialogTitle( "NewBreeze IO Manager" );
	setDialogIcon( QIcon( ":/icons/newbreeze2.png" ) );

	setMinimumSize( QSize( 800, 500 ) );
};

void NBIOManager::addIO( NBFileIO *io ) {

	ioList << io;

	NBIOWidget *iow = new NBIOWidget( io );
	connect( iow, SIGNAL( removeIO( NBFileIO* ) ), this, SLOT( removeIO( NBFileIO* ) ) );

	baseLyt->insertWidget( ioList.count() - 1, iow );
};

void NBIOManager::showCritical() {

	killIOOnClose = true;
	NBIOManager::show();
};

void NBIOManager::closeEvent( QCloseEvent *cEvent ) {

	/* If this is the last window then we should see if all the IO procs are complete */
	if ( killIOOnClose ) {
		/* If there are no processes, then simple close */
		if ( not ioList.count() ) {
			cEvent->accept();
			return;
		}

		/* Question phrasing based on the number of processes */
		QString question;
		if ( ioList.count() == 1 ) {
			question = QString::fromLocal8Bit(
				"There is still 1 active IO process. Do you want to cancel it and close window? "
				"To cancel press 'Yes'. Click 'No' to let the IO continue."
			);
		}

		else {
			question = QString::fromLocal8Bit(
				"There are still %1 active IO processes. Do you want to cancel all of them and close window? "
				"To cancel press 'Yes'. Click 'No' to let the IO continue."
			).arg( ioList.count() );
		}

		/* This means we have running IO processes. Ask the user if we want to stop the IO */
		int reply = NBMessageDialog::question(
			tr( "Cancel all pending IO?" ),
			question,
			QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Yes << NBMessageDialog::No
		);

		/* If yes, stop them and get out */
		if ( reply == NBMessageDialog::Yes ) {
			Q_FOREACH( NBFileIO *io, ioList )
				io->cancel();
			cEvent->accept();
		}

		/* Otherwise, wait till they complete */
		else {
			cEvent->ignore();
			return;
		}
	}

	cEvent->accept();
};

void NBIOManager::removeIO( NBFileIO *io ) {

	ioList.removeAll( io );
};

NBIOManagerMini::NBIOManagerMini() : QFrame() {

	checked = false;
	jobList.clear();

	totalF = 0;
	ioManager = NULL;

	painter = new QPainter();
	setFixedSize( QSize( 128, 52 ) );

	timer = new QTimer();
	connect( timer, SIGNAL( timeout() ), this, SLOT( manageTimer() ) );
	connect( timer, SIGNAL( timeout() ), this, SLOT( updateProgress() ) );
};

NBIOManagerMini::~NBIOManagerMini() {

	if ( painter->isActive() )
		painter->end();

	delete painter;
};

void NBIOManagerMini::addJob( QStringList sourceList, QString target, NBIOMode::Mode iomode ) {

	if ( iomode == NBIOMode::ACopy ) {
		qDebug() << "Alphabetical copy";
		return;
	}

	NBFileIO *io = new NBFileIO();
	io->setSources( sourceList );
	io->setTarget( target );
	io->setIOMode( iomode );

	connect( io, SIGNAL( IOComplete() ), this, SLOT( handleJobComplete() ) );

	jobList << io;
	QTimer::singleShot( 100, io, SLOT( performIO() ) );

	manageTimer();

	if ( ioManager )
		ioManager->addIO( io );
};

quint64 NBIOManagerMini::activeJobs() {

	return jobList.count();
};

void NBIOManagerMini::showAllIODialogs() {

	if ( ioManager ) {
		ioManager->showCritical();
	}

	else {
		ioManager = new NBIOManager( jobList );
		ioManager->showCritical();
	}
};

void NBIOManagerMini::paintEvent( QPaintEvent *pEvent ) {

	painter->begin( this );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	painter->setFont( QFont( "Courier 10 Pitch", 10 ) );

	// Draw the frame
	painter->setPen( Qt::gray );
	painter->setBrush( checked ? QColor( 100, 100, 100, 72 ) : QColor( 0, 0, 0, 72 ) );
	painter->drawRoundedRect( 0, 0, 128, 52, 9, 9 );

	quint64 jobs = jobList.count();

	if ( not jobs ) {
		painter->setBrush( Qt::transparent );
		if ( Settings->General.Style == QString( "TransDark" ) or Settings->General.Style == QString( "DullBlack" ) )
			painter->setPen( Qt::white );

		else
			painter->setPen( Qt::black );

		painter->drawText( 0, 0, 128, 52, Qt::AlignCenter, QString( "No active Jobs" ) );
	}

	else {
		// Draw text indicating total active jobs
		painter->setBrush( Qt::transparent );
		if ( Settings->General.Style == QString( "TransDark" ) or Settings->General.Style == QString( "DullBlack" ) )
			painter->setPen( Qt::white );

		else
			painter->setPen( Qt::black );

		painter->drawText( 0, 6, 128, 17, Qt::AlignCenter, QString( "%1 active Job%2" ).arg( jobs ).arg( ( jobs == 1 ) ? "" : "s" ) );

		// Draw the total progress indicator
		painter->setPen( Qt::gray );
		painter->setBrush( Qt::darkGreen );
		painter->drawRoundedRect( 5, 29, 118 * totalF, 17, 5, 5 );
	}

	painter->end();
	pEvent->accept();
};

void NBIOManagerMini::mousePressEvent( QMouseEvent *mEvent ) {

	if ( checked ) {
		ioManager->close();
		ioManager = NULL;

		checked = false;
		repaint();
	}

	else {
		ioManager = new NBIOManager( jobList );
		connect( ioManager, SIGNAL( destroyed() ), this, SLOT( uncheck() ) );
		ioManager->show();

		checked = true;
		repaint();
	}

	mEvent->accept();
};

void NBIOManagerMini::uncheck() {

	checked = false;
	repaint();
};

void NBIOManagerMini::manageTimer() {

	// If there are jobs still running,
	if ( activeJobs() ) {
		// And timer is not active, activate it
		if ( not timer->isActive() )
			timer->start( 1000 );
	}

	// If there are no active jobs,
	else {
		// And timer is active, halt it
		if ( timer->isActive() )
			timer->stop();
	}
};

void NBIOManagerMini::updateProgress() {

	qreal totalP = 0;
	if ( activeJobs() ) {
		foreach( NBFileIO *io, jobList ) {
			totalP += io->copiedSize * 1.0 / io->totalSize;
		}
		totalF = totalP / jobList.count();
	}

	manageTimer();
	repaint();
};

void NBIOManagerMini::handleJobComplete() {

	NBFileIO *io = qobject_cast<NBFileIO*>( sender() );
	QStringList errors = io->errors();
	NBIOMode::Mode mode = io->ioMode();

	if ( errors.count() ) {
		QString title = QString( "Error %1 files" ).arg( ( mode == NBIOMode::Copy ) ? "copying" : "moving" );
		QString text = QString(
			"<p>Some errors were encountered while %1 the files. Please check the copied data.</p>"		\
			"<p>These errors were mostly caused due to insufficient permissions or invalid characters "	\
			"in files names. You can rectify these problems and try again.</p>"
		).arg( ( mode == NBIOMode::Copy ) ? "copying" : "moving" );

		QListWidget *errorList = new QListWidget();
		errorList->setFocusPolicy( Qt::NoFocus );
		errorList->setIconSize( QSize( 32, 32 ) );

		foreach( QString errorNode, errors ) {
			QString iconName = NBIconProvider::icon( errorNode );
			QListWidgetItem *item = new QListWidgetItem( QIcon::fromTheme( iconName, QIcon( iconName ) ), errorNode );
			errorList->addItem( item );
		}

		NBMessageDialog::error( title, text, QList<NBMessageDialog::StandardButton>() << NBMessageDialog::Ok, errorList );
	}

	jobList.removeOne( io );

	manageTimer();
	repaint();
};
