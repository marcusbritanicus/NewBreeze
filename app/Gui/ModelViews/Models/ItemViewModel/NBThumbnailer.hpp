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
};
