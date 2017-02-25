/*
	*
	* NBFileSystemWatcher.hpp - NBFileSystemWatcher Class Header
	*
*/

#pragma once

#include "Global.hpp"

class NBFileSystemWatcher : public QFileSystemWatcher {
	Q_OBJECT

	public:
		NBFileSystemWatcher();

		void startWatch( QString );
		void stopWatch();

	private Q_SLOTS:
		void handleChanged( QString );

	private:
		QString watchPath;
		QStringList contents;
		QStringList renames;

	Q_SIGNALS :
		void nodeCreated( QString );
		void nodeDeleted( QString );
		void nodeChanged( QString );
		void nodeRenamed( QString, QString );

		void watchPathDeleted();

		void watchFailed();
};
