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

class NBTabWidget;
class NBTab;

class NBUtilityBar : public QWidget {
	Q_OBJECT

	public:
		NBUtilityBar();
		~NBUtilityBar();

		NBTabWidget *tabs;
		NBIOManagerMini *procWidget;

	private:
		void createGUI();
		void setupConnections();

		NBToolButton *closeBtn;

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

class NBTabWidget : public QWidget {
	Q_OBJECT

	public:
		NBTabWidget();

		/* Add a tab: @param is the path, which helps us determine the icon and the tab name */
		int addTab( QString );

		/* Remove a tab: @param is the logical index */
		void removeTab( int );

		/* Switch to a tab */
		void setCurrentIndex( int );

		/* Get the current index */
		int currentIndex();

	private:
		QList<NBTab *> tabsList;
		QHBoxLayout *tabsLyt;

		int __currentIndex;

	private slots:
		void switchToTab();

	protected:
		void paintEvent( QPaintEvent * );
};

class NBTab : public QWidget {
	Q_OBJECT

	public:
		NBTab( QString, QString );

		void showText();
		void hideText();

	private:
		QString __text;
		bool __textVisible;
		QIcon __icon;

	protected:
		void paintEvent( QPaintEvent * );
		void mousePressEvent( QMouseEvent * );

	Q_SIGNALS:
		void clicked();
};

#endif
