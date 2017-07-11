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

		void createThumbnails( QString path );

		inline void terminate() {

			mTerminate = true;
			wait();
		};

	protected:
		void run();

	private:
		bool mTerminate;
		QString mPath;

	Q_SIGNALS :
		void updateNode( QString );
};
