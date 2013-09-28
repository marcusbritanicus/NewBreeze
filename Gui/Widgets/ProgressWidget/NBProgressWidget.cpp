/*
	*
	* NBProgressWidget.cpp - The NBProgressWidget class for NewBreeze
	*
*/

#include <NBProgressWidget.hpp>

NBProgressWidget::NBProgressWidget() : QFrame() {

	checked = false;
	jobList.clear();

	progressPopup = new NBProgressPopup();

	totalPC = 0;
	cfilePC = 0;

	painter = new QPainter();
	setFixedSize( QSize( 128, 52 ) );

	timer = new QTimer();
};

NBProgressWidget::~NBProgressWidget() {

	if ( painter->isActive() )
		painter->end();

	delete painter;
};

void NBProgressWidget::addJob( QStringList sourceList, QString target, NBIOMode::Mode iomode, NBJobMode::Mode jobmode ) {

	NBFileIO *fileIO = new NBFileIO( jobList.count() );
	fileIO->setMode( iomode );
	fileIO->setSources( sourceList );
	fileIO->setTarget( target );

	// connect(
		// fileIO, SIGNAL( ioVolume( quint64, quint64, quint64 ) ),
		// this, SLOT( updateInfo( quint64, quint64, quint64 ) )
	// );

	// connect(
		// fileIO, SIGNAL( newNode( QString ) ),
		// this, SIGNAL( newNode( QString ) )
	// );

	// connect(
		// fileIO, SIGNAL( copied( quint64, QString, float, float ) ),
		// this, SLOT( updateProgress( quint64, QString, float, float ) )
	// );

	// connect(
		// fileIO, SIGNAL( complete( quint64, QStringList ) ),
		// this, SLOT( handleJobComplete( quint64, QStringList ) )
	// );

	jobList << fileIO;

	// if ( jobmode == NBJobMode::Queued ) {
		// qDebug() << "Queuing job";
		// connect( this, SIGNAL( allJobsCompleted() ), fileIO, SLOT( start() ) );
	// }

	// else {
		fileIO->start();
		// qDebug() << "Starting IO";
	// }

	// manageTimer();
};

bool NBProgressWidget::cancelJob( quint64 jobID ) {

	NBFileIO *io = jobList.at( jobID );
	io->cancelIO();

	return io->wait( 5000 );
};

bool NBProgressWidget::hasPendingJobs() {

	return false;
};

quint64 NBProgressWidget::pendingJobsCount() {

	return 0;
};

float NBProgressWidget::jobProgress( quint64 jobID ) {

	return jobList.at( jobID )->progress();
};

float NBProgressWidget::totalProgress() {

	float progress = 0;
	foreach( NBFileIO *job, jobList )
		progress += job->progress();

	return progress / jobList.count();
};

quint64 NBProgressWidget::activeJobs() {

	return jobList.count();
};

void NBProgressWidget::paintEvent( QPaintEvent *pEvent ) {

	painter->begin( this );
	painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	painter->setFont( QFont( "Courier 10 Pitch", 10 ) );

	// Draw the frame border
	painter->setPen( Qt::gray );
	painter->setBrush( checked ? QColor( 100, 100, 100, 72 ) : QColor( 0, 0, 0, 72 ) );
	painter->drawRoundedRect( 0, 0, 128, 52, 9, 9 );

	quint64 jobs = jobList.count();

	if ( not jobs ) {
		painter->setBrush( Qt::transparent );
		if ( Settings.General.Style == QString( "TransDark" ) or Settings.General.Style == QString( "DullBlack" ) )
			painter->setPen( Qt::white );

		else
			painter->setPen( Qt::black );

		painter->drawText( 0, 0, 128, 52, Qt::AlignCenter, QString( "No active Jobs" ) );
	}

	else if ( jobs == 1 ) {
		// Draw the total progress indicator
		painter->setPen( Qt::gray );
		painter->setBrush( Qt::darkGreen );
		painter->drawRoundedRect( 5, 6, 118 * totalPC, 17, 5, 5 );

		// Draw the current file progress indicator
		painter->setPen( Qt::gray );
		painter->setBrush( Qt::darkGreen );
		painter->drawRoundedRect( 5, 29, 118 * cfilePC, 17, 5, 5 );
	}

	else {
		// Draw text indicating total active jobs
		painter->setBrush( Qt::transparent );
		if ( Settings.General.Style == QString( "TransDark" ) or Settings.General.Style == QString( "DullBlack" ) )
			painter->setPen( Qt::white );

		else
			painter->setPen( Qt::black );

		painter->drawText( 0, 6, 128, 17, Qt::AlignCenter, QString( "%1 active Jobs" ).arg( jobs ) );

		// Draw the total progress indicator
		painter->setPen( Qt::gray );
		painter->setBrush( Qt::darkGreen );
		painter->drawRoundedRect( 5, 29, 118 * totalPC, 17, 5, 5 );
	}

	painter->end();
	pEvent->accept();
};

void NBProgressWidget::mousePressEvent( QMouseEvent *mEvent ) {

	if ( checked ) {
		pDialog->hide();
		checked = false;
		repaint();
	}

	else {
		if ( jobList.count() ) {
			pDialog->show();
			checked = true;
			repaint();
		}
	}

	mEvent->accept();
};

void NBProgressWidget::updateInfo( quint64 jobID, quint64 totalFiles, quint64 totalBytes ) {

	progressPopup->job( jobID )->updateInfo( totalFiles, totalBytes );
};

void NBProgressWidget::updateProgress( quint64 jobID, QString curFile, float fCopied, float tCopied ) {

	progressPopup->job( jobID )->updateProgress( curFile, fCopied, tCopied );
	totalPC = tCopied;
	cfilePC = fCopied;

	repaint();
};

void NBProgressWidget::handleJobComplete( quint64, QStringList ) {

};

void NBProgressWidget::uncheck() {

	checked = false;
	repaint();
};


NBProgressDisplay::NBProgressDisplay( QStringList sourceList, QString target ) : QWidget() {

	sourceLbl = new QLabel( QString( "%1 (Computing sizes)" ).arg( dirName( sourceList.at( 0 ) ) ) );
	targetLbl = new QLabel( target );

	curFileLbl = new QLabel();

	totalPBar = new QProgressBar();
	totalPBar->setRange( 0, 100 );

	cfilePBar = new QProgressBar();
	cfilePBar->setRange( 0, 100 );

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->addWidget( sourceLbl );
	lyt->addWidget( targetLbl );
	lyt->addWidget( totalPBar );
	lyt->addWidget( curFileLbl );
	lyt->addWidget( cfilePBar );

	setLayout( lyt );
};

void NBProgressDisplay::updateInfo( quint64 totalFiles, quint64 totalBytes ) {

	QString srcTxt = sourceLbl->text();
	QString newTxt = srcTxt.left( srcTxt.count() - 17 );
	sourceLbl->setText( newTxt + QString( "(%1 files, %2)" ).arg( totalFiles ).arg( formatSize( totalBytes ) ) );
};

void NBProgressDisplay::updateProgress( QString curFile, float totalProgress, float cfileProgress ) {

	curFileLbl->setText( curFile );
	totalPBar->setValue( totalProgress );
	cfilePBar->setValue( cfileProgress );
};

void NBProgressWidget::manageTimer() {

	// If there are jobs still running,
	if ( activeJobs() ) {
		// And timer is not active, activate it
		if ( not timer->isActive() )
			timer->start( 500 );

		qDebug() << "Timer activated:" << timer->isActive();
	}

	// If there are no active jobs,
	else {
		// And timer is active, halt it
		if ( timer->isActive() )
			timer->stop();
	}
};

NBProgressPopup::NBProgressPopup() {
};

NBProgressDisplay* NBProgressPopup::job( quint64 ) {

	return 0;
};
