/*
	*
	* NBLogger.cpp - Some tools for NewBreeze
	*
*/

#include <NBLogger.hpp>

FILE *nblog;
QString logPath;

void NBMessageOutput( QtMsgType type, const QMessageLogContext &context, const QString &message ) {

	Q_UNUSED( context );

	/*
		*
		* Define nblog
		*
	*/
	QString logPath = QDir( QStandardPaths::writableLocation( QStandardPaths::GenericConfigLocation ) ).filePath( "NewBreeze/newbreeze.log" );
	nblog = fopen( ( Settings->value( "LogDebug", NBSettings::GlobalScope ) ? logPath.toLocal8Bit().data() : "/dev/null" ), "a" );

	if ( not nblog )
		nblog = fopen( "/dev/null", "w" );

	switch ( type ) {
		case QtInfoMsg: {
			fprintf( nblog, "NewBreeze::Debug# %s\n", message.toLocal8Bit().data() );
			fflush( nblog );
			fprintf( stderr, "\033[01;32mNewBreeze::Info# %s\n\033[00;00m", message.toLocal8Bit().data() );
			break;
		}

		case QtDebugMsg: {
			fprintf( nblog, "NewBreeze::Debug# %s\n", message.toLocal8Bit().data() );
			fflush( nblog );
			fprintf( stderr, "\033[01;30mNewBreeze::Debug# %s\n\033[00;00m", message.toLocal8Bit().data() );
			break;
		}

		case QtWarningMsg: {
			fprintf( nblog, "NewBreeze::Warning# %s\n", message.toLocal8Bit().data() );
			fflush( nblog );
			if ( QString( message ).contains( "X Error" ) or QString( message ).contains( "libpng warning" ) )
				break;
			fprintf( stderr, "\033[01;33mNewBreeze::Warning# %s\n\033[00;00m", message.toLocal8Bit().data() );
			break;
		}

		case QtCriticalMsg: {
			fprintf( nblog, "NewBreeze::CriticalError# %s\n", message.toLocal8Bit().data() );
			fflush( nblog );
			fprintf( stderr, "\033[01;31mNewBreeze::CriticalError# %s\n\033[00;00m", message.toLocal8Bit().data() );
			break;
		}

		case QtFatalMsg: {
			fprintf( nblog, "NewBreeze::FatalError# %s\n", message.toLocal8Bit().data() );
			fflush( nblog );
			fprintf( stderr, "\033[01;41mNewBreeze::FatalError# %s\n\033[00;00m", message.toLocal8Bit().data() );
			abort();
		}
	}
};
