/*
	*
	* NBAppFile.hpp - NBAppFile.cpp header
	*
*/

#ifndef NBAPPFILE_HPP
#define NBAPPFILE_HPP

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QUrl>
#include <QMap>

#include <libgen.h>

class NBAppFile {

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
			Comment,
			NoDisplay
		};

		NBAppFile( QString );

		// Value of various fields
		QVariant value( NBAppFile::Fields ) const;

		// Contains only the arguments, expanded wherever necessary accoring to fredesktop.org standards
		QStringList execArgs() const;

		// Can handle multiple input files?
		bool multipleArgs() const;

		// Does it take arguments
		bool takesArgs() const;

		// Grade
		short grade() const;

		// Name of the desktop file
		QString desktopFileName() const;

		// Compare this and other
		// this.grade() - other.grade()
		int compare( NBAppFile ) const;

		// Category if this NBAppFile
		QString category() const;

		// Merge to NBAppFiles
		static NBAppFile merge( NBAppFile first, NBAppFile second );

		// Check if this NBAppFile is equivalent ot @other
		bool operator==( const NBAppFile& ) const;

	private:
		NBAppFile( QVariantList );

		void parseDesktopFile();
		void generateExecArgs();

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
		QString __category;
		QString __comment;
		bool __nodisplay;

		QStringList __execArgs;
		bool __multipleFiles;
		bool __takesArgs;

		short __grade;
};

class NBAppsList {
	public:
		NBAppsList();

		void clear();
		int count();
		void move( int, int );

		NBAppFile at( quint64 );

		bool contains( NBAppFile );
		void removeDuplicates();

		void remove( NBAppFile );
		void remove( quint64 );

		QList<NBAppFile> toQList();
		QList<NBAppFile> toQList() const;

		NBAppsList operator<<( NBAppsList );
		NBAppsList operator<<( NBAppFile );
		NBAppsList operator=( NBAppsList );
		bool operator==( NBAppsList );
		NBAppFile operator[]( int );

	private:
		QList<NBAppFile> __appsList;
		bool __clearedOfDuplicates = false;
};

uint qHash( const NBAppFile &app );

#endif
