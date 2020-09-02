/*
	*
	* NBXdg.cpp - NBXdgimplementation for NewBreeze
	*
*/

#include "NBXdg.hpp"
#include "NBTools.hpp"

/* We assume that the home partition is always mounted */
QString NBXdg::homePartition = NBDeviceInfo( QDir::homePath() ).mountPoint();

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
	files << NBXdg::home() + QString( "/.config/mimeapps.list" );							// User overrides
	files << NBXdg::home() + QString( "/.local/share/applications/mimeapps.list" );			// User default
	files << QString( "/usr/local/share/applications/mimeapps.list" );						// System alternate
	files << QString( "/etc/xdg/mimeapps.list" );											// System override
	files << QString( "/usr/share/applications/mimeapps.list" );							// System default

	QStringList caches;
	caches << NBXdg::home() + QString( "/.local/share/applications/mimeinfo.cache" );
	caches << QString( "/usr/local/share/applications/mimeinfo.cache" );
	caches << QString( "/usr/share/applications/mimeinfo.cache" );

	QString defaultValue;
	Q_FOREACH( QString file, files ) {
		QSettings defaults( file, QSettings::IniFormat );
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

	if ( defaultValue.isEmpty() and defaultValue.isNull() ) {
		Q_FOREACH( QString cf, caches ) {
			QSettings cache( cf, QSettings::IniFormat );
			defaultValue = cache.value( QString( "MIME Cache/%1" ).arg( mimeType ) ).toString();

			if ( not defaultValue.isEmpty() )
				break;
		}
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
				return xdgDataHome + "/";
		};

		case NBXdg::XDG_CONFIG_HOME : {
			QString xdgConfigHome = QString::fromLocal8Bit( qgetenv( "XDG_CONFIG_HOME" ) );
			if ( xdgConfigHome.isNull() or xdgConfigHome.isEmpty() )
				return NBXdg::home() + QString( "/.config/" );

			else
				return xdgConfigHome + "/";
		};

		case NBXdg::XDG_CACHE_HOME : {
			QString xdgCacheHome = QString::fromLocal8Bit( qgetenv( "XDG_CACHE_HOME" ) );
			if ( xdgCacheHome.isNull() or xdgCacheHome.isEmpty() )
				return NBXdg::home() + QString( "/.cache/" );

			else
				return xdgCacheHome + "/";
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

	/* Check if $HOME and @path are on the same partition, and path begins with $HOME */
	if ( ( homePartition == QStorageInfo( path ).rootPath() + "/" ) and path.startsWith( QDir::homePath() ) ) {

		// Same partition, ensure all the paths exist
		QDir::home().mkpath( ".local/share/Trash/" );
		QDir::home().mkpath( ".local/share/Trash/files/" );
		QDir::home().mkpath( ".local/share/Trash/info/" );

		QFile::setPermissions( QDir::home().filePath( "/.local/share/Trash" ), QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner );
		QFile::setPermissions( QDir::home().filePath( "/.local/share/Trash/files" ), QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner );
		QFile::setPermissions( QDir::home().filePath( "/.local/share/Trash/info" ), QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner );

		return QDir::home().filePath( ".local/share/Trash/" );
	}

	else {
		QString mountPoint = QStorageInfo( path ).rootPath();

		/* If the mount point does not exist, or we have read write issues return a NULL string */
		if ( access( mountPoint.toLocal8Bit().data(), R_OK | W_OK | X_OK ) )
			return QString();

		/* If $MNTPT/.Trash/$UID is present, and accessible with right permissions */
		/* We blindly try to make $MTPT/.Trash/$uid/files, $MTPT/.Trash/$uid/info */
		if( access( ( mountPoint + "/.Trash/" + QString::number( getuid() ) ).toLocal8Bit().data(), R_OK | W_OK | X_OK ) == 0 ) {
			QDir( mountPoint ).mkpath( QString( ".Trash/%1/files/" ).arg( getuid() ) );
			QDir( mountPoint ).mkpath( QString( ".Trash/%1/info/" ).arg( getuid() ) );

			/* Check if the any one above folders exist, say $MTPT/.Trash-$uid/files */
			if( access( ( mountPoint + "/.Trash/" + QString::number( getuid() ) + "/files/" ).toLocal8Bit().data(), R_OK | W_OK | X_OK ) == 0 )
				return mountPoint + "/.Trash/" + QString::number( getuid() ) + "/";
		}

		/* Otherwise we create $MNTPT/.Trash-$UID */
		QDir( mountPoint ).mkpath( QString( ".Trash-%1/" ).arg( getuid() ) );
		QDir( mountPoint ).mkpath( QString( ".Trash-%1/files/" ).arg( getuid() ) );
		QDir( mountPoint ).mkpath( QString( ".Trash-%1/info/" ).arg( getuid() ) );
		QFile::setPermissions( mountPoint + "/.Trash-" + QString::number( getuid() ), QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner );

		/* Check if the any one above folders exist, say $MTPT/.Trash-$uid/files */
		if( access( ( mountPoint + "/.Trash-" + QString::number( getuid() ) + "/files/" ).toLocal8Bit().data(), R_OK | W_OK | X_OK ) == 0 )
			return mountPoint + "/.Trash-" + QString::number( getuid() ) + "/";

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

/*
	*
	* NBDesktopFile
	*
*/

NBDesktopFile::NBDesktopFile( QString filename ) {

	if ( filename.isEmpty() )
		return;

	mFileUrl = NBXdgMime::instance()->desktopPathForName( filename );
	mDesktopName = baseName( filename );

	if ( mFileUrl.startsWith( "/usr/share/applications" ) )
		mRank = 1;

	else if ( mFileUrl.startsWith( "/usr/local/share/applications" ) )
		mRank = 2;

	else if ( mFileUrl.startsWith( QDir::home().filePath( ".local/share/applications" ) ) )
		mRank = 3;

	QSettings s( mFileUrl, QSettings::NativeFormat );
	s.beginGroup( "Desktop Entry" );

	mName = s.value( "Name" ).toString();
	mGenericName = s.value( "GenericName", mName ).toString();
	mDescription = s.value( "Description", "" ).toString();
	mCommand = s.value( "Exec" ).toString();
	mExec = s.value( "TryExec", mCommand.split( " ", QString::SkipEmptyParts ).value( 0 ) ).toString();

	if ( not exists( mExec ) ) {
		Q_FOREACH( QString path, QString::fromLocal8Bit( qgetenv( "PATH" ) ).split( ":", QString::SkipEmptyParts ) ) {
			if ( exists( path + "/" + mCommand ) ) {
				mExec = path + "/" + mCommand;
				break;
			}
		}
	}

	mIcon = s.value( "Icon" ).toString();

	QStringList args = mCommand.split( " " );
	foreach( QString arg, args ) {
		if ( arg == "%f" ) {
			mMultiArgs = false;
			mTakesArgs = true;
			mParsedArgs << "<#COREARG-FILE#>";
		}

		else if ( arg == "%F" ) {
			mMultiArgs = true;
			mTakesArgs = true;
			mParsedArgs << "<#COREARG-FILES#>";
		}

		else if ( arg == "%u" ) {
			mMultiArgs = false;
			mTakesArgs = true;
			mParsedArgs << "<#COREARG-URL#>";
		}

		else if ( arg == "%U" ) {
			mMultiArgs = true;
			mTakesArgs = true;
			mParsedArgs << "<#COREARG-URLS#>";
		}

		else if ( arg == "%i" ) {
			if ( !mIcon.isEmpty() )
				mParsedArgs << "--icon" << mIcon;
		}

		else if ( arg == "%c" )
			mParsedArgs << mName;

		else if ( arg == "%k" )
			mParsedArgs << QUrl( mFileUrl ).toLocalFile();

		else
			mParsedArgs << arg;
	}

	QFile desktop( mFileUrl );
	desktop.open( QFile::ReadOnly );
	QStringList lines = QString::fromLocal8Bit( desktop.readAll() ).split( "\n" );
	Q_FOREACH( QString line, lines ) {
		if ( line.startsWith( "MimeType=" ) )
			mMimeTypes = line.replace( "MimeType=", "" ).split( ";", QString::SkipEmptyParts );

		if ( line.startsWith( "Categories=" ) )
			mCategories = line.replace( "Categories=", "" ).split( ";", QString::SkipEmptyParts );
	}
	desktop.close();

	getCategory();

	mVisible = not s.value( "NoDisplay", false ).toBool();
	mRunInTerminal = s.value( "Terminal", false ).toBool();

	if ( s.value( "Type" ).toString() == "Application" )
		mType = Application;

	else if ( s.value( "Type" ).toString() == "Link" )
		mType = Link;

	else if ( s.value( "Type" ).toString() == "Directory" )
		mType = Directory;

	if ( mName.count() and mCommand.count() )
		mValid = true;

	if ( mDesktopName == "org.kde.okular.desktop" )
		kdeFix( "okular" );
};

bool NBDesktopFile::startApplication() {

	if ( not mValid )
		return false;

	QProcess proc;
	return proc.startDetached( mExec );
};

bool NBDesktopFile::startApplicationWithArgs( QStringList args ) {

	if ( not mValid )
		return false;

	QProcess proc;

	QStringList execList = parsedExec();
	QString exec = execList.takeFirst();

	if ( not args.count() ) {

		execList.removeAll( "<#COREARG-FILES#>" );
		execList.removeAll( "<#COREARG-FILE#>" );
		execList.removeAll( "<#COREARG-URLS#>" );
		execList.removeAll( "<#COREARG-URL#>" );

		return QProcess::startDetached( exec, execList );
	}

	QStringList argList;
	if ( mTakesArgs ) {
		if ( mMultiArgs ) {
			Q_FOREACH( QString exeArg, execList ) {
				if ( exeArg == "<#COREARG-FILES#>" ) {
					if ( args.count() )
						argList << args;
				}

				else if ( exeArg == "<#COREARG-URLS#>" ) {
					if ( args.count() ) {
						Q_FOREACH( QString arg, args )
							argList << ( arg.startsWith( "/" ) ? "file://" : "file:" ) + arg;
					}
				}

				else
					argList << exeArg;
			}
		}

		else {
			if ( exec.indexOf( "<#COREARG-FILE#>" ) ) {
				int idx = exec.indexOf( "<#COREARG-FILE#>" );
				argList << execList;
				argList.removeAt( idx );
				if ( args.count() ) {
					argList.insert( idx, args.takeAt( 0 ) );
					argList << args;
				}
			}

			else {
				int idx = exec.indexOf( "<#COREARG-URL#>" );
				argList << execList;
				argList.removeAt( idx );
				if ( args.count() ) {
					argList.insert( idx, ( args.at( 0 ).startsWith( "/" ) ? "file://" : "file:" ) + args.takeAt( 0 ) );
					Q_FOREACH( QString arg, args )
						argList << ( arg.startsWith( "/" ) ? "file://" : "file:" ) + arg;
				}
			}
		}
	}

	else {
		argList << execList;
		if ( args.count() )
			argList << args;
	}

	return QProcess::startDetached( exec, argList );
};

QString NBDesktopFile::desktopName() const {

	return mDesktopName;
};

QString NBDesktopFile::name() const {

	return mName;
};

QString NBDesktopFile::genericName() const {

	return mGenericName;
};

QString NBDesktopFile::description() const {

	return mDescription;
};

QString NBDesktopFile::exec() const {

	return mExec;
};

QString NBDesktopFile::command() const {

	return mCommand;
};

QString NBDesktopFile::icon() const {

	return mIcon;
};

QString NBDesktopFile::category() const {

	return mCategory;
};

QStringList NBDesktopFile::mimeTypes() const {

	return mMimeTypes;
};

QStringList NBDesktopFile::categories() const {

	return mCategories;
};

QStringList NBDesktopFile::parsedExec() const {

	return mParsedArgs;
};

int NBDesktopFile::type() const{

	return mType;
};

int NBDesktopFile::rank() const {

	return mRank;
};

bool NBDesktopFile::visible() const {

	return mVisible;
};

bool NBDesktopFile::runInTerminal() const {

	return mRunInTerminal;
};

bool NBDesktopFile::takesArgs() const {

	return mTakesArgs;
};

bool NBDesktopFile::multipleArgs() const {

	return mMultiArgs;
};

bool NBDesktopFile::isValid() const {

	return mValid;
};

QString NBDesktopFile::desktopFileUrl() const {

	return mFileUrl;
};

bool NBDesktopFile::operator==( const NBDesktopFile& other ) const {

	bool truth = true;
	truth &= ( mCommand == other.command() );
	truth &= ( mRank == other.rank() );

	return truth;
};

void NBDesktopFile::getCategory() {

	QStringList Accessories = QStringList() << "Utility" << "Utilities" << "Accessory" << "Accessories";
	QStringList Development = QStringList() << "Development";
	QStringList Education = QStringList() << "Education";
	QStringList Games = QStringList() << "Games" << "Game" << "ArcadeGame" << "StrategyGame" << "LogicGame";
	QStringList Graphics = QStringList() << "Graphics";
	QStringList Internet = QStringList() << "Network" << "Internet";
	QStringList Multimedia = QStringList() << "Audio" << "Video" << "AudioVideo" << "Multimedia";
	QStringList Office = QStringList() << "Office";
	QStringList ScienceMath = QStringList() << "Science" << "Math";
	QStringList Settings = QStringList() << "Settings";
	QStringList System = QStringList() << "System";
	QStringList Wine = QStringList() << "Wine";

	/* If the file name and the command contains 'wine', then it's a wine application */
	if ( mFileUrl.contains( "wine", Qt::CaseInsensitive ) and mCommand.contains( "wine", Qt::CaseInsensitive ) ) {
		mCategories = QStringList( { "Wine" } );
		mCategory = "Wine";
		return;
	}

	else {
		mCategory = "Uncategorized";
		foreach( QString cate, mCategories ) {
			if ( Accessories.contains( cate ) )
				mCategory = "Accessories";

			else if ( Development.contains( cate ) )
				mCategory = "Development";

			else if ( ScienceMath.contains( cate ) )
				mCategory = "Science & Math";

			else if ( Education.contains( cate ) )
				mCategory = "Education";

			else if ( Games.contains( cate ) )
				mCategory = "Games";

			else if ( Office.contains( cate ) )
				mCategory = "Office";

			else if ( Graphics.contains( cate ) )
				mCategory = "Graphics";

			else if ( Internet.contains( cate ) )
				mCategory = "Internet";

			else if ( Multimedia.contains( cate ) )
				mCategory = "Multimedia";

			else if ( Settings.contains( cate ) )
				mCategory = "Settings";

			else if ( System.contains( cate ) )
				mCategory = "System";

			else if ( Wine.contains( cate ) )
				mCategory = "Wine";

			else
				continue;
		}
	}
};

void NBDesktopFile::kdeFix( QString appName ) {

	QDirIterator dirIt( "/usr/share/applications/", QDir::Files, QDirIterator::Subdirectories );

	while ( dirIt.hasNext() ) {
		QString desktop = dirIt.next();
		if ( desktop.contains( appName ) ) {
			QFile appFile( desktop );
			appFile.open( QFile::ReadOnly );
			QStringList lines = QString::fromLocal8Bit( appFile.readAll() ).split( "\n" );
			Q_FOREACH( QString line, lines ) {
				if ( line.startsWith( "MimeType=" ) )
					mMimeTypes << line.replace( "MimeType=", "" ).split( ";", QString::SkipEmptyParts );
			}
			appFile.close();
		}
	}
};

/*
	*
	* NBXdgMime
	*
*/

NBXdgMime* NBXdgMime::globalInstance = nullptr;

AppsList NBXdgMime::appsForMimeType( QMimeType mimeType ) {

	AppsList appsForMimeList;
	QStringList mimeList = QStringList() << mimeType.name() << mimeType.allAncestors();
	QSet<QString> mimeSet = mimeList.toSet();

	Q_FOREACH( NBDesktopFile app, appsList ) {
		QSet<QString> intersected = app.mimeTypes().toSet().intersect( mimeSet );
		if ( intersected.count() ) {
			if ( ( app.type() == NBDesktopFile::Application ) and app.visible() )
				appsForMimeList << app;
		}
	}

	QString defaultName = NBXdg::xdgDefaultApp( mimeType.name() );
	for( int i = 0; i < appsForMimeList.count(); i++ ) {
		if ( appsForMimeList.value( i ).desktopName() == baseName( defaultName ) ) {
			appsForMimeList.move( i, 0 );
			break;
		}
	}

	return appsForMimeList;
};

QStringList NBXdgMime::mimeTypesForApp( QString desktopName ) {

	QStringList mimeList;

	if ( not desktopName.endsWith( ".desktop" ) )
		desktopName += ".desktop";

	foreach( QString appDir, appsDirs ) {
		if ( QFile::exists( appDir + desktopName ) ) {
			mimeList << NBDesktopFile( appDir + desktopName ).mimeTypes();
			break;
		}
	}

	return mimeList;
};

AppsList NBXdgMime::allDesktops() {

	return appsList;
};

NBDesktopFile NBXdgMime::application( QString exec ) {

	AppsList list;
	Q_FOREACH( NBDesktopFile app, appsList ) {
		if ( app.command().contains( exec, Qt::CaseSensitive ) )
			list << app;

		else if ( app.name().compare( exec, Qt::CaseInsensitive ) == 0 )
			list << app;
	}

	if ( not list.count() )
		return NBDesktopFile();

	int rank = -1, index = -1;
	for( int i = 0; i < list.count(); i++ ) {
		if ( rank < list.value( i ).rank() ) {
			rank = list.value( i ).rank();
			index = i;
		}
	}

	/* Desktop file with the highest rank will be used always */
	return list.at( index );
};

QString NBXdgMime::desktopPathForName( QString desktopName ) {

	if ( not desktopName.endsWith( ".desktop" ) )
		desktopName += ".desktop";

	if ( exists( desktopName ) )
		return desktopName;

	Q_FOREACH( QString appDirStr, appsDirs ) {
		if ( exists( appDirStr + "/" + desktopName ) ) {
			return appDirStr + "/" + desktopName;
		}
	}

	return QString();
};

NBDesktopFile NBXdgMime::desktopForName( QString desktopName ) {

	if ( not desktopName.endsWith( ".desktop" ) )
		desktopName += ".desktop";

	if ( exists( desktopName ) )
		return NBDesktopFile( desktopName );

	QString desktopPath;
	Q_FOREACH( QString appDirStr, appsDirs ) {
		if ( exists( appDirStr + "/" + desktopName ) ) {
			desktopPath = appDirStr + "/" + desktopName;
			break;
		}
	}

	return NBDesktopFile( desktopPath );
};

void NBXdgMime::parseDesktops() {

	appsList.clear();
	Q_FOREACH( QString appDirStr, appsDirs ) {
		QDir appDir( appDirStr );
		Q_FOREACH( QFileInfo desktop, appDir.entryInfoList( QStringList() << "*.desktop", QDir::Files ) ) {
			appsList << NBDesktopFile( desktop.absoluteFilePath() );
		}
	}
};

NBXdgMime* NBXdgMime::instance() {

	if ( NBXdgMime::globalInstance )
		return globalInstance;

	NBXdgMime::globalInstance = new NBXdgMime();
	globalInstance->parseDesktops();

	return NBXdgMime::globalInstance;
};

void NBXdgMime::setApplicationAsDefault( QString appFileName, QString mimetype ) {

	if ( QProcess::execute( "xdg-mime", QStringList() << "default" << appFileName << mimetype ) )
		qDebug() << "Error while setting" << appFileName << "as the default handler for" << mimetype;
};

NBXdgMime::NBXdgMime() {

	appsDirs << QDir::home().filePath( ".local/share/applications/" );
	appsDirs << "/usr/local/share/applications/" << "/usr/share/applications/";
	appsDirs << "/usr/share/applications/kde4/" << "/usr/share/gnome/applications/";
};

NBDesktopFile NBXdgMime::xdgDefaultApp( QMimeType mimeType ) {

	QStringList mimes;
	mimes << mimeType.name();
	mimes << mimeType.allAncestors();

	NBDesktopFile app;
	Q_FOREACH( QString mimeName, mimes ) {
		app = NBDesktopFile( NBXdg::xdgDefaultApp( mimeName ) );
		if ( app.isValid() )
			return app;
	}

	return NBDesktopFile();
};

uint qHash( const NBDesktopFile &app ) {

	QString hashString;
	hashString += app.name();
	hashString += app.genericName();
	hashString += app.description();
	hashString += app.command();
	hashString += app.icon();
	hashString += app.mimeTypes().join( " " );
	hashString += app.categories().join( " " );

	return qChecksum( hashString.toLocal8Bit().data(), hashString.count() );
};

QVariant& toQVariant( const NBDesktopFile &app ) {

	QVariant appVar;
	appVar.setValue( app );

	return appVar;
};
