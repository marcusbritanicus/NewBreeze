#include "ZipFileInfo.h"
#include "CentralDirFileHeader.h"
#include "Utils.h"

#include <QTextCodec>

ZipInfo::ZipInfo() {

    fileSize = packedSize = unpackedSize = 0;
    fileCount = 0;
    version = 0;
    isZip64 = false;
    filesAdded = filesExtracted = filesDeleted = errorCount = 0;
};

ZipFileInfo::ZipFileInfo() {

    crc32 = 0;
    compressedSize = uncompressedSize = 0;
    permissions = 0;
    isFolder = isEncrypted = false;
    compressionMethod = 0;
    attributes = 0;
    version = madeByOS = 0;
};

ZipFileInfo::ZipFileInfo( CentralDirFileHeader* header ) {

    if ( header ) {

        filePath = header->sFileName;
        if (header->fileCommentLength > 0 ) {

            if ( header->isUTF8() )
                comment = QTextCodec::codecForUtfText(header->comment)->toUnicode( header->comment );

            else
                comment = QTextCodec::codecForLocale()->toUnicode( header->comment );
        }

        lastModifiedDate = header->lastModDate();

        crc32 = header->crc_32;
        compressedSize = header->getCompressedSize();
        uncompressedSize = header->getUncompressedSize();
        permissions = header->getPermissions();
        isFolder = header->isDirectory();
        compressionMethod = header->compressionMethod;
        isEncrypted = header->isEncrypted();
        attributes = (header->externalFileAttr & 0xff); // Just low byte
        version = LOBYTE( header->versionMadeBy );
        madeByOS = HIBYTE( header->versionMadeBy );
    }

    else {

        crc32 = 0;
        compressedSize = uncompressedSize = 0;
        permissions = 0;
        isFolder = isEncrypted = false;
        compressionMethod = 0;
        attributes = 0;
        version = madeByOS = 0;
    }
};
