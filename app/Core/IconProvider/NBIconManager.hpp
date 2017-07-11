/*
	*
	* NBIconManager.hpp - Icon Manager for NewBreeze
	*
*/

#pragma once

#include "Global.hpp"

typedef QMap<QString, QString> DeviceInfo;
typedef QPair<QString, QString> BookMark;

class NBIconManager {

	public:
		/* Unique single instance */
		static NBIconManager *instance();

		/* Icon string for file type: @mName: Mime Name */
		QStringList iconsForFile( QString mName, QString file = QString() );

		/* Icon string for file type: @mName: Mime Name */
		QStringList icon( QString iName );

		/* Icon string for file type: @mName: Icon Name */
		bool hasIcon( QString iName );

		/* Thumbnail for an image file */
		QString imageThumbnail( QString );

	private:
		/* Private init function */
		NBIconManager();

		/* Generate theme database */
		void generateThemeDatabase();

		/* Global Instance */
		static NBIconManager *iMgr;
		bool init;

		/* Default theme location */
		QString iconThemePath;
};
