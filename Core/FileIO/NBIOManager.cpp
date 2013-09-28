/*
	*
	* NBIOManagerManager.cpp - NewBreeze FileIO Manager
	*
*/

#include <NBIOManager.hpp>
#include <NBTools.hpp>
#include <NBDeleteManager.hpp>

NBIOManager::NBIOManager() : QObject() {

	jobList.clear();

	timer = new QTimer();
	connect( timer, SIGNAL( timeout() ), this, SLOT( progressClick() ) );
	connect( this, SIGNAL( allJobsCompleted() ), this, SLOT( manageTimer() ) );
};

quint64 NBIOManager::addJob( QStringList sourceList, QString target, NBIOMode::Mode iomode, NBJobMode::Mode jobmode ) {

	NBFileIO *fileIO = new NBFileIO( jobList.count() );
	fileIO->setMode( iomode );
	fileIO->setSources( sourceList );
	fileIO->setTarget( target );

	connect(
		fileIO, SIGNAL( ioVolume( quint64, quint64, quint64 ) ),
		this, SIGNAL( ioVolume( quint64, quint64, quint64 ) )
	);

	connect(
		fileIO, SIGNAL( newNode( QString ) ),
		this, SIGNAL( newNode( QString ) )
	);

	connect(
		fileIO, SIGNAL( copied( quint64, QString, float, float ) ),
		this, SIGNAL( copied( quint64, QString, float, float ) )
	);

	connect(
		fileIO, SIGNAL( complete( quint64, QStringList, QString, QStringList ) ),
		this, SIGNAL( jobComplete( quint64, QStringList, QString, QStringList ) )
	);

	jobList << fileIO;

	if ( jobmode == NBJobMode::Queued ) {
		qDebug() << "Queuing job";
		connect( this, SIGNAL( allJobsCompleted() ), fileIO, SLOT( start() ) );
	}

	else {
		fileIO->start();
		qDebug() << "Starting IO";
	}

	manageTimer();

	emit jobAdded( fileIO->jobID(), sourceList, target, iomode );
	return fileIO->jobID();
};

bool NBIOManager::cancelJob( quint64 jobID ) {

	NBFileIO *io = jobList.at( jobID );
	io->cancelIO();

	return io->wait( 5000 );
};

float NBIOManager::jobProgress( quint64 jobID ) {

	return jobList.at( jobID )->progress();
};

float NBIOManager::totalProgress() {

	float progress = 0;
	foreach( NBFileIO *job, jobList )
		progress += job->progress();

	return ( progress /  jobList.count() );
};

quint64 NBIOManager::activeJobs() {

	quint64 active = 0;
	foreach( NBFileIO* job, jobList ) {
		if ( job->isRunning() )
			active++;
	}

	return active;
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

void NBIOManager::progressClick() {

	float progress = 0;
	foreach( NBFileIO *job, jobList )
		progress += job->progress();

	emit copied( progress / jobList.count() );
};
