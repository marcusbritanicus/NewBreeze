/*
	*
	* NBXdg.cpp - NBXdgimplementation for NewBreeze
	*
*/

#include <NBXdg.hpp>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

QString NBXdg::home() {

	/* If the env variable HOME is set and its proper, good! */
	QString __home = QString::fromLocal8Bit( qgetenv( "HOME" ) );
	if ( access( __home.toLocal8Bit().data(), R_OK | X_OK ) == 0 )
		return __home + "/";

	/* Otherwise, we divine it from the user ID */
	struct passwd *pwd = getpwuid( getuid() );
	return QString::fromLocal8Bit( pwd->pw_dir ) + "/";
};

QString NBXdg::xdgDefaultApp( QString mimeType ) {

	/*
		*
		* We have the appsForMimeList. Now we need to filter some applications out as per user's choice and get the default value
		* First check mimeapps.list/[Default Associations], then mimeapps.list/[Added Associations]. The entry corresponding to the mimetype in
		* the first case and the first entry in the second case are the user defaults.
		* If the mimetype is not listed, then check mimeinfo.cache
		*
		* Do the same for /usr/local/share/applications and /usr/share/applications
		*
	*/

	QStringList files;
	files << NBXdg::home() + QString( "/.local/share/applications/mimeapps.list" );
	files << NBXdg::home() + QString( "/.local/share/applications/defaults.list" );
	files << NBXdg::home() + QString( "/.local/share/applications/mimeinfo.cache" );

	files << QString( "/usr/local/share/applications/mimeapps.list" );
	files << QString( "/usr/local/share/applications/defaults.list" );
	files << QString( "/usr/local/share/applications/mimeinfo.cache" );

	files << QString( "/usr/share/applications/mimeapps.list" );
	files << QString( "/usr/share/applications/defaults.list" );
	files << QString( "/usr/share/applications/mimeinfo.cache" );

	QString defaultValue;
	foreach( QString file, files ) {
		QSettings defaults( file, QSettings::NativeFormat );
		defaultValue = defaults.value( QString( "Default Applications/%1" ).arg( mimeType ) ).toString();
		if ( defaultValue.isEmpty() )
			defaultValue = defaults.value( QString( "Added Associations/%1" ).arg( mimeType ) ).toString();

		else
			break;

		if ( defaultValue.isEmpty() or defaultValue.isNull() )
			continue;

		else
			break;
	}

	return defaultValue;
};


QString NBXdg::userDir( NBXdg::XdgUserDir pathEnum ) {

	switch( pathEnum ) {
		case NBXdg::XDG_DATA_HOME : {
			QString xdgDataHome = QString::fromLocal8Bit( qgetenv( "XDG_DATA_HOME" ) );
			if ( xdgDataHome.isNull() or xdgDataHome.isEmpty() )
				return NBXdg::home() + QString( "/.local/share/" );

			else
				return xdgDataHome;
		};

		case NBXdg::XDG_CONFIG_HOME : {
			QString xdgConfigHome = QString::fromLocal8Bit( qgetenv( "XDG_CONFIG_HOME" ) );
			if ( xdgConfigHome.isNull() or xdgConfigHome.isEmpty() )
				return NBXdg::home() + QString( "/.config/" );

			else
				return xdgConfigHome;
		};

		case NBXdg::XDG_CACHE_HOME : {
			QString xdgCacheHome = QString::fromLocal8Bit( qgetenv( "XDG_CACHE_HOME" ) );
			if ( xdgCacheHome.isNull() or xdgCacheHome.isEmpty() )
				return NBXdg::home() + QString( "/.cache/" );

			else
				return xdgCacheHome;
		};
	};

	return QString();
};

QStringList NBXdg::systemDirs( NBXdg::XdgSystemDirs pathEnum ) {

	switch( pathEnum ) {
		case NBXdg::XDG_DATA_DIRS : {
			QString xdgDataDirs = QString::fromLocal8Bit( qgetenv( "XDG_DATA_DIRS" ) );
			if ( xdgDataDirs.isNull() or xdgDataDirs.isEmpty() )
				return QStringList() << "/usr/local/share/" << "/usr/share/";

			else
				return xdgDataDirs.split( ":" );
		};

		case NBXdg::XDG_CONFIG_DIRS : {
			QString xdgConfigDirs = QString::fromLocal8Bit( qgetenv( "XDG_CONFIG_DIRS" ) );
			if ( xdgConfigDirs.isNull() or xdgConfigDirs.isEmpty() )
				return QStringList() << "/etc/xdg/";

			else
				return xdgConfigDirs.split( ":" );
		};
	};

	return QStringList();
};

/*
	*
	* freedesktop.org says, check if home and path are on the same partition. The $XDG_DATA_DIR/.Trash is the trash location.
	*
	* If not, check $mountpoint/.Trash/ if it exists, then ( create and ) use $mountpoint/.Trash/$uid
	*	* $mountpoint/.Trash/ should have sticky bit set (S_ISVTX), and should not be a symbolic link
	*   * readLink( $mountpoint/.Trash/ ) == $mountpoint/.Trash/
	* Otherwise, use $mountpoint/.Trash-$uid
	*
*/

QString NBXdg::trashLocation( QString path ) {

	/* Ideally, if path is inside home */
	if ( path.startsWith( home() ) ) {
		return NBXdg::homeTrashLocation();
	}

	else {
		NBDeviceInfo trashDevInfo = NBDeviceManager::deviceInfoForPath( path );

		QString mountPoint = trashDevInfo.mountPoint();

		/* If the mount point does not exist, return a NULL string */
		if ( access( mountPoint.toLocal8Bit().data(), R_OK | W_OK | X_OK ) )
			return QString();

		/* If $MNTPT/.Trash/$UID is present, and accessible with right permissions */
		/* We blindly try to make $MTPT/.Trash/$uid/files, $MTPT/.Trash/$uid/info */
		if( access( ( mountPoint + "/.Trash/" + QString::number( getuid() ) ).toLocal8Bit().data(), R_OK | W_OK | X_OK ) == 0 ) {
			QDir( mountPoint ).mkpath( QString( ".Trash/%1/" ).arg( getuid() ) );
			QDir( mountPoint ).mkpath( QString( ".Trash/%1/files/" ).arg( getuid() ) );
			QDir( mountPoint ).mkpath( QString( ".Trash/%1/info/" ).arg( getuid() ) );

			/* Check if the any one above folders exist, say $MTPT/.Trash-$uid/files */
			if( access( ( mountPoint + "/.Trash/" + QString::number( getuid() ) + "/files/" ).toLocal8Bit().data(), R_OK | W_OK | X_OK ) == 0 )
				return mountPoint + "/.Trash/" + QString::number( getuid() );

			return QString();
		}

		/* Otherwise we create $MNTPT/.Trash-$UID */
		QDir( mountPoint ).mkpath( QString( ".Trash-%1/" ).arg( getuid() ) );
		QDir( mountPoint ).mkpath( QString( ".Trash-%1/files/" ).arg( getuid() ) );
		QDir( mountPoint ).mkpath( QString( ".Trash-%1/info/" ).arg( getuid() ) );
		QFile::setPermissions( mountPoint + "/.Trash-" + QString::number( getuid() ), QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner );

		/* Check if the any one above folders exist, say $MTPT/.Trash-$uid/files */
		if( access( ( mountPoint + "/.Trash-" + QString::number( getuid() ) + "/files/" ).toLocal8Bit().data(), R_OK | W_OK | X_OK ) == 0 )
			return mountPoint + "/.Trash-" + QString::number( getuid() );

		return QString();
	}

	return QString();
};

QString NBXdg::homeTrashLocation() {

	/* If the permissions of Trash folder are right; we assume $trash/files and $trash/info exists */
	if ( access( ( home() + "/.local/share/Trash/" ).toLocal8Bit().data(), R_OK | W_OK | X_OK ) == 0 )
		return home() + "/.local/share/Trash/";

	else {
		/* Try to make the trash folder */
		QDir::home().mkpath( "/.local/share/Trash/" );
		QDir::home().mkpath( "/.local/share/Trash/files/" );
		QDir::home().mkpath( "/.local/share/Trash/info/" );
		QFile::setPermissions( home() + "/.local/share/Trash/", QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner );

		return home() + "/.local/share/Trash/";
	}
};
