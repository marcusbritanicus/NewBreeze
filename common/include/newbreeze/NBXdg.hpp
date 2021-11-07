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

#include <newbreeze/common.hpp>
#include <newbreeze/NBDeviceInfo.hpp>

class QIcon;

namespace NewBreeze {
	class DesktopFile;
	class XdgMime;
}

class NBXdg {
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

class NewBreeze::DesktopFile {

	public:
		enum Type {
			Application = 0x906AF2,							// A regular executable app
			Link,											// Linux equivalent of '.lnk'			! NOT HANDLED
			Directory										// Desktopthat points to a directory	! NOT HANDLED
		};

		DesktopFile( QString filename = QString() );	// Create an instance of a desktop file

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

		/* Check if this DesktopFile is equivalent to @other */
		bool operator==( const DesktopFile& ) const;

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

typedef QList<NewBreeze::DesktopFile> AppsList;

class NewBreeze::XdgMime {

	public:
		static XdgMime* instance();

		// Get a list of applications for a mime type given
		AppsList appsForMimeType( QMimeType );

		// Get a list of mimetypes an application handles, given the desktop name
		QStringList mimeTypesForApp( QString );

		// List all the applications
		AppsList allDesktops();

		// Get the consolidated/unified application file for a desktop name
		DesktopFile application( QString );

		// Get the best desktop file path for a desktop name
		QString desktopPathForName( QString );

		// Get the best desktop file path for a desktop name
		DesktopFile desktopForName( QString );

		// Add one new application location
		void addAppsLocations( QString );

		// Add multiple applications locations
		void addAppsLocations( QStringList );

		// Parse all desktops
		void parseDesktops();

		static void setApplicationAsDefault( QString, QString );

		DesktopFile xdgDefaultApp( QMimeType );

	private:
		XdgMime();

		static NewBreeze::XdgMime *globalInstance;

		QStringList appsDirs;
		AppsList appsList;
};

uint qHash( const NewBreeze::DesktopFile &app );
QVariant& toQVariant( const NewBreeze::DesktopFile &app );
Q_DECLARE_METATYPE( NewBreeze::DesktopFile );
