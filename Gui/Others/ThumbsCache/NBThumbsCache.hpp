/*
	*
	* NBThumbsCache.hpp - NBThumbsCache.cpp header
	*
*/

#ifndef NBTHMUBSCACHE_HPP
#define NBTHMUBSCACHE_HPP

#include <Global.hpp>

class NBThumbsCache {

	public :
		static QPixmap thumbnail( QString, QSize );

	private :
		static QPixmap getThumb( QString );
		static QPixmap storeThumb( QString );

		static bool hasStoredThumb( QString );
};

class NBIcon {

	public :
		static QPixmap icon( QFileInfo, QSize );
		static QIcon icon( QString, QString );
		static void loadCacheToMemory();
		static bool hasIcon( QString );

	private :
		static QStringList getThemeDirs( QString );
		static void buildCacheOld();
		static void buildCache();
		static QStringList getIconForNewSuffix( QString );
		static QStringList getIconForNewMime( QString );
		static QPixmap NoSuffixIcon( QString );
};

#endif
