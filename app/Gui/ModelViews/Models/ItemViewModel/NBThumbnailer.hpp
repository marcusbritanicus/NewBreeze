/*
	*
	* NBThumbnailer.hpp - NewBreeze Thumbnailer Header
	*
*/

#pragma once

#include "Global.hpp"

class NBThumbnailer : public QThread {
	Q_OBJECT

	public:
		inline NBThumbnailer() {

			mTerminate = false;
		};

		void createThumbnails( QString path, QStringList nodes );
		void createThumbnails( QStringList nodes );

		inline void terminate() {

			mTerminate = true;
			wait();
		};

	protected:
		void run();

	private:
		bool mTerminate;

		QStringList images;
		QStringList documents;
		QStringList videos;
		QStringList pdfs;
		QStringList djvus;
		QStringList epubs;
		QStringList cbzs;

	Q_SIGNALS :
		void updateNode( QString );
		void saveThumb( QString );
};

class NBThumbSaver : public QThread {
	Q_OBJECT

	public:
		NBThumbSaver();

	public Q_SLOTS:
		/* Save the generated thumbnails */
		void save( QString filename );

	protected:
		/* Loop to generate thumbnails one by one and intimate the model */
		void run();

	private:
		/* I don't trust Qthread::isRunning(), custom flag to indicate that */
		bool isActive;

		/* List of files whose thumbnails are to be generated. */
		QStringList fileList;
};
