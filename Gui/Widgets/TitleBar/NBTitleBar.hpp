/*
	*
	* NBGuiWidgets.hpp - NBGuiWidgets.cpp header
	*
*/

#ifndef NBTITLEBAR_HPP
#define NBTITLEBAR_HPP

#include <Global.hpp>
#include <NBGuiWidgets.hpp>

class NBTitleBar : public QWidget {
	Q_OBJECT

	public:
		NBTitleBar();
		~NBTitleBar();

		void setTitle( QString );
		void setIcon( QIcon );

		bool isMaximized;

	private:
		QString m_title;
		QIcon m_icon;

		QToolButton *closeBtn, *maxBtn, *minBtn;

		QPainter *painter;

	public slots:
		void maxBtnClicked();

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
		void titlebarMousePress( QMouseEvent * );
		void titlebarMouseMove( QMouseEvent * );

		void aboutNB();
		void aboutQt4();
};

class NBActionButtons : public QWidget {
	Q_OBJECT

	public:
		NBActionButtons( bool, bool, bool );
		~NBActionButtons();

	private:
		QPainter *painter;

		bool maxBtnEnabled;
		bool minBtnEnabled;
		bool closeBtnEnabled;

		quint64 widgetWidth;

		QRect minRect = QRect( 0, 0, 0, 0 );
		QRect maxRect = QRect( 0, 0, 0, 0 );
		QRect clsRect = QRect( 0, 0, 0, 0 );

	protected:
		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );
		void paintEvent( QPaintEvent * );

	Q_SIGNALS:
		void maximizeWindow();
		void minimizeWindow();
		void closeWindow();
};

#endif
