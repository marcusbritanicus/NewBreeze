/* 
 * File:   document.cpp
 * Author: pro
 * Struzzo bello in fare le cose
 * Created on 24. Oktober 2013, 11:36
 */
#include <QtCore/QByteArray>
#include <QtCore/QMap>
#include <QtCore/QString>
#include "document.h"
#include "kzip.h"
#include <QtCore/QDebug>
#include <QImage>


using namespace OOO;

Document::Document(const QString &fileName)
: mFileName(fileName) {
    /// password protected file search ManifestEntry::ManifestEntry( const QString &fileName ) : on okular
}

bool Document::open() {
    mContent.clear();
    mStyles.clear();
    KZipStream *Kzip = new KZipStream(mFileName);
    if (Kzip->canread()) {
        const QStringList entries = Kzip->filelist();
        ODTDEBUG() << "entry list:" << entries;

        if (!entries.contains("META-INF")) {
            setError(i18n("Invalid document structure (META-INF directory is missing)"));
            return false;
        }

        if (!entries.contains("content.xml")) {
            setError(i18n("Invalid document structure (content.xml is missing)"));
            return false;
        }

        if (!entries.contains("styles.xml")) {
            /// nokia qt odt export dont write
            setError(i18n("Invalid document structure (styles.xml is missing)"));
            ////  return false;
        }

        if (!entries.contains("meta.xml")) {
            /// nokia qt odt export dont write
            setError(i18n("Invalid document structure (meta.xml is missing)"));
            //// return false;
        }

        /// fill files
        //// QMap<QString,QByteArray> listData()
        QMap<QString, QByteArray> allfiles = Kzip->listData();
        QMapIterator<QString, QByteArray> i(allfiles);
        while (i.hasNext()) {
            i.next();
            QByteArray xdata(i.value());
            QImage pic; /// i.value() /// i.key(); 
            const QString namex = QString(i.key());
            pic.loadFromData(xdata);
            if (!pic.isNull()) {
                mImages.insert(namex, xdata);
                ODTDEBUG() << "New name pics len:" << namex << " s:" << xdata.size();
            } else {
                ODTDEBUG() << "New name xml len:" << namex << " s:" << xdata.size();
                if (namex == "styles.xml") {
                    mStyles = xdata;
                }
                if (namex == "content.xml") {
                    mContent = xdata;
                }
                if (namex == "META-INF/manifest.xml") {
                    mMeta = xdata;
                }
                if (namex == "mimetype") {
                    mMimetype = xdata;
                }
                if (namex == "settings.xml") {
                    mSetting = xdata;
                }
            }

        }
        if (QByteArray("application/vnd.oasis.opendocument.text") != mMimetype) {
            setError(i18n("Invalid MimeType found: %1!").arg(QString::fromUtf8(mMimetype)));
            return false;
        }
        
        Tools::_write_file( "/Users/pro/Desktop/kdemini/cbody.xml",  content() , "utf-8");
        
        //// ODTDEBUG() << "heja :" << content(); 
        return true;
    }
    return false;
}

Document::~Document() {
    /// delete mManifest;
}

QString Document::lastErrorString() const {
    return mErrorString;
}

QByteArray Document::content() const {
    return mContent;
}

QByteArray Document::meta() const {
    return mMeta;
}

QByteArray Document::styles() const {
    return mStyles;
}

QMap<QString, QByteArray> Document::images() const {
    return mImages;
}

void Document::setError(const QString &error) {
    ODTDEBUG() << "New error:" << error;
    mErrorString = error;
}

