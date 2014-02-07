/*
	*
	* NBGuiWidgets.hpp - NBGuiWidgets.cpp header
	*
*/

#pragma once
#ifndef NBGUIWIDGETS_HPP
#define NBGUIWIDGETS_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBLabels.hpp>

class NBSearchBar : public QWidget {
	Q_OBJECT

	public:
		NBSearchBar();
		QLineEdit *searchLE;

	private slots:
		void searchChanged( QString );

	Q_SIGNALS:
		void searchString( QString );
		void searchCleared();
};

class NBMenu : public QMenu {
	Q_OBJECT

	public:
		NBMenu( QWidget *parent = 0 );
		NBMenu( QString title, QWidget *parent = 0 );
};

class NBToolButton : public QToolButton {
	Q_OBJECT

	public:
		NBToolButton( QString themeIcon = QString(), QString customIcon = QString() );
};

class NBPushButton : public QPushButton {
	Q_OBJECT

	public:
		NBPushButton( QIcon icon, QString text );
};

class NBTitleIcon : public QLabel {
	Q_OBJECT

	public:
		NBTitleIcon( QString icon );

	private :
		QMenu menu;

	protected:
		virtual void mousePressEvent( QMouseEvent * );

	signals:
		void aboutNB();
		void aboutQt4();
};

class NBViewModeButton: public QPushButton {
	Q_OBJECT

	public:
		NBViewModeButton();
		int checkedAction();

	private :
		QAction *tilesAct, *iconsAct, *detailsAct;
		QActionGroup *viewsGroup;

	public slots:
		void showMenu();

	Q_SIGNALS:
		void changeViewMode();
		void switchToNextView();
};

class NBDriveLabel : public QWidget {
	Q_OBJECT

	public:
		NBDriveLabel( const QString path );
		~NBDriveLabel();

	private:
		QPainter *painter;
		QString label;

	protected:
		void paintEvent( QPaintEvent * );
};

class NBDriveInfo : public QFrame {
	Q_OBJECT

	public:
		NBDriveInfo( qint64 used, qint64 total );
		~NBDriveInfo();

	private:
		QPainter *painter;
		QString label;
		float percent;

	protected:
		virtual void paintEvent( QPaintEvent * );
};

class Separator {

	public:
		static QWidget* vertical();
		static QWidget* horizontal();
};

class NBSpacer {

	public:
		static QWidget* vertical( int spacing = 1 );
		static QWidget* horizontal( int spacing = 1 );
};

#endif
