/*
	*
	* libtar.hpp - libtar Header
	*
*/

#ifndef NBLIBTAR_HPP
#define NBLIBTAR_HPP

#include <Global.hpp>
#include <vector>
#include <libtar.h>

using namespace std;

struct TarHeader {
	char name[ 100 ];			// Offset 0
	char mode[ 8 ];				// Offset 100
	char uid[ 8 ];				// Offset 108
	char gid[ 8 ];				// Offset 116
	char size[ 12 ];			// Offset 124
	char mtime[ 12 ];			// Offset 136
	char chksum[ 8 ];			// Offset 148
	char typeflag[ 1 ];			// Offset 156
	char linkname[ 100 ];		// Offset 157
	char magic[ 6 ];			// Offset 257
	char version[ 2 ];			// Offset 263
	char uname[ 32 ];			// Offset 265
	char gname[ 32 ];			// Offset 297
	char devmajor[ 8 ];			// Offset 329
	char devminor[ 8 ];			// Offset 337
	char prefix[ 155 ];			// Offset 345
	char padding[ 12 ];			// Offset 500
};

class TarFile {

	public:
		TarFile( QString name, char mode );
		void close();

		// I/O
		void extractAll( QString );
		void addFile( QString, QString );
		void addDir( QString, QString );

		QString filename;
		QChar openmode;
		TAR *tar;
		vector<struct TarHeader> tarList;

	private :
		void openTar( int oflag );
		int readNextHeader();

		struct TarHeader tarHeader;
};

class NBTar {

	public:
		enum TarError {
			NREG,			// Not a reguar file
			NFND,			// File/Folder not found
		};

		enum Mode {
			READ,			// Read a zip file
			WRITE,			// Write a zip file
		};

		NBTar( QString, NBTar::Mode mode );
		void create( QStringList, QString );
		void extract( QString );

		static QString name;
		static NBTar::Mode mode;

	private:
		void extractMember( QString, QString );
		QStringList fileList;

		TarFile *tarfile;
		int NBTarError;
};

#endif
