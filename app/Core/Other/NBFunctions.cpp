/*
	*
	* NBFunction.hpp - NBFunction.cpp header
	*
*/

#include "NBFunctions.hpp"

QStringList getTerminal() {

	QSettings sett( "NewBreeze", "NewBreeze" );

	QString defTerm = sett.value( "Terminals/Default", "Inbuilt" ).toString();
	/* If we have an terminal plugin, and it is the default */
	if ( defTerm == "Inbuilt" or not defTerm.count() )
		return QStringList() << "Inbuilt";

	/* We have set something else as the default terminal */
	if ( sett.value( "Terminals/" + defTerm ).toStringList().count() )
		return sett.value( "Terminals/" + defTerm ).toStringList();

	return QStringList();
};

QString getAppImagePath() {

	/* Frist argument is app exec */
	QString appExec = QFileInfo( qApp->arguments().at( 0 ) ).fileName();

	QProcess proc;
	proc.start( "mount" );

	proc.waitForReadyRead( 5000 );
	QStringList mounts = QString::fromUtf8( proc.readAll() ).split( "\n" );

	Q_FOREACH( QString mount, mounts ) {
		if ( mount.startsWith( appExec ) and mount.contains( "fuse." + appExec ) ) {
			/* Typically: NB3.App on /tmp/.mount_NB3AppmLkuDL type fuse.NB3App (ro,nosuid,nodev,user=1000) */
			QStringList list = mount.split( QRegExp( "\\s+" ), QString::SkipEmptyParts );
			return list.value( 2 );
		}
	}

	return QString();
};
