/*
	*
	* NBDesktopFile.hpp - NBDesktopFile.cpp Header
	*
*/

#ifndef NBDESKTOPFILE_HPP
#define NBDESKTOPFILE_HPP

#include <Global.hpp>
#include <NBTools.hpp>

class NBDesktopFile {

	public:
		enum Fields {
			Name,
			Type,
			Exec,
			Icon,
			MimeTypes,
			WorkPath,
			TerminalMode,
			Categories,
			Comment
		};

		NBDesktopFile( QString );

		// Essential accoring to fredesktop.org standards
		QString name();
		QString type();
		QString exec();

		QString icon();
		QStringList mimeTypes();
		QString workPath();
		bool terminalMode();

		QStringList categories();
		QString comment();

		// Contains only the arguments, expanded wherever necessary accoring to fredesktop.org standards
		QStringList execArgs();

		// Can handle multiple input files?
		bool multipleArgs();

		// Does it take arguments
		bool takesArgs();

		// Grade
		short grade();

		QString desktopFileName();

		bool isSameAs( NBDesktopFile );
		static bool areSame( NBDesktopFile, NBDesktopFile );

		// Merge this with other
		NBDesktopFile mergeWith( NBDesktopFile );

		// Merge other with this
		NBDesktopFile merge( NBDesktopFile );

		// Merge one and two
		static bool merge( NBDesktopFile, NBDesktopFile );

	private:
		void parseDesktopFile();
		void generateExecArgs();

		bool canTakeArgs;
		QVariantList fields;

		QString fileUrl;

		// variables
		QString __name;
		QString __type;
		QString __exec;

		QString __icon;
		QStringList __mimeTypes;
		QString __workPath;
		bool __terminalMode;

		QStringList __categories;
		QString __comment;

		QStringList __execArgs;
		bool __multipleFiles;
		bool __takesArgs;

		short __grade;
};

#endif
