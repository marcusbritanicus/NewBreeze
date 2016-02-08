/*
	*
	* NewBreeze.hpp - NewBreeze Header
	*
*/

#ifndef NEWBREEZETRAY_HPP
#define NEWBREEZETRAY_HPP

#include <Global.hpp>

class NBTrayIcon : public QSystemTrayIcon {
	Q_OBJECT

	public :
		NBTrayIcon();

	public slots:
		void handleActivation( QSystemTrayIcon::ActivationReason reason );

	private slots:
		void showInfo();

	Q_SIGNALS:
		void newWindow();
		void toggleVisible();
};

#endif
