/*
	*
	* NBXdg.hpp - NBXdg class header
	*
	* The class NBDesktopFile was written by marcusbritanicus as a part of CoreApps
	* Almost the same code is being used here with suitable modifications to name
	* and other parts.
	*
*/

#pragma once

#include "common.hpp"
#include "NBDeviceInfo.hpp"

class QIcon;

class NBCOMMON_DLLSPEC NBXdg {
	public:
		enum XdgUserDir {
			XDG_DATA_HOME    = 0x02544878,
			XDG_CONFIG_HOME,
			XDG_CACHE_HOME
		};

		enum XdgSystemDirs {
			XDG_DATA_DIRS    = 0x196BB115,
			XDG_CONFIG_DIRS
		};

		static QString home();
		static QString xdgDefaultApp( QString );
		static QString userDir( NBXdg::XdgUserDir );
		static QStringList systemDirs( NBXdg::XdgSystemDirs );

		static QString trashLocation( QString path );
		static QString homeTrashLocation();

		static QString homePartition;
};

class NBCOMMON_DLLSPEC NBDesktopFile {

	public:
		enum Type {
			Application = 0x906AF2,							// A regular executable app
			Link,											// Linux equivalent of '.lnk'			! NOT HANDLED
			Directory										// Desktopthat points to a directory	! NOT HANDLED
		};

		NBDesktopFile( QString filename = QString() );	// Create an instance of a desktop file

		bool startApplication();
		bool startApplicationWithArgs( QStringList );

		QString desktopName()const;							// Filename of the desktop
		QString name() const;								// Name
		QString genericName() const;						// Generic Name
		QString description() const;						// Comment
		QString exec() const;								// 'TryExec' value or the path divined from 'Exec'
		QString command() const;							// Full command as given in 'Exec'
		QString icon() const;								// Application Icon Name or Path
		QString category() const;							// Main category according to XDG

		QStringList mimeTypes() const;						// MimeTypes handled by this app
		QStringList categories() const;						// Categories this app belongs to
		QStringList parsedExec() const;						// Arguments parsed with %U, %F, %u, %f etc removed

		int type() const;									// Application/Link/Directory
		int rank() const;

		bool visible() const;								// Visible in 'Start' Menu
		bool runInTerminal() const;							// If this app should be run in the terminal
		bool takesArgs() const;								// Does the app take any arguments?
		bool multipleArgs() const;							// Does the app take multiple arguments?
		bool isValid() const;								// Is a valid desktop file

		QString desktopFileUrl() const;						// URL of the desktop file

		/* Check if this NBDesktopFile is equivalent to @other */
		bool operator==( const NBDesktopFile& ) const;

	private:
		void getCategory();

		/*
			*
			* KDE has a very annoying habit of splitting its a single
			* desktop file (ex. okular) into multiple files based on mimetypes
			* okularApplication_chm.desktop, okularApplication_pdf.desktop,
			* and so on...
			*
		*/
		void kdeFix( QString );

		QString mFileUrl, mDesktopName, mExec, mCommand;
		QString mName, mGenericName, mDescription, mIcon;
		QString mCategory;
		QStringList mMimeTypes, mCategories, mParsedArgs;

		bool mVisible, mRunInTerminal, mValid = false;
		bool mMultiArgs = false, mTakesArgs = false;

		int mType;

		short int mRank = 3;
};

typedef QList<NBDesktopFile> AppsList;

class NBCOMMON_DLLSPEC NBXdgMime {

	public:
		static NBXdgMime* instance();

		// Get a list of applications for a mime type given
		AppsList appsForMimeType( QMimeType );

		// Get a list of mimetypes an application handles, given the desktop name
		QStringList mimeTypesForApp( QString );

		// List all the applications
		AppsList allDesktops();

		// Get the consolidated/unified application file for a desktop name
		NBDesktopFile application( QString );

		// Get the best desktop file path for a desktop name
		QString desktopPathForName( QString );

		// Get the best desktop file path for a desktop name
		NBDesktopFile desktopForName( QString );

		// Add one new application location
		void addAppsLocations( QString );

		// Add multiple applications locations
		void addAppsLocations( QStringList );

		// Parse all desktops
		void parseDesktops();

		static void setApplicationAsDefault( QString, QString );

		NBDesktopFile xdgDefaultApp( QMimeType );

	private:
		NBXdgMime();

		static NBXdgMime *globalInstance;

		QStringList appsDirs;
		AppsList appsList;
};

uint qHash( const NBDesktopFile &app );
QVariant& toQVariant( const NBDesktopFile &app );
Q_DECLARE_METATYPE( NBDesktopFile );
