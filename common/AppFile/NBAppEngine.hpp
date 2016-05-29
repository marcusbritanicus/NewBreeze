/*
	*
	* NBAppEngine.hpp - NBAppEngine.cpp header
	*
*/

#pragma once

#include <sys/types.h>
#include <dirent.h>

#include <QTime>
#include <NBAppFile.hpp>

#if QT_VERSION >= 0x050000
	#include <QMimeDatabase>
	#include <QMimeType>
#else
	#include <NBMimeDatabase.hpp>
	#include <NBMimeType.hpp>
	#include <NBStandardPaths.hpp>
#endif

#include <QProcess>
#include <QSettings>
#include <NBXdg.hpp>

class NBAppEngine {
	public:
		// Get a list of applications for a mime type given
		NBAppsList appsForMimeType( QMimeType );

		// Get a list of mimetypes an application handles, given the desktop name
		QStringList mimeTypesForApp( QString );

		// List all the applications
		NBAppsList allDesktops();

		//Get the consolidated/unified application file for a desktop name
		NBAppFile application( QString );

		// List the application locations
		QStringList appsLocations();

		// Add one new application location
		void addAppsLocations( QString );

		// Add multiple applications locations
		void addAppsLocations( QStringList );

		// Parse all desktops
		void parseDesktops();

		static NBAppEngine* instance();
		static void setApplicationAsDefault( QString, QString );

		NBAppFile xdgDefaultApp( QMimeType );

	private:
		// Constructor
		NBAppEngine();

		static NBAppEngine *globalInstance;

		// List files in location @arg 1
		QStringList listApplications( QString );

		QStringList appsDirs;

		NBAppsList appsList;
};
