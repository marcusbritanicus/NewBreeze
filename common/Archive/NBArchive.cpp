/*
	*
	* NBArchive.cpp - Archive class for NewBreeze
	*
*/

#include "NBArchive.hpp"

static QMimeDatabase mimeDb;

NBArchive::NBArchive( QString archive ) {

	readDone = false;
	archiveName = QDir( archive ).absolutePath();
};

void NBArchive::updateInputFiles( QStringList inFiles ) {

	Q_FOREACH( QString file, inFiles ) {
		if ( isDir( file ) )
			inputList.append( recDirWalk( file ) );

		else
			inputList.append( file );
	}

	inputList.sort();
	inputList.removeDuplicates();
};

void NBArchive::setWorkingDir( QString wDir ) {

	src = QString( wDir );
};

void NBArchive::setDestination( QString path ) {

	/*
		*
		* @p path will be a absolute.
		* So QDir we construct will be home path
		*
	*/

	dest = QString( path );
	if ( not QFileInfo( QDir( dest ).absolutePath() ).exists() )
		mkpath( path, 0755 );
};

void NBArchive::create() {

	QMimeType mime = mimeDb.mimeTypeForFile( archiveName );

	if ( mime == mimeDb.mimeTypeForFile( "file.gz" ) ) {
		Q_FOREACH( QString input, inputList ) {
			NBGZip *archive = new NBGZip( archiveName, NBGZip::WRITE, input );
			archive->create();
		}
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.bz2" ) ) {
		Q_FOREACH( QString input, inputList ) {
			NBBZip2 *archive = new NBBZip2( archiveName, NBBZip2::WRITE, input );
			archive->create();
		}
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.lzma" ) ) {
		Q_FOREACH( QString input, inputList ) {
			NBLzma *archive = new NBLzma( archiveName, NBLzma::WRITE, input  );
			archive->create();
		}
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.xz" ) ) {
		Q_FOREACH( QString input, inputList ) {
			NBXz *archive = new NBXz( archiveName, NBXz::WRITE, input  );
			archive->create();
		}
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.uu" ) ) {
		qDebug() << "No compressor available as of now";
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.lz" ) ) {
		qDebug() << "No compressor available as of now";
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.lzo" ) ) {
		qDebug() << "No compressor available as of now";
	}

	else {
		struct archive *a;
		struct archive_entry *entry;
		struct stat st;
		char buff[ 8192 ];
		int len;
		int fd;
		int r;

		a = archive_write_new();

		// Depend on the format provided by the user
		r = setFilterFormat( a, mime );
		if ( r < ARCHIVE_OK )
			qDebug() << "Cannot use the input filter/format.";

                r = archive_write_open_filename( a, archiveName.toUtf8().data() );
		if ( r < ARCHIVE_OK )
			qDebug() << "Unable to write file for writing.";

		Q_FOREACH( QString file, inputList ) {
			char *filename;
			filename = new char[ file.count() + 1 ];
                        strcpy( filename, file.toUtf8().data() );

			stat( filename, &st );
			entry = archive_entry_new();
			archive_entry_set_pathname( entry, filename );
			archive_entry_set_size( entry, st.st_size );
			archive_entry_set_filetype( entry, st.st_mode );
			archive_entry_set_perm( entry, st.st_mode );

			archive_write_header( a, entry );

			// Perform the write
			fd = open( filename, O_RDONLY );
			len = read( fd, buff, sizeof( buff ) );
			while ( len > 0 ) {
				archive_write_data( a, buff, len );
				len = read( fd, buff, sizeof( buff ) );
			}
			close( fd );
			archive_entry_free( entry );
		}

		archive_write_close( a );
		archive_write_free( a );
	}
};

int NBArchive::extract() {

	QMimeType mime = mimeDb.mimeTypeForFile( archiveName );

	if ( mime == mimeDb.mimeTypeForFile( "file.gz" ) ) {
		Q_FOREACH( QString input, inputList ) {
			NBGZip *archive = new NBGZip( archiveName, NBGZip::READ, dest );
			archive->extract();
		}
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.bz2" ) ) {
		Q_FOREACH( QString dest, inputList ) {
			NBBZip2 *archive = new NBBZip2( archiveName, NBBZip2::READ, dest );
			archive->extract();
		}
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.lzma" ) ) {
		Q_FOREACH( QString dest, inputList ) {
			NBLzma *archive = new NBLzma( archiveName, NBLzma::READ, dest  );
			archive->extract();
		}
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.xz" ) ) {
		Q_FOREACH( QString dest, inputList ) {
			NBXz *archive = new NBXz( archiveName, NBXz::READ, dest  );
			archive->extract();
		}
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.uu" ) ) {
		qDebug() << "No compressor available as of now";
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.lz" ) ) {
		qDebug() << "No compressor available as of now";
	}

	else if ( mime == mimeDb.mimeTypeForFile( "file.lzo" ) ) {
		qDebug() << "No compressor available as of now";
	}

	else {
		// Change to the target directory
		char srcDir[ 10240 ] = { 0 };
		getcwd( srcDir, 10240 );
		chdir( dest.toUtf8().data() );

		struct archive *a;
		struct archive *ext;
		struct archive_entry *entry;
		int flags;
		int r;

		/* Select which attributes we want to restore. */
		flags = ARCHIVE_EXTRACT_TIME;
		flags |= ARCHIVE_EXTRACT_PERM;
		flags |= ARCHIVE_EXTRACT_ACL;
		flags |= ARCHIVE_EXTRACT_FFLAGS;
		flags |= ARCHIVE_EXTRACT_OWNER;

		// Source Archive
		a = archive_read_new();
		archive_read_support_format_all( a );
		archive_read_support_filter_all( a );

		// Structure to write files to disk
		ext = archive_write_disk_new();
		archive_write_disk_set_options( ext, flags );
		archive_write_disk_set_standard_lookup( ext );

		if ( ( r = archive_read_open_filename( a, archiveName.toUtf8().data(), 10240 ) ) )
			return 1;

		while ( true ) {
			r = archive_read_next_header( a, &entry );
			if ( r == ARCHIVE_EOF )
				break;

			if ( r < ARCHIVE_OK )
				fprintf( stderr, "%s\n", archive_error_string( a ) );

			if ( r < ARCHIVE_WARN )
				return 1;

			r = archive_write_header( ext, entry );
			if ( r < ARCHIVE_OK )
				fprintf( stderr, "%s\n", archive_error_string( ext ) );

			else if ( archive_entry_size( entry ) > 0 ) {
				r = copyData( a, ext );
				if ( r < ARCHIVE_OK )
					fprintf( stderr, "%s\n", archive_error_string( ext ) );

				if ( r < ARCHIVE_WARN )
					return 1;
			}

			r = archive_write_finish_entry( ext );
			if ( r < ARCHIVE_OK )
				fprintf( stderr, "%s\n", archive_error_string( ext ) );

			if ( r < ARCHIVE_WARN )
				return 1;
		}

		archive_read_close( a );
		archive_read_free( a );

		archive_write_close( ext );
		archive_write_free( ext );

		chdir( srcDir );

		return 0;
	}

	return 0;
};

int NBArchive::extractMember( QString memberName ) {

	list();

	// Change to the target directory
	char srcDir[ 10240 ] = { 0 };
	getcwd( srcDir, 10240 );
	chdir( dest.toUtf8().data() );

	struct archive *a;
	struct archive *ext;
	struct archive_entry *entry;
	int flags;
	int r;

	/* Select which attributes we want to restore. */
	flags = ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_PERM;
	flags |= ARCHIVE_EXTRACT_ACL;
	flags |= ARCHIVE_EXTRACT_FFLAGS;
	//flags |= ARCHIVE_EXTRACT_OWNER;

	// Source Archive
	a = archive_read_new();
	archive_read_support_format_all( a );
	archive_read_support_filter_all( a );

	// Structure to write files to disk
	ext = archive_write_disk_new();
	archive_write_disk_set_options( ext, flags );
	archive_write_disk_set_standard_lookup( ext );

	r = archive_read_open_filename( a, archiveName.toUtf8().data(), 10240 );
	if ( r != ARCHIVE_OK ) {
		qDebug() << "[ERROR]: Failed to open archive:" << archiveName;
		return 1;
	}

	bool dir = false, found = false;

	/* Direct member */
	Q_FOREACH( ArchiveEntry *ae, memberList ) {
		if ( ae->name == memberName ) {
			dir = ( ae->type == AE_IFDIR );
			found = true;
			break;
		}

		if ( ae->name == memberName + "/" ) {
			memberName += "/";
			dir = ( ae->type == AE_IFDIR );
			found = true;
			break;
		}
	}

	if ( not found ) {
		/* Always check for @memberName + "/" because, all indirect members will be directories */
		memberName += "/";

		/* Indirect member: ex. debug/ is a member if debug/path/to/file.ext exists */
		Q_FOREACH( ArchiveEntry *ae, memberList ) {
			if ( ae->name.startsWith( memberName ) == 0 ) {
				dir = true;
				found = true;
				break;
			}
		}
	}

	if ( found ) {
		while ( true ) {
			r = archive_read_next_header( a, &entry );
			if ( r == ARCHIVE_EOF )
				break;

			if ( r < ARCHIVE_OK )
				fprintf( stderr, "%s\n", archive_error_string( a ) );

			if ( r < ARCHIVE_WARN )
				return 1;

			QString entryPath = archive_entry_pathname( entry );

			/* Check if the current entry starts with @memberName */
			if ( entryPath.startsWith( memberName ) ) {
				if ( not dir ) {
					if ( entryPath != memberName )
						continue;
				}

				r = archive_write_header( ext, entry );
				if ( r < ARCHIVE_OK )
					fprintf( stderr, "%s\n", archive_error_string( ext ) );

				else if ( archive_entry_size( entry ) > 0 ) {
					r = copyData( a, ext );
					if ( r < ARCHIVE_OK )
						fprintf( stderr, "%s\n", archive_error_string( ext ) );

					if ( r < ARCHIVE_WARN )
						return 1;
				}

				r = archive_write_finish_entry( ext );
				if ( r < ARCHIVE_OK )
					fprintf( stderr, "%s\n", archive_error_string( ext ) );

				if ( r < ARCHIVE_WARN )
					return 1;
			}
		}
	}

	else {

		qDebug() << "[Error]" << "File not found in the archive:" << memberName;
	}

	archive_read_close( a );
	archive_read_free( a );

	archive_write_close( ext );
	archive_write_free( ext );

	chdir( srcDir );

	return 0;
};

ArchiveEntries NBArchive::list() {

	if ( readDone )
		return memberList;

	memberList.clear();

	struct archive *a;
	struct archive_entry *entry;
	int r;

	// Source Archive
	a = archive_read_new();
	archive_read_support_format_all( a );
	archive_read_support_filter_all( a );

	if ( ( r = archive_read_open_filename( a, archiveName.toUtf8().data(), 10240 ) ) ) {
		qDebug() << "[Error]" << archive_error_string( a );
		readDone = true;
		return ArchiveEntries();
	}

	while ( true ) {
		r = archive_read_next_header( a, &entry );

		if ( r == ARCHIVE_EOF )
			break;

		if ( r < ARCHIVE_OK )
			qDebug() << archive_error_string( a );

		ArchiveEntry *ae = new ArchiveEntry;
		ae->name = archive_entry_pathname( entry );
		ae->size = archive_entry_size( entry );
		ae->type = archive_entry_filetype( entry );
		memcpy( &ae->info, archive_entry_stat( entry ), sizeof( struct stat ) );

		memberList << ae;
	}

	archive_read_close( a );
	archive_read_free( a );

	readDone = true;

	return memberList;
};

int NBArchive::copyData( struct archive *ar, struct archive *aw ) {

	int r;
	const void *buff;
	size_t size;

	#ifdef __LP64__
		off_t offset;
	#else
		la_int64_t offset;
	#endif

	while ( true ) {
		r = archive_read_data_block( ar, &buff, &size, &offset );
		if ( r == ARCHIVE_EOF )
			return ( ARCHIVE_OK );

		if ( r < ARCHIVE_OK )
			return ( r );

		r = archive_write_data_block( aw, buff, size, offset );
		if ( r < ARCHIVE_OK ) {
			fprintf( stderr, "%s\n", archive_error_string( aw ) );
			return ( r );
		}
	}
};

int NBArchive::setFilterFormat( struct archive *ar, QMimeType mType ) {

	int r = ARCHIVE_OK;

	if ( mType == mimeDb.mimeTypeForFile( "file.cpio" ) ) {
		r |= archive_write_add_filter_none( ar );
		r |= archive_write_set_format( ar, ARCHIVE_FORMAT_CPIO );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.shar" ) ) {
		r |= archive_write_add_filter_none( ar );
		r |= archive_write_set_format( ar, ARCHIVE_FORMAT_SHAR );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.tar" ) ) {
		r |= archive_write_add_filter_none( ar );
		r |= archive_write_set_format( ar, ARCHIVE_FORMAT_TAR );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.tar.gz" ) ) {
		r |= archive_write_add_filter_gzip( ar );
		r |= archive_write_set_format( ar, ARCHIVE_FORMAT_TAR );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.tar.xz" ) ) {
		r |= archive_write_add_filter_xz( ar );
		r |= archive_write_set_format( ar, ARCHIVE_FORMAT_TAR );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.tar.bz2" ) ) {
		r |= archive_write_add_filter_bzip2( ar );
		r |= archive_write_set_format( ar, ARCHIVE_FORMAT_TAR );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.tar.Z" ) ) {
		r |= archive_write_add_filter_compress( ar );
		r |= archive_write_set_format( ar, ARCHIVE_FORMAT_TAR );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.iso" ) ) {
		r |= archive_write_add_filter_none( ar );
		r |= archive_write_set_format_iso9660( ar );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.zip" ) ) {
		r |= archive_write_add_filter_none( ar );
		r |= archive_write_set_format_zip( ar );
		r |= archive_write_zip_set_compression_deflate( ar );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.ar" ) ) {
		r |= archive_write_add_filter_none( ar );
		r |= archive_write_set_format( ar, ARCHIVE_FORMAT_AR );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.xar" ) ) {
		r |= archive_write_add_filter_none( ar );
		r |= archive_write_set_format_xar( ar );
	}

	else if ( mType == mimeDb.mimeTypeForFile( "file.7z" ) ) {
		r |= archive_write_add_filter_none( ar );
		r |= archive_write_set_format_7zip( ar );
	}

	return r;
};
