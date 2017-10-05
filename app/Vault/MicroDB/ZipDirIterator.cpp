
#include "ZipDirIterator.h"
#include "AbZip.h"
#include "abzip_p.h"
#include "CentralDirFileHeader.h"
#include "Utils.h"

#include <QVector>
#include <QRegExp>

class ZipDirIteratorPrivate
{

public:
    ZipDirIteratorPrivate( const AbZip& zip, const QString& path, const QStringList &nameFilters,
                        AbZip::ZipOptions options );

    void findFirst();
    void advance();

    bool entryMatches(const QString & fileName, CentralDirFileHeader* header);
    bool matchesFilters(const QString &fileName, CentralDirFileHeader* header ) const;
    bool hasNext() const;

    const AbZip& zip;
    QString path;
    const QStringList nameFilters;
    const AbZip::ZipOptions options;

    QVector<QRegExp> nameRegExps;

    CentralDirFileHeader* currentHeader;
    CentralDirFileHeader* nextHeader;
};


ZipDirIteratorPrivate::ZipDirIteratorPrivate(const AbZip& zip, const QString& path, const QStringList &nameFilters,
                                         AbZip::ZipOptions options )
    : zip(zip)
      ,path(path)
      , nameFilters(nameFilters.contains(QLatin1String("*")) ? QStringList() : nameFilters)
      , options(options)
{
    currentHeader = NULL;
    nextHeader = NULL;

    nameRegExps.reserve(nameFilters.size());
    for (int i = 0; i < nameFilters.size(); ++i)
        nameRegExps.append( QRegExp(nameFilters.at(i),
                    (options & AbZip::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive,
                    QRegExp::Wildcard));


    // Populate headers for hasNext() and next()
    findFirst();
}


void ZipDirIteratorPrivate::findFirst()
{
    CentralDirFileHeader* header = zip.d_ptr->centralDir->getFirstFile();

    while ( header )
    {
        if (path.isEmpty() ||
                header->sFileName.startsWith( path, (options & AbZip::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive ) )
        {
            // Check our filters
            if (entryMatches( header->sFileName, header ))
            {
                currentHeader = nextHeader;
                return;
            }
        }

        // Try next file
        header = zip.d_ptr->centralDir->getNextFile();
    }
}

// This gets the next header if one is found
void ZipDirIteratorPrivate::advance()
{
    if ( !nextHeader )
    {
        currentHeader = NULL;
        return;
    }

    CentralDirFileHeader* header;
    while ( (header = zip.d_ptr->centralDir->getNextFile()) )
    {
        if (!path.isEmpty() &&
                !header->sFileName.startsWith( path, (options & AbZip::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive ) )
            break;  // reached the end of the 'path' we are searching for

        // Check our filters
        if (entryMatches( header->sFileName, header ))
            return;
    }

    // no more items to iterate, take final 'nextHeader'
    if ( nextHeader )
        currentHeader = nextHeader;
    nextHeader = NULL;
}


bool ZipDirIteratorPrivate::hasNext() const
{
    return nextHeader ? true : false;
}

inline bool ZipDirIteratorPrivate::entryMatches(const QString & fileName, CentralDirFileHeader* header)
{
    if (matchesFilters(fileName, header))
    {
        //We found a matching entry.
        currentHeader = nextHeader;
        nextHeader = header;
        return true;
    }

    return false;
}

bool ZipDirIteratorPrivate::matchesFilters(const QString &fileName, CentralDirFileHeader* header ) const
{
    Q_ASSERT(!fileName.isEmpty());

    // Pass all entries through name filters
    if ( !nameFilters.isEmpty() )
    {
        bool matched = false;
        for (QVector<QRegExp>::const_iterator iter = nameRegExps.constBegin(), end = nameRegExps.constEnd(); iter != end; ++iter)
        {
            QRegExp copy = *iter;
            if (copy.exactMatch(fileName))
            {
                matched = true;
                break;
            }
        }
        if (!matched)
            return false;
    }

    // skip directories
    if ( (options & AbZip::IgnoreFolders) && header->isDirectory())
        return false;

    // skip files
    if ( (options & AbZip::IgnoreFiles) && !header->isDirectory())
        return false;

    return true;
}


ZipDirIterator::ZipDirIterator(const AbZip& zip, const QString &path, AbZip::ZipOptions options)
    : d_ptr(new ZipDirIteratorPrivate(zip, path, QStringList(), options ))
{
}


ZipDirIterator::ZipDirIterator(const AbZip& zip, const QString &path, const QStringList &nameFilters,
                           AbZip::ZipOptions options)
    : d_ptr(new ZipDirIteratorPrivate(zip, path, nameFilters, options))
{
}

ZipDirIterator::~ZipDirIterator()
{
    delete d_ptr;
}

// Private get functions
CentralDirFileHeader* ZipDirIterator::getCurrentHeader()
{
    if ( d_ptr->currentHeader )
        return d_ptr->currentHeader;

    return NULL;
}


QString ZipDirIterator::next()
{
    d_ptr->advance();
    if ( d_ptr->currentHeader )
        return d_ptr->currentHeader->sFileName;

    return QString();
}

bool ZipDirIterator::hasNext() const
{
    return d_ptr->hasNext();
}

QString ZipDirIterator::fileName() const
{
    if ( d_ptr->currentHeader )
        return ZipUtils::getFileName( d_ptr->currentHeader->sFileName );
    return QString();
}

QString ZipDirIterator::filePath() const
{
    if ( d_ptr->currentHeader )
        return d_ptr->currentHeader->sFileName;
    return QString();
}

ZipFileInfo ZipDirIterator::fileInfo() const
{
    return ZipFileInfo( d_ptr->currentHeader );
}

QString ZipDirIterator::path() const
{
    if ( d_ptr->currentHeader )
        return QDir( d_ptr->currentHeader->sFileName ).path();
    return QString();
}





// Sorting
struct ZipDirSortItem
{
    mutable QString filename;
    mutable QString suffix;
    ZipFileInfo item;
};


class ZipDirSortItemComparator
{
    AbZip::ZipOptions options;
public:
    ZipDirSortItemComparator(AbZip::ZipOptions options) : options(options) {}
    bool operator()(const ZipDirSortItem &, const ZipDirSortItem &) const;
};

bool ZipDirSortItemComparator::operator()(const ZipDirSortItem &n1, const ZipDirSortItem &n2) const
{
    const ZipDirSortItem* f1 = &n1;
    const ZipDirSortItem* f2 = &n2;

    qint64 r = 0;

    if ( options.testFlag( AbZip::SortByTime ) )
    {
        QDateTime firstModified = f1->item.lastModifiedDate;
        QDateTime secondModified = f2->item.lastModifiedDate;

        // QDateTime by default will do all sorts of conversions on these to
        // find timezones, which is incredibly expensive. As we aren't
        // presenting these to the user, we don't care (at all) about the
        // local timezone, so force them to UTC to avoid that conversion.
        firstModified.setTimeSpec(Qt::UTC);
        secondModified.setTimeSpec(Qt::UTC);

        r = firstModified.msecsTo(secondModified);
    }
    else if ( options.testFlag( AbZip::SortByCompressedSize ) )
    {
        r = f2->item.compressedSize - f1->item.compressedSize;
    }
    else if ( options.testFlag( AbZip::SortByUncompressedSize ) )
    {
        r = f2->item.uncompressedSize - f1->item.uncompressedSize;
    }
    else if ( options.testFlag( AbZip::SortByType ) )
    {
        if ( options.testFlag( AbZip::CaseSensitive ) )
        {
            f1->suffix = ZipUtils::getFileSuffix( f1->item.filePath );
            f2->suffix = ZipUtils::getFileSuffix( f2->item.filePath );
        }
        else
        {
            f1->suffix = ZipUtils::getFileSuffix( f1->item.filePath ).toLower();
            f2->suffix = ZipUtils::getFileSuffix( f2->item.filePath ).toLower();
        }
        r = f1->suffix.compare(f2->suffix);
    }
    else
    {
        if ( options.testFlag( AbZip::CaseSensitive ) )
        {
            f1->filename = ZipUtils::getFileName( f1->item.filePath );
            f2->filename = ZipUtils::getFileName( f2->item.filePath );
        }
        else
        {
            f1->filename = ZipUtils::getFileName( f1->item.filePath ).toLower();
            f2->filename = ZipUtils::getFileName( f2->item.filePath ).toLower();
        }

        r = f1->filename.compare(f2->filename);
    }

    if ( options.testFlag( AbZip::SortReversed) )
        return r > 0;

    return r < 0;
}

void sortInfoList(QList<ZipFileInfo> &list, QList<ZipFileInfo>& sortedList, AbZip::ZipOptions options)
{
    int n = list.size();

    QScopedArrayPointer<ZipDirSortItem> si(new ZipDirSortItem[n]);

    for (int i = 0; i < n; ++i)
        si[i].item = list.at(i);

    std::sort(si.data(), si.data() + n, ZipDirSortItemComparator(options));

    for (int i = 0; i < n; ++i)
        sortedList.append(si[i].item);
}
