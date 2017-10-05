
#ifndef ZIPDIRITERATOR_H
#define ZIPDIRITERATOR_H

#include "zipglobal.h"
#include "AbZip.h"

#include <QDir>

class ZipDirIteratorPrivate;
class CentralDirFileHeader;

/**
 * @brief This class provides a search or extraction directory iterator.  You can use this class to
 * navigate entries within the archive. A starting root folder and file name fitlers can be applied.
 *
 * Here is an example of how to find all PDF files within the archive.
\verbatim
    AbZip zip("myarchive.zip");

    ZipDirIterator it( zip, "", QStringList() << "*.pdf", AbZip::Recursive );
    while (it.hasNext())
    {
        it.next();
        ZipFileInfo info  = it.fileInfo();
        qDebug() << info.filePath << info.lastModifiedDate;
    }
    zip.close();
\endverbatim
 */
class ZIP_EXPORT ZipDirIterator
{
public:

    /**
     * @brief Constructs a ZipDirIterator class ready to search the archive.
     * You can pass options to decide how the directory should be iterated.
     * @param zip   The AbZip class
     * @param path  The root folder within the archive to start iterating from.
     * @param options Iterator options (Recursive, CaseSensitive, IgnoreFolders, IgnoreFiles)
     */
    ZipDirIterator(const AbZip& zip, const QString &path = QString(), AbZip::ZipOptions options = AbZip::None);

    /**
     * @brief Constructs a ZipDirIterator class ready to search the archive using nameFilters.
     * You can pass options to decide how the directory should be iterated.
     * @param zip   The AbZip class
     * @param path  The root folder within the archive to start iterating from.
     * @param nameFilters Set name filters to search for (e.g. "*.pdf" or "*abc*)
     * @param options Iterator options (Recursive, CaseSensitive, IgnoreFolders, IgnoreFiles)
     */
    ZipDirIterator(const AbZip& zip, const QString &path, const QStringList &nameFilters, AbZip::ZipOptions options = AbZip::None);

    /**
     * @brief Destroys the ZipDirIterator
     */
    virtual ~ZipDirIterator();

    /**
     * @brief next Advances the iterator to the next matching entry, and returns the file path of this new entry.
     * If hasNext() returns false, this function does nothing, and returns a null QString.
     * @return Returns the full path and file name to the found entry.
     */
    QString next();
    /**
     * @brief hasNext Tests if their is at least one more matching entry found.
     * @return Returns true if there is at least one more entry in the archive; otherwise, false is returned.
     */
    bool hasNext() const;

    /**
     * @brief fileName Returns the file name for the current entry, without the path prepended.
     * @return A QString containing the file name only
     */
    QString fileName() const;

    /**
     * @brief filePath Returns the full file and path for the current entry.
     * @return A QString containing the full path and file name.
     */
    QString filePath() const;

    /**
     * @brief fileInfo Get a ZipFileInfo class containing all the information about the entry in the archive.
     * @return A ZipFileInfo class containing all the information about the file. This will contain empty data if
     * no entry was found
     */
    ZipFileInfo fileInfo() const;

    /**
     * @brief path Returns the path only of the found entry.
     * @return A QString containing the path only.
     */
    QString path() const;

private:
    Q_DISABLE_COPY(ZipDirIterator)

    ZipDirIteratorPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QDirIterator)

    CentralDirFileHeader* getCurrentHeader();

    friend class AbZipPrivate;
};

void sortInfoList(QList<ZipFileInfo> &list, QList<ZipFileInfo>& sortedList, AbZip::ZipOptions options);

#endif
