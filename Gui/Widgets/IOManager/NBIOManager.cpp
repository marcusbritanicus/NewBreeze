/*
	*
	* NBIOManager.cpp - The NBIOManager class for NewBreeze
	*
*/

#include <NBIOManager.hpp>

NBIOManager::NBIOManager() : QFrame() {

	checked = false;
	jobList.clear();

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

	Job newJob;
	newJob.jobID = QString::fromStdString( MD5( qPrintable( sourceDir + target + datetime ) ).hexdigest() );
	newJob.sources << sourceList;
	newJob.target = QString( target );
	newJob.mode = iomode;

	// if ( jobList.count() > Settings.General.MaxIOJobs )
		// pendingJobs << newJob;

	// else
	jobList[ newJob.jobID ] = newJob;
	QtConcurrent::run( performIO, &newJob );

	manageTimer();
};

bool NBIOManager::cancelJob( QString jobID ) {

	Job _job = jobList.value( jobID );
	_job.canceled = true;

	return true;
};

bool NBIOManager::hasPendingJobs() {

	return false;
};

quint64 NBIOManager::pendingJobsCount() {

	return 0;
};

qreal NBIOManager::jobProgress( QString jobID ) {

	if ( jobList.value( jobID ).totalBytes > 0 )
		return jobList.value( jobID ).totalBytesCopied / jobList.value( jobID ).totalBytes;

	else
		return 0;
};

qreal NBIOManager::totalProgress() {

	qreal progress = 0;
	foreach( QString id, jobList.keys() )
		progress += jobProgress( id );

	return progress / jobList.count();
};

int NBIOManager::activeJobs() {

	return jobList.count();
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
	}

	// If there are no active jobs,
	else {
		// And timer is active, halt it
		if ( timer->isActive() )
			timer->stop();
	}
};

void NBIOManager::updateProgress() {

	QStringList jobIDs = jobList.keys();

	if ( activeJobs() ) {
		totalF = totalProgress();
		foreach( QString id, jobIDs ) {
			if ( jobList.value( id ).completed )
				QTimer::singleShot( 100, this, SLOT( handleJobComplete( jobList.value( id ) ) ) );

			else
				cfileF = 1.0 * jobList.value( id ).cfileBytes / jobList.value( id ).cfileBytesCopied;
		}
	}

	manageTimer();
	repaint();
};

void NBIOManager::handleJobComplete( Job *job ) {

	/*
		*
		* This must be used to show the error dialog. Currently we
		* are struggling with other issues, so we are skipping it.
		*
	*/
	#warning "Show the error dialog";

	jobList.remove( job->jobID );
	manageTimer();
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

void NBProgressDisplay::updateProgress( QString curFile, qreal totalProgress, qreal cfileProgress ) {

	curFileLbl->setText( curFile );
	totalPBar->setValue( totalProgress );
	cfilePBar->setValue( cfileProgress );
};

NBProgressPopup::NBProgressPopup() {
};
