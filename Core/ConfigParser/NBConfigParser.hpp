/*
	*
	* NBConfigParser.hpp - NBConfigParser Header
	*
*/

#pragma once
#ifndef NBCONFIGPARSER_HPP
#define NBCONFIGPARSER_HPP

#include <Global.hpp>

class NBConfigParser {

	public :
		NBConfigParser( QStringList dPths );

		static void createCache();
		static QMap<QString, QStringList> parseDir( QString dirPath );
		static QMap<QString, QStringList> parseDirs( QStringList dPths );

		static QStringList dirPaths;
};

#endif
