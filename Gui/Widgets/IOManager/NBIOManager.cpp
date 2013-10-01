/*
	*
	* NBIOManager.cpp - The NBIOManager class for NewBreeze
	*
*/

#include <NBIOManager.hpp>

NBIOManager::NBIOManager() : QFrame() {

	checked = false;
	jobList.clear();

	threadPool = QThreadPool::globalInstance();
	threadPool->setMaxThreadCount( Settings.General.MaxIOJobs );

	progressPopup = new NBProgressPopup();

	totalF = 0;
	cfileF = 0;

	painter = new QPainter();
	setFixedSize( QSize( 128, 52 ) );

	timer = new QTimer();
	connect( timer, SIGNAL( timeout() ), this, SLOT( manageTimer() ) );
	connect( timer, SIGNAL( timeout() ), this, SLOT( updateProgress() ) );
};

NBIOManager::~NBIOManager() {

	if ( painter->isActive() )
		painter->end();

	delete painter;
};

void NBIOManager::addJob( QStringList sourceList, QString target, NBIOMode::Mode iomode ) {

	/*
		*
		*  Calculate the Job ID
		*
		* JobID is the MD5 has of the sourceDir + target + QString( datetime )
		* This a gauranteed to produce a unique string
		*
	*/
	QString sourceDir = dirName( sourceList.at( 0 ) );
	QString datetime = QString::number( QDateTime::currentDateTime().toTime_t() );

	QString jobid = QString::fromStdString( MD5( qPrintable( sourceDir + target + datetime ) ).hexdigest() );

	NBFileIO *fileIO = new NBFileIO();
	fileIO->setMode( iomode );
	fileIO->setSources( sourceList );
	fileIO->setTarget( target );
	fileIO->setJobID( jobid );

	connect(
		fileIO, SIGNAL( complete( QString, QStringList ) ),
		this, SLOT( handleJobComplete( QString, QStringList ) )
	);

	jobList[ jobid ] = fileIO;

	manageTimer();

	threadPool->start( fileIO );
};

bool NBIOManager::cancelJob( QString jobID ) {

	NBFileIO *io = jobList.value( jobID );
	io->cancelIO();

	return io->wait( 5000 );
};

bool NBIOManager::hasPendingJobs() {

	return false;
};

quint64 NBIOManager::pendingJobsCount() {

	return 0;
};

float NBIOManager::jobProgress( QString jobID ) {

	return jobList.value( jobID )->totalProgress();
};

float NBIOManager::totalProgress() {

	float progress = 0;
	foreach( QString id, jobList.keys() )
		progress += jobList.value( id )->totalProgress();

	return progress / jobList.count();
};

int NBIOManager::activeJobs() {

	return threadPool->activeThreadCount();
};

void NBIOManager::paintEvent( QPaintEvent *pEvent ) {

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
		painter->drawRoundedRect( 5, 6, 118 * totalF, 17, 5, 5 );

		// Draw the current file progress indicator
		painter->setPen( Qt::gray );
		painter->setBrush( Qt::darkGreen );
		painter->drawRoundedRect( 5, 29, 118 * cfileF, 17, 5, 5 );
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
		painter->drawRoundedRect( 5, 29, 118 * totalF, 17, 5, 5 );
	}

	painter->end();
	pEvent->accept();
};

void NBIOManager::mousePressEvent( QMouseEvent *mEvent ) {

	if ( checked ) {
		// pDialog->hide();
		checked = false;
		repaint();
	}

	else {
		if ( jobList.count() ) {
			// pDialog->show();
			checked = true;
			repaint();
		}
	}

	mEvent->accept();
};

void NBIOManager::uncheck() {

	checked = false;
	repaint();
};

void NBIOManager::manageTimer() {

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

void NBIOManager::updateProgress() {

	float fracTotals = 0;
	QStringList jobIDs = jobList.keys();

	if ( activeJobs() ) {
		foreach( QString id, jobIDs ) {
			NBFileIO *io = jobList.value( id );
			fracTotals += io->totalProgress();
			cfileF = io->cfileProgress();
		}
	}

	totalF = fracTotals / jobIDs.count();

	repaint();
};

void NBIOManager::handleJobComplete( QString jobID, QStringList errorNodes ) {

	Q_UNUSED( errorNodes );

	jobList.value( jobID )->deleteLater();
	jobList.remove( jobID );
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

NBProgressPopup::NBProgressPopup() {
};
