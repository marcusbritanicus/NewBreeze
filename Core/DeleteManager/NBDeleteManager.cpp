/*
	*
	* NBDeleteManager.cpp - NewBreeze File/Folder Deleter Class
	*
*/

#include <NBDeleteManager.hpp>
#include <NBMessageDialog.hpp>

NBDeleteManager::NBDeleteManager( QObject *parent, bool trash ) : QObject( parent ) {

	thread = new QThread;
	deleter = new NBDeleter;
	trasher = new NBTrasher;

	if ( trash == true ) {
		trasher->moveToThread( thread );
		connect(
			trasher, SIGNAL( completed( QStringList, QStringList ) ),
			this, SLOT( failureHandler( QStringList, QStringList ) )
		);
	}

	else {
		deleter->moveToThread( thread );
		connect(
			deleter, SIGNAL( completed( QStringList, QStringList ) ),
			this, SLOT( failureHandler( QStringList, QStringList ) )
		);
	}

	thread->start();
};

void NBDeleteManager::deleteFromDisk( QStringList deleteList ) {

	QMetaObject::invokeMethod( deleter, "deleteFilesAndFolders", Qt::QueuedConnection, Q_ARG( QStringList, deleteList ) );
};

void NBDeleteManager::sendToTrash( QStringList trashList ) {

	Q_UNUSED( trashList );

	// QMetaObject::invokeMethod( trasher, "trashFilesAndFolders", Qt::QueuedConnection, Q_ARG( QStringList, trashList ) );
	NBMessageDialog::information(
		"Sorry: Unimplemented Function",
		"I'm extremely sorry about this, but it seems that NewBreeze was released without fixing this issue. Please contact me at "		\
		"marcusbritanicus@gmail.com to know about the fix." );
};

void NBDeleteManager::failureHandler( QStringList failedFiles, QStringList failedDirs ) {

	emit deleteOperationComplete( failedFiles, failedDirs );
};

NBDeleter::NBDeleter( QObject *parent ) : QObject( parent ) {

};

void NBDeleter::removeDir( QString dirName ) {

	QDir dir( dirName);

    if ( dir.exists( dirName ) ) {
		QDir::Filters dFilters = QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files;
        Q_FOREACH( QFileInfo info, dir.entryInfoList( dFilters, QDir::DirsFirst ) ) {
            if ( info.isDir() )
                removeDir( info.absoluteFilePath() );

            else
                removeFile( info.absoluteFilePath() );
        }

        if ( not dir.rmdir( dirName ) )
			failedDirs << dirName;
    }
};

void NBDeleter::removeFile( QString fileName ) {

	if ( not QFile::remove( fileName ) )
		failedFiles << fileName;
};

void NBDeleter::deleteFilesAndFolders( QStringList deleteList ) {

	foreach( QString item, deleteList ) {
		if ( QFileInfo( item ).isDir() )
			removeDir( item );

		else
			removeFile( item );
	}

	emit completed( failedFiles, failedDirs );
};

NBTrasher::NBTrasher( QObject *parent ) : QObject( parent ) {

};

void NBTrasher::removeDir( QString dirName ) {

	QDir dir( dirName);

    if ( dir.exists( dirName ) ) {
        Q_FOREACH( QFileInfo info, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst ) ) {
            if ( info.isDir() )
                removeDir( info.absoluteFilePath() );

            else
                removeFile( info.absoluteFilePath() );
        }

        if ( not dir.rmdir( dirName ) )
			failedDirs << dirName;
    }
};

void NBTrasher::removeFile( QString fileName ) {

	if ( not QFile::remove( fileName ) )
		failedFiles << fileName;
};

void NBTrasher::trashFilesAndFolders( QStringList deleteList ) {

	foreach( QString item, deleteList ) {
		if ( QFileInfo( item ).isDir() )
			removeDir( item );

		else
			removeFile( item );
	}

	emit completed( failedFiles, failedDirs );
};
