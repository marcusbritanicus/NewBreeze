/*
	*
	* NBSideBarEntry.hpp - SidePanelItem class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDeleteProcess.hpp"
#include "NBProcessManager.hpp"
#include "NBSideBarMenu.hpp"

class NBSideBarEntry : public QLabel {
	Q_OBJECT

	public:
		NBSideBarEntry( QString icon, QString name, QWidget *parent );

	private:
		QString mName;
		QString mIconStr;
		QPixmap mIcon;

		QBasicTimer menuTimer;

	public Q_SLOTS:
		virtual void showMenu( bool autoClose = false );

	protected:
		void mouseReleaseEvent( QMouseEvent *mrEvent );

		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );

		void timerEvent( QTimerEvent * );

	Q_SIGNALS:
		void clicked();
		void driveClicked( QString );
};

class NBSideBarTrashEntry : public NBSideBarEntry {
	Q_OBJECT

	public:
		NBSideBarTrashEntry( QString icon, QString name, QWidget *parent );

	private:
		QString mName;
		QPixmap mIcon;

	public Q_SLOTS:
		void showMenu() {};

	protected:
		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );
};
