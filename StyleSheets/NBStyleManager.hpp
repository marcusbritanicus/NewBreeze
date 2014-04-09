/*
	*
	* NBStyleManager.hpp - NBStyleManager.cpp Header
	*
*/

#pragma once
#ifndef NBSTYLEMANAGER_HPP
#define NBSTLYEMANAGER_HPP

#include <Global.hpp>

class NBStyleManager {

	public:
		static QString getStyleSheet( QString widget, QString style );
		static QPalette getPalette( QString style );
		static QPalette transparentPalette();

	private:
		static QString getStyleSheetTD( QString widget );
		static QString getStyleSheetTL( QString widget );
		static QString getStyleSheetLG( QString widget );
		static QString getStyleSheetDB( QString widget );

		static QPalette getPaletteTD();
		static QPalette getPaletteTL();
		static QPalette getPaletteLG();
		static QPalette getPaletteDB();

		static void initStyleManager();

		static bool initDone;
		static QMap<QString, QStringList> variableMap;
};

#endif
