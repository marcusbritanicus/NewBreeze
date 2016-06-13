/*
	*
	* NBDeleteManager.hpp - NBDeleteManager.cpp header
	*
*/

#pragma once

#include "Global.hpp"

class NBDeleter;
class NBTrasher;

class NBDeleteManager : public QObject {
	Q_OBJECT

	public:
		NBDeleteManager( QObject*, bool );

		void deleteFromDisk( QStringList );
		void sendToTrash( QStringList );

	private:
		NBDeleter *deleter;
		NBTrasher *trasher;
		QThread *thread;

	private slots:
		void failureHandler( QStringList, QStringList );

	Q_SIGNALS:
		void deleteOperationComplete( QStringList, QStringList );
		void trashOperationComplete( QStringList, QStringList );
};

class NBDeleter : public QObject {
	Q_OBJECT

	public:
		explicit NBDeleter( QObject *parent = 0 );

	private:
		void removeDir( QString );
		void removeFile( QString );

		QStringList failedFiles;
		QStringList failedDirs;

	public slots:
		void deleteFilesAndFolders( QStringList );

	Q_SIGNALS:
		void completed( QStringList, QStringList );
};

class NBTrasher : public QObject {
	Q_OBJECT

	public:
		explicit NBTrasher( QObject *parent = 0 );

	private:
		void removeDir( QString );
		void removeFile( QString );

		QStringList failedFiles;
		QStringList failedDirs;

	public slots:
		void trashFilesAndFolders( QStringList );

	Q_SIGNALS:
		void completed( QStringList, QStringList );
};
