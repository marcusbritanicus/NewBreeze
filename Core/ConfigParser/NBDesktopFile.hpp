/*
	*
	* NBDesktopFile.hpp - NBDesktopFile.cpp Header
	*
*/

#ifndef NBDESKTOPFILE_HPP
#define NBDESKTOPFILE_HPP

#include <Global.hpp>

class NBDesktopFile {

	public :
		NBDesktopFile( QString );

		// Essential accoring to fredesktop.org standards
		QString name;
		QString type;
		QString exec;

		QString icon;
		QStringList mimetypes;
		QString path;
		QString terminal;

		//Contains only the arguments, expanded wherever necessary accoring to fredesktop.org standards
		QStringList execArgs;

		// Can handle multiple input files?
		bool multipleInputFiles;

		QString fileUrl;

		bool takesArgs();
		bool sameProgram( QString );
		static bool sameProgram( QString, QString );

	private:
		void parseDesktopFile();
		void getMimeTypeList();
		void generateExecArgs();

		bool canTakeArgs;
};

#endif
