/*
	*
	* NBTar-mHeader->hpp - Header implementation for TAR
	*
*/

#include <NBTarHeader.hpp>

TarHeader *TarHeader::mHeader = NULL;

TarHeader* TarHeader::createHeader( QString fileName ) {

	mHeader = new TarHeader();

	/* Assume this will be a valid tar header */
	mHeader->mIsValid = true;

	/* File attributes */
	struct stat fileAtts;

	/* If we are unable to read the attributes, then we set validity to false */
	if ( stat( fileName.toLocal8Bit().data(), &fileAtts ) != 0 ) {
		mHeader->mIsValid = false;
		return mHeader;
	}

	/* Sanitize the name: already "QDir::cleanPath" has been performed */
	QString arcFn = fileName.replace( "../", "" ).replace( "./", "" );
	if ( arcFn.startsWith( "/") )
		arcFn.remove( 0, 1 );

	/* First comes the name */
	mHeader->name = arcFn.toLocal8Bit().leftJustified( 99, 0 ) + '\0';

	/* File mode */
	mHeader->mode = QByteArray::number( fileAtts.st_mode & ~S_IFMT, 8 ).rightJustified( 7, '0' ) + '\0';

	/* User ID */
	mHeader->uid = QByteArray::number( fileAtts.st_uid, 8 ).rightJustified( 7, '0' ) + '\0';

	/* Group ID */
	mHeader->gid = QByteArray::number( fileAtts.st_gid, 8 ).rightJustified( 7, '0' ) + '\0';

	/* Size */
	if ( S_ISREG( fileAtts.st_mode ) )
		mHeader->size = QByteArray::number( qlonglong( fileAtts.st_size ), 8 ).rightJustified( 11, '0' ) + '\0';

	else
		mHeader->size = QByteArray( 11, '0' ) + '\0';

	/* MTime */
	mHeader->mtime = QByteArray::number( qlonglong( fileAtts.st_mtime ), 8 ).rightJustified( 11, '0' ) + '\0';

	/* CheckSum */
	// The check sum is computed at the end

	/* TypeFlag */
	switch( fileAtts.st_mode & S_IFMT ) {
		case S_IFSOCK :{
			/* We ignore sockets */
			mHeader->mIsValid = false;
			mHeader->type = QByteArray();
		}

		case S_IFLNK :{
			mHeader->type = "2";
			break;
		}

		case S_IFREG :{
			mHeader->type = "0";
			break;
		}

		case S_IFBLK :{
			mHeader->type = "4";
			break;
		}

		case S_IFDIR :{
			mHeader->type = "5";
			break;
		}

		case S_IFCHR :{
			mHeader->type = "3";
			break;
		}

		case S_IFIFO :{
			mHeader->type = "6";
			break;
		}

		default:
			mHeader->type = " ";
			break;
	}

	/* Link Name */
	if ( S_ISLNK( fileAtts.st_mode ) ) {
		char *buff = new char[ 100 ];
		readlink( fileName.toLocal8Bit().data(), buff, 100 );
		mHeader->target = QByteArray( buff ).leftJustified( 100, 0 );
		delete buff;
	}

	else
		mHeader->target = QByteArray( 100, 0 );

	/* Magic */
	mHeader->magic = QByteArray( "ustar" ) + '\0';

	/* Version */
	mHeader->version = QByteArray( "00" );

	/* User Name */
	mHeader->uname = QByteArray( getpwuid( fileAtts.st_uid )->pw_name ).leftJustified( 31, 0 ) + '\0';

	/* Group Name */
	mHeader->gname = QByteArray( getgrgid( fileAtts.st_gid )->gr_name ).leftJustified( 31, 0 ) + '\0';

	/* Dev Major */
	mHeader->major = QByteArray::number( major( fileAtts.st_dev ), 8 ).leftJustified( 7, '0' ) + '\0';

	/* Dev Minor */
	mHeader->minor = QByteArray::number( major( fileAtts.st_dev ), 8 ).leftJustified( 7, '0' ) + '\0';

	/* Prefix */
	mHeader->prefix = QByteArray( 155, 0 );

	/* Padding */
	mHeader->padding = QByteArray( 12, 0 );

	/* CheckSum */
	mHeader->computeChecksum();

	return mHeader;
};

TarHeader* TarHeader::parseHeader( QByteArray rawHeader ) {

	mHeader = new TarHeader();

	/* If the rawHeader size is not 512 bytes then it cannot be a TtarHeader */
	if ( rawHeader.count() != 512 ) {
		mHeader->mIsValid = false;
		return mHeader;
	}

	/* If rawHeader starts with a null byte, then it cannot be a valid header */
	if ( rawHeader.at( 0 ) == '\0' ) {
		mHeader->mIsValid = false;
		return mHeader;
	}

	/* If we have a 512 bytes block filled with null bytes, then its the end of tarfile */
	if ( rawHeader.count( '\0' ) == 512 ) {
		mHeader->mIsValid = false;
		return mHeader;
	}

	mHeader->name =       rawHeader.mid( 0  , 100 );
	mHeader->mode =       rawHeader.mid( 100, 8   );
	mHeader->uid =        rawHeader.mid( 108, 8   );
	mHeader->gid =        rawHeader.mid( 116, 8   );
	mHeader->size =       rawHeader.mid( 124, 12  );
	mHeader->mtime =      rawHeader.mid( 136, 12  );
	mHeader->chksum =     rawHeader.mid( 148, 8   );
	mHeader->type =       rawHeader.mid( 156, 1   );
	mHeader->target =     rawHeader.mid( 157, 100 );
	mHeader->magic =      rawHeader.mid( 257, 6   );
	mHeader->version =    rawHeader.mid( 263, 2   );
	mHeader->uname =      rawHeader.mid( 265, 32  );
	mHeader->gname =      rawHeader.mid( 297, 32  );
	mHeader->major =      rawHeader.mid( 329, 8   );
	mHeader->minor =      rawHeader.mid( 337, 8   );
	mHeader->prefix =     rawHeader.mid( 345, 155 );
	mHeader->padding =    rawHeader.mid( 500, 12  );

	mHeader->validateHeader();

	return mHeader;
};

QByteArray TarHeader::headerString() {

	header += name;
	header += mode;
	header += uid;
	header += gid;
	header += size;
	header += mtime;
	header += chksum;
	header += type;
	header += target;
	header += magic;
	header += version;
	header += uname;
	header += gname;
	header += major;
	header += minor;
	header += prefix;
	header += padding;

	return header;
};

QVariant TarHeader::value( TarHeader::HeaderField field ) {

	if ( not mIsValid )
		return QVariant();

	switch( field ) {
		case Name: {

			return QString::fromLocal8Bit( name.replace( '\0', "" ) );
		}

		case Mode: {

			return strtoll( mode.data(), NULL, 8 );
		}

		case Uid: {

			return strtoll( uid.data(), NULL, 8 );
		}

		case Gid: {

			return strtoll( gid.data(), NULL, 8 );
		}

		case Size: {

			return strtoll( size.data(), NULL, 8 );
		}

		case Time: {

			return strtoll( mtime.data(), NULL, 8 );
		}

		case CheckSum: {

			return strtoll( chksum.data(), NULL, 8 );
		}

		case Type: {

			return type.at( 0 );
		}

		case Target: {

			return QString::fromLocal8Bit( target.replace( '\0', "" ) );
		}

		case Magic: {

			return "ustar";
		}

		case Version: {

			return "00";
		}

		case User: {

			return QString::fromLocal8Bit( uname.replace( '\0', "" ) );
		}

		case Group: {

			return QString::fromLocal8Bit( gname.replace( '\0', "" ) );
		}

		case Major: {

			return strtoll( major.data(), NULL, 8 );
		}

		case Minor: {

			return strtoll( minor.data(), NULL, 8 );
		}

		case Prefix: {

			return QString::fromLocal8Bit( gname.replace( '\0', "" ) );
		}

	    case Padding: {

			return 0;
		}

		default:
			return QVariant();
	}
};

bool TarHeader::isValid() {

	return mIsValid;
};

void TarHeader::computeChecksum() {

	int hash = 0;
	for( int i = 0; i < name.count(); i++ )
		hash += name.at( i );

	for( int i = 0; i < mode.count(); i++ )
		hash += mode.at( i );

	for( int i = 0; i < uid.count(); i++ )
		hash += uid.at( i );

	for( int i = 0; i < gid.count(); i++ )
		hash += gid.at( i );

	for( int i = 0; i < size.count(); i++ )
		hash += size.at( i );

	for( int i = 0; i < mtime.count(); i++ )
		hash += mtime.at( i );

	for( int i = 0; i < type.count(); i++ )
		hash += type.at( i );

	for( int i = 0; i < target.count(); i++ )
		hash += target.at( i );

	for( int i = 0; i < magic.count(); i++ )
		hash += magic.at( i );

	for( int i = 0; i < version.count(); i++ )
		hash += version.at( i );

	for( int i = 0; i < uname.count(); i++ )
		hash += uname.at( i );

	for( int i = 0; i < gname.count(); i++ )
		hash += gname.at( i );

	for( int i = 0; i < major.count(); i++ )
		hash += major.at( i );

	for( int i = 0; i < minor.count(); i++ )
		hash += minor.at( i );

	for( int i = 0; i < prefix.count(); i++ )
		hash += prefix.at( i );

	for( int i = 0; i < padding.count(); i++ )
		hash += padding.at( i );

	/* CheckSum bits */
	hash += 32 * 8;

	chksum = QByteArray::number( hash, 8 ).rightJustified( 7, '0' ) + '\0';
};

void TarHeader::validateHeader() {

	int hash = 0;
	for( int i = 0; i < name.count(); i++ )
		hash += name.at( i );

	if ( hash == 0 ) {
		mIsValid = false;
		return;
	}

	for( int i = 0; i < mode.count(); i++ )
		hash += mode.at( i );

	for( int i = 0; i < uid.count(); i++ )
		hash += uid.at( i );

	for( int i = 0; i < gid.count(); i++ )
		hash += gid.at( i );

	for( int i = 0; i < size.count(); i++ )
		hash += size.at( i );

	for( int i = 0; i < mtime.count(); i++ )
		hash += mtime.at( i );

	for( int i = 0; i < type.count(); i++ )
		hash += type.at( i );

	for( int i = 0; i < target.count(); i++ )
		hash += target.at( i );

	for( int i = 0; i < magic.count(); i++ )
		hash += magic.at( i );

	for( int i = 0; i < version.count(); i++ )
		hash += version.at( i );

	for( int i = 0; i < uname.count(); i++ )
		hash += uname.at( i );

	for( int i = 0; i < gname.count(); i++ )
		hash += gname.at( i );

	for( int i = 0; i < major.count(); i++ )
		hash += major.at( i );

	for( int i = 0; i < minor.count(); i++ )
		hash += minor.at( i );

	for( int i = 0; i < prefix.count(); i++ )
		hash += prefix.at( i );

	for( int i = 0; i < padding.count(); i++ )
		hash += padding.at( i );

	/* CheckSum bits */
	hash += 32 * 8;

	if ( strtol( chksum, NULL, 8 ) != hash )
		mIsValid = false;

	mIsValid = true;
};
