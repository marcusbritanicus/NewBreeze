/*
	*
	* NBUtilityBar.hpp - NewBreeze UtilityBar Class HEader
	*
*/

#pragma once
#ifndef NBUTILITYBAR_HPP
#define NBUTILITYBAR_HPP

#include <Global.hpp>
#include <NBGuiWidgets.hpp>
#include <NBIOManager.hpp>

class NBTabs;

class NBUtilityBar : public QWidget {
	Q_OBJECT

	public:
		NBUtilityBar();
		~NBUtilityBar();

	private:
		void createGUI();
		void setupConnections();

		NBTabs *tabs;
		NBToolButton *closeBtn;
		NBToolButton *procWidget;

	protected:
		void mouseDoubleClickEvent( QMouseEvent * );
		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );

		void paintEvent( QPaintEvent * );

	Q_SIGNALS:
		void restoreWindow();
		void maximizeWindow();
		void minimizeWindow();

		void closeWindow();
		void titleBarMousePress( QMouseEvent * );
		void titleBarMouseMove( QMouseEvent * );

		void aboutNB();
		void aboutQt4();
};

class NBTabs : public QTabBar {
	Q_OBJECT

	public:
		NBTabs();

		/* Add a tab: @param is the path, which helps us determine the icon and the tab name */
		int addTab( QString );

		/* Remove a tab: @param is the logical index */
		void removeTab( int );

	protected:
		void paintEvent( QPaintEvent * );
};

#endif
