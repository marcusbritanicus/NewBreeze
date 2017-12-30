/*
	*
	* NBAppFile.cpp - NewBreeze Application File Class
	*
*/

#include "NBAppFile.hpp"

inline QString findIn( QString what, QString where ) {

	QRegExp rx( what );
	rx.setMinimal( true );

	if ( rx.indexIn( where ) >= 0 )
		return rx.cap( 1 );

	return QString();
};

inline QString dirName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	return QString( dirname( strdup( path.toLocal8Bit().data() ) ) ) + "/";
};

inline QString baseName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	return QString( basename( strdup( path.toLocal8Bit().data() ) ) );
};

NBAppFile::NBAppFile() {

	mIsValid = false;
};

NBAppFile::NBAppFile( QString path ) {

	mIsValid = false;

	if ( exists( path ) ) {

		fileUrl = path;
	}

	else {
		if ( exists( NBXdg::home() + "/.local/share/applications/" + path ) )
			fileUrl = NBXdg::home() + "/.local/share/applications/" + path;

		else if ( exists( "/usr/local/share/applications/" + path ) )
			fileUrl = "/usr/local/share/applications/" + path;

		else if ( exists( "/usr/share/applications/" + path ) )
			fileUrl = "/usr/share/applications/" + path;

		else
			return;
	}

	parseDesktopFile();
};

QVariant NBAppFile::value( NBAppFile::Fields field ) const {

	switch( field ) {
		case Name:
			return __name;

		case Type:
			return __type;

		case Exec:
			return __exec;

		case Icon:
			return __icon;

		case MimeTypes:
			return __mimeTypes;

		case WorkPath:
			return __workPath;

		case TerminalMode:
			return __terminalMode;

		case Categories:
			return __categories;

		case Comment:
			return __comment;

		case NoDisplay:
			return __nodisplay;

		default:
			return QVariant();
	};
};

QStringList NBAppFile::execArgs() const {

	return __execArgs;
};

bool NBAppFile::multipleArgs() const {

	return __multipleFiles;
};

bool NBAppFile::takesArgs() const {

	return __takesArgs;
};

short NBAppFile::grade() const {

	return __grade;
};

QString NBAppFile::desktopFileName() const {

	return baseName( fileUrl );
};

QString NBAppFile::filePath() const {

	return fileUrl;
};

int NBAppFile::compare( NBAppFile other ) const {

	if ( __execArgs.at( 0 ) == other.execArgs().at( 0 ) )
		return __grade - other.grade();

	return INT_MAX;
};

QString NBAppFile::category() const {

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

	/*
		*
		* Some applications that declare themselves as Education are also Science and Math are also Educational
		* So we prefer them to be in Science and Math category. So Science and Math categories are checked for
		* before Education.
		*
		* Some applications that are Office applications are also Graphics applications, like okular
		* We want them to be in Office. So we check for Office category first and then Graphics.
		*
	*/

	foreach( QString cate, __categories ) {
		if ( Accessories.contains( cate ) )
			return "Accessories";

		else if ( Development.contains( cate ) )
			return "Development";

		else if ( ScienceMath.contains( cate ) )
			return "Science and Math";

		else if ( Education.contains( cate ) )
			return "Education";

		else if ( Games.contains( cate ) )
			return "Games";

		else if ( Office.contains( cate ) )
			return "Office";

		else if ( Graphics.contains( cate ) )
			return "Graphics";

		else if ( Internet.contains( cate ) )
			return "Internet";

		else if ( Multimedia.contains( cate ) )
			return "Multimedia";

		else if ( Settings.contains( cate ) )
			return "Settings";

		else if ( System.contains( cate ) )
			return "System";

		else
			continue;
	}

	return "Uncategorized";
};

void NBAppFile::merge( NBAppFile other ) {

	__mimeTypes << other.value( NBAppFile::MimeTypes ).toStringList();
	__categories << other.value( NBAppFile::Categories ).toStringList();
	__takesArgs = __takesArgs or other.takesArgs();
	__multipleFiles = __multipleFiles or other.multipleArgs();
	__nodisplay = __nodisplay and other.value( NBAppFile::NoDisplay ).toBool();

	if ( __grade < other.grade() ) {
		__grade = other.grade();
		fileUrl = other.filePath();
	}

	if ( __comment.isEmpty() and other.value( NBAppFile::Comment ).toString().count() )
		__comment = other.value( NBAppFile::Comment ).toString();
};

bool NBAppFile::save() {

	if ( __grade < 100 ) {
		__grade = 100;
		fileUrl = QDir::home().filePath( ".local/share/applications/" ) + baseName( fileUrl );
	}

	if ( not __exec.count() )
		return false;

	QFile desktopFile( fileUrl );
	if ( not desktopFile.open( QFile::WriteOnly ) )
		return false;

	desktopFile.write( "[Desktop Entry]\n" );

	desktopFile.write( "Name=" + __name.toLocal8Bit() + "\n" );
	desktopFile.write( "Type=Application\n" );
	desktopFile.write( "Exec=" + __exec.toLocal8Bit() + "\n" );

	if ( __icon.count() )
		desktopFile.write( "Icon=" + __icon.toLocal8Bit() + "\n" );

	if ( __mimeTypes.count() )
		desktopFile.write( "MimeType=" + __mimeTypes.join( ";" ).toLocal8Bit() + "\n" );

	if ( __workPath.count() )
		desktopFile.write( "WorkPath=" + __workPath.toLocal8Bit() + "\n" );

	desktopFile.write( "Terminal=" + QByteArray( __terminalMode ? "true" : "false" ) + "\n" );

	if ( __categories.count() )
		desktopFile.write( "Categories=" + __categories.join( ";" ).toLocal8Bit() + "\n" );

	if ( __comment.count() )
		desktopFile.write( "Comment=" + __comment.toLocal8Bit() + "\n" );

	if ( __description.count() )
		desktopFile.write( "GenericName=" + __description.toLocal8Bit() + "\n" );

	desktopFile.write( "NoDisplay=" + QByteArray( __nodisplay ? "true" : "false" ) + "\n" );

	desktopFile.close();

	return true;
};

NBAppFile NBAppFile::merge( NBAppFile first, NBAppFile second ) {

	QVariantList data;

	QStringList mMimeTypes;
	mMimeTypes << first.value( NBAppFile::MimeTypes ).toStringList() << second.value( NBAppFile::MimeTypes ).toStringList();

	QStringList mCategories;
	mCategories << first.value( NBAppFile::Categories ).toStringList() << second.value( NBAppFile::Categories ).toStringList();

	bool firstDisplay = ( not first.value( NBAppFile::NoDisplay ).toBool() );
	bool secondDisplay = ( not second.value( NBAppFile::NoDisplay ).toBool() );

	QString desktopName = ( firstDisplay ? first.filePath() : ( secondDisplay ? second.filePath() : QString() ) );
	if ( not( firstDisplay or secondDisplay ) )
		desktopName = first.filePath();

	data << desktopName;
	data << first.value( NBAppFile::Name );
	data << first.value( NBAppFile::Type );
	data << first.value( NBAppFile::Exec );
	data << first.value( NBAppFile::Icon );
	data << mMimeTypes;
	data << first.value( NBAppFile::WorkPath );
	data << first.value( NBAppFile::TerminalMode );
	data << mCategories;
	data << first.value( NBAppFile::Comment );
	data << ( first.value( NBAppFile::NoDisplay ).toBool() and second.value( NBAppFile::NoDisplay ).toBool() );
	data << ( first.takesArgs() or second.takesArgs() );
	data << ( first.multipleArgs() or second.multipleArgs() );
	data << first.grade();

	// LibreOffice Fix
	if ( desktopName.toLower().contains( "libreoffice" ) ) {
		data[ 0 ] = "libreoffice-startcenter.desktop";
		data[ 1 ] = "LibreOffice";
		data[ 3 ] = first.execArgs().at( 0 ) + " %U";
		data[ 4 ] = first.execArgs().at( 0 ) + "-startcenter";
	}

	return NBAppFile( data );
};

bool NBAppFile::isValid() {

	return mIsValid;
};

bool NBAppFile::operator==( const NBAppFile& other ) const {

	bool truth = true;
	truth &= ( value( NBAppFile::Name ) == other.value( NBAppFile::Name ) );
	truth &= ( value( NBAppFile::Type ) == other.value( NBAppFile::Type ) );
	truth &= ( value( NBAppFile::Exec ) == other.value( NBAppFile::Exec ) );
	truth &= ( value( NBAppFile::Icon ) == other.value( NBAppFile::Icon ) );
	truth &= ( value( NBAppFile::MimeTypes ) == other.value( NBAppFile::MimeTypes ) );
	truth &= ( value( NBAppFile::WorkPath ) == other.value( NBAppFile::WorkPath ) );
	truth &= ( value( NBAppFile::TerminalMode ) == other.value( NBAppFile::TerminalMode ) );
	truth &= ( value( NBAppFile::Categories ) == other.value( NBAppFile::Categories ) );
	truth &= ( value( NBAppFile::Comment ) == other.value( NBAppFile::Comment ) );
	truth &= ( grade() == other.grade() );

	return truth;
};

NBAppFile::NBAppFile( QVariantList data ) {

	mIsValid = false;

	fileUrl = data[ 0 ].toString();

	__name = data[ 1 ].toString();
	__type = data[ 2 ].toString();
	__exec = data[ 3 ].toString();
	__icon = data[ 4 ].toString();
	__mimeTypes = data[ 5 ].toStringList();
	__workPath = data[ 6 ].toString();
	__terminalMode = data[ 7 ].toBool();
	__categories = data[ 8 ].toStringList();
	__comment = data[ 9 ].toString();
	__nodisplay = data[ 10 ].toBool();
	__takesArgs = data[ 11 ].toBool();
	__multipleFiles = data[ 12 ].toBool();
	__grade = data[ 13 ].toInt();

	QStringList args = __exec.split( " " );
	foreach( QString arg, args ) {
		if ( arg == "%f" or arg == "%u" ) {
			__multipleFiles = false;
			__takesArgs = true;
			__execArgs << "<#NEWBREEZE-ARG-FILE#>";
		}

		else if ( arg == "%F" or arg == "%U" ) {
			__multipleFiles = true;
			__takesArgs = true;
			__execArgs << "<#NEWBREEZE-ARG-FILES#>";
		}

		else if ( arg == "%i" ) {
			if ( !__icon.isEmpty() )
				__execArgs << "--icon" << __icon;
		}

		else if ( arg.contains( "\"%c\"" ) or arg.contains( "'%c'" ) or arg.contains( "%c" ) )
			__execArgs << __name;

		else if ( arg == "%k" )
			__execArgs << QUrl( fileUrl ).toLocalFile();

		else
			__execArgs << arg;
	}

	mIsValid = true;
};

void NBAppFile::parseDesktopFile() {

	QString rxName( "\nName=(.*)(\n|\r\n)" );
	QString rxType( "\nType=(.*)(\n|\r\n)" );
	QString rxExec( "\nExec=(.*)(\n|\r\n)" );
	QString rxIcon( "\nIcon=(.*)(\n|\r\n)" );
	QString rxPath( "\nPath=(.*)(\n|\r\n)" );
	QString rxMime( "\nMimeType=(.*)(\n|\r\n)" );
	QString rxTerm( "\nTerminal=(.*)(\n|\r\n)" );
	QString rxCate( "\nCategories=(.*)(\n|\r\n)" );
	QString rxComm( "\nComment=(.*)(\n|\r\n)" );
	QString rxDscr( "\nGenericName=(.*)(\n|\r\n)" );
	QString rxDisp( "\nNoDisplay=(.*)(\n|\r\n)" );

	// Grade - location
	// 100   - ".local/share/applications/"
	// 099   - "/usr/local/share/applications/"
	// 098   - "/usr/share/applications"
	// 097   - "/usr/share/applications/kde4"
	// 097   - "/usr/share/gnome/applications/"

	QFile dFile( fileUrl );
	if ( !dFile.exists() )
		return;

	if ( !dFile.open( QFile::ReadOnly ) )
		return;

	QString entry = QString( dFile.readAll() );
	dFile.close();

	__name = findIn( rxName, entry );
	__type = findIn( rxType, entry );
	__exec = findIn( rxExec, entry );


	if ( __name.isEmpty() ) {
		qDebug() << "Nameless monster:" << fileUrl;
		__name = __exec.split( " ", QString::SkipEmptyParts ).at( 0 );
		__name = __name.at( 0 ).toUpper() + __name.right( __name.count() - 1 );
	}

	__icon = findIn( rxIcon, entry );
	if ( __icon.isEmpty() )
		__icon = QString( "application-x-desktop" );
	__workPath = findIn( rxPath, entry );
	__mimeTypes = findIn( rxMime, entry ).split( ";", QString::SkipEmptyParts );
	QString __terminal = findIn( rxTerm, entry );
	__terminalMode = ( __terminal.toLower() == "true" ? true : false );

	__categories << findIn( rxCate, entry ).split( ";", QString::SkipEmptyParts );
	__comment = findIn( rxComm, entry );
	__description = findIn( rxDscr, entry );
	__nodisplay = ( findIn( rxDisp, entry ).toLower() == "true" ? true : false );

	// By default set @v __multipleFiles to false
	__multipleFiles = false;
	__takesArgs = false;

	QStringList args = __exec.split( " " );
	foreach( QString arg, args ) {
		if ( arg == "%f" or arg == "%u" ) {
			__multipleFiles = false;
			__takesArgs = true;
			__execArgs << "<#NEWBREEZE-ARG-FILE#>";
		}

		else if ( arg == "%F" or arg == "%U" ) {
			__multipleFiles = true;
			__takesArgs = true;
			__execArgs << "<#NEWBREEZE-ARG-FILES#>";
		}

		else if ( arg == "%i" ) {
			if ( !__icon.isEmpty() )
				__execArgs << "--icon" << __icon;
		}

		else if ( arg == "%c" )
			__execArgs << __name;

		else if ( arg == "%k" )
			__execArgs << QUrl( fileUrl ).toLocalFile();

		else
			__execArgs << arg;
	}

	// Grade calculation
	if ( dirName( fileUrl ) == QDir::home().filePath( ".local/share/applications/" ) )
		__grade = 100;

	else if ( dirName( fileUrl ) == "/usr/local/share/applications/" )
		__grade = 99;

	else if ( dirName( fileUrl ) == "/usr/share/applications/" )
		__grade = 98;

	else if ( dirName( fileUrl ) == "/usr/share/applications/kde4/" )
		__grade = 97;

	else if ( dirName( fileUrl ) == "/usr/share/gnome/applications/" )
		__grade = 97;

	else {
		qDebug() << dirName( fileUrl );
		__grade = 96;
	}

	mIsValid = true;
};

NBAppsList::NBAppsList() {

	__clearedOfDuplicates = false;
	clear();
};

void NBAppsList::clear() {

	__appsList.clear();
};

int NBAppsList::count() {

	return __appsList.count();
};

void NBAppsList::move( int from, int to ) {

	__appsList.move( from, to );
};

NBAppFile NBAppsList::at( quint64 pos ) {

	return __appsList.at( pos );
};

NBAppFile NBAppsList::value( quint64 pos ) {

	return __appsList.value( pos );
};

int NBAppsList::indexOf( NBAppFile app ) {

	return __appsList.indexOf( app );
};

void NBAppsList::insert( int at, NBAppFile app ) {

	return __appsList.insert( at, app );
};

bool NBAppsList::contains( NBAppFile app ) {

	return __appsList.contains( app );
};

void NBAppsList::remove( NBAppFile app ) {

	__appsList.removeAt( __appsList.indexOf( app ) );
};

void NBAppsList::remove( quint64 pos ) {

	__appsList.removeAt( pos );
};

QList<NBAppFile> NBAppsList::toQList() {

	return __appsList;
};

QList<NBAppFile> NBAppsList::toQList() const {

	return __appsList;
};

void NBAppsList::removeDuplicates() {

	if ( __clearedOfDuplicates )
		return;

	foreach( NBAppFile thisApp, __appsList ) {
		foreach( NBAppFile other, __appsList ) {
			if ( __appsList.indexOf( thisApp ) == __appsList.indexOf( other ) ) {
				continue;
			}

			if ( thisApp.compare( other ) == INT_MAX ) {
				continue;
			}

			else if ( thisApp.compare( other ) < 0 ) {
				__appsList.removeAt( __appsList.indexOf( thisApp ) );
				break;
			}

			else if ( thisApp.compare( other ) > 0 ) {
				/*
					*
					* Since @other is deleted, we have more to compare @thisApp with
					*
				*/
				__appsList.removeAt( __appsList.indexOf( other ) );
			}

			else if ( thisApp.compare( other ) == 0 ) {
				// Same location multiple files
				qDebug() << thisApp.value( NBAppFile::Name ).toString();
				qDebug() << thisApp.grade();
			}
		}
	}

	__clearedOfDuplicates = true;
};

NBAppsList NBAppsList::operator<<( NBAppsList newList ) {

	__appsList << newList.toQList();

	return *this;
};

NBAppsList NBAppsList::operator<<( NBAppFile app ) {

	// If app is not an Application type, do not add it.
	if ( app.value( NBAppFile::Type ).toString() != QString( "Application" ) )
		return *this;

	// If by chance there is no executable, do not add it
	if ( not app.execArgs().count() )
		return *this;

	// If this executable does not exist in $PATH, do not add it
	bool found = false;
	Q_FOREACH( QString path, QString( getenv( "PATH" ) ).split( ":" ) ) {
		if ( exists( path + "/" + app.execArgs().at( 0 ) ) ) {
			found = true;
			break;
		}
	}
	if ( not found and not exists( app.execArgs().at( 0 ) ) )
		return *this;

	// Check if this application has been added already
	foreach( NBAppFile other, __appsList ) {
		if ( app.compare( other ) == INT_MAX ) {
			// This means nothing. Just that the other app is not the same as this.
			// So we continue the search.

			continue;
		}

		else {
			// Same executable. We just merge the two apps

			other.merge( app );
			return *this;
		}
	}

	// This means no application was found similar to the one being added.
	// We add this application
	__appsList << app;

	return *this;
};

NBAppsList NBAppsList::operator=( NBAppsList newList ) {

	__appsList = newList.toQList();

	return *this;
};

bool NBAppsList::operator==( NBAppsList newList ) {

	if ( count() != newList.count() )
		return false;

	return __appsList == newList.toQList();
};

NBAppFile NBAppsList::operator[]( int pos ) {

	return __appsList.at( pos );
};

uint qHash( const NBAppFile &app ) {

	QString hashString;
	hashString += app.value( NBAppFile::Name ).toString();
	hashString += app.value( NBAppFile::Type ).toString();
	hashString += app.value( NBAppFile::Exec ).toString();
	hashString += app.value( NBAppFile::Icon ).toString();
	hashString += app.value( NBAppFile::MimeTypes ).toStringList().join( " " );
	hashString += app.value( NBAppFile::WorkPath ).toString();
	hashString += app.value( NBAppFile::TerminalMode ).toString();
	hashString += app.value( NBAppFile::Categories ).toStringList().join( " " );
	hashString += app.value( NBAppFile::Comment ).toString();
	hashString += app.value( NBAppFile::NoDisplay ).toString();

	return qChecksum( hashString.toLocal8Bit().data(), hashString.count() );
};
