/*
	*
	* NBSideBarGroup.hpp - SideBarGruop class Header
	*
*/

#pragma once

#include "Global.hpp"

class NBMenuItem : public QLabel {
	Q_OBJECT

	public:
		NBMenuItem( QString name, QString icon, QWidget *parent );

		QString name();
		QIcon icon();

	private:
		QString mName;
		QString mIcon;

		bool mPressed;
		bool mHover;

	protected:
		void enterEvent( QEvent* );
		void leaveEvent( QEvent* );

		void mouseReleaseEvent( QMouseEvent *mrEvent );
		void mousePressEvent( QMouseEvent *mpEvent );
		void paintEvent( QPaintEvent *pEvent );

	Q_SIGNALS:
		void clicked();
};

class NBMenuItemButton : public QToolButton {
	Q_OBJECT

	public:
		NBMenuItemButton( QString icon, QWidget *parent );
};

class NBMenuItemCheck : public QCheckBox {
	Q_OBJECT

	public:
		NBMenuItemCheck( QString text, QWidget *parent );
};

class NBSystemMenu : public QWidget {
	Q_OBJECT

	public:
		NBSystemMenu( QWidget *parent );

		void populateMenu();

	public Q_SLOTS:
		void exec( QPoint );

	private:
		QList<NBMenuItem*> itemList;

		QWidget *itemsBase;
		QVBoxLayout *itemsLayout;

	protected:
		void closeEvent( QCloseEvent *cEvent );

	Q_SIGNALS:
		void clicked();
		void closed();
};
