/*
	*
	* NBAutoMount.cpp - NewBreeze UDisks2 AutoMount  Class
	*
*/

#include "NBAutoMount.hpp"

NBAutoMount *NBAutoMount::autoMount = NULL;

NBAutoMount *NBAutoMount::instance() {

	if ( autoMount != NULL and autoMount->init )
		return autoMount;

	/* Init our process manager. */
	autoMount = new NBAutoMount();

	return autoMount;
};

NBAutoMount::NBAutoMount() : QObject() {

	UDI = new UDisks2();
	init = true;
	running = false;
};

NBAutoMount::~NBAutoMount() {

	stop();

	delete UDI;
};

void NBAutoMount::start() {

	if ( running )
		return;

	Q_FOREACH( QString bDev, UDI->blockDevices() ) {

		UDisks2Block *block = new UDisks2Block( bDev );
		if ( block ) {
			UDisks2Filesystem *ufs = block->fileSystem();
			if ( ufs )
				ufs->mount();
		}
	}

	connect( UDI, SIGNAL( filesystemAdded( const QString& ) ), this, SLOT( mountFS( QString ) ) );
	running = true;

	qDebug() << "Starting Volume AutoMount manager...";
};

void NBAutoMount::stop() {

	disconnect( UDI, SIGNAL( filesystemAdded( const QString& ) ), this, SLOT( mountFS( QString ) ) );
	running = false;
};

void NBAutoMount::mountFS( QString node ) {

	UDisks2Filesystem *ufs = new UDisks2Filesystem( node );
	if ( ufs )
		ufs->mount();
};
