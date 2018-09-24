/*
	*
	* NBAutoMount.hpp - NewBreeze AutoMount Class Header
	*
*/

#pragma once

#include <QtCore>
#include <QtDBus>
#include "udisks2.h"

class NBAutoMount : public QObject {
	Q_OBJECT

	public:
		static NBAutoMount *instance();
		~NBAutoMount();

		void start();
		void stop();

	public Q_SLOTS:
		void mountFS( QString );

	private:
		NBAutoMount();

		UDisks2 *UDI;
		bool running;
		bool init;

		static NBAutoMount *autoMount;
};
