/*
	*
	* NBStyleManager.hpp - NBStyleManager.cpp Header
	*
*/

#ifndef NBSTYLEMANAGER_HPP
#define NBSTLYEMANAGER_HPP

#include <Global.hpp>

class NBStyleManager {

	public:
		NBStyleManager();
		QString getStyleSheet( QString widget, QString style );

	private:
		QString getStyleSheetTD( QString widget );
		QString getStyleSheetTL( QString widget );
		QString getStyleSheetLG( QString widget );
		QString getStyleSheetDB( QString widget );

		QMap<QString, QStringList> variableMap;
};

static NBStyleManager styleManager;

#endif
