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
		void closeBtnClicked();
		void minBtnClicked();
		void maxBtnClicked();

	protected:
		void mouseDoubleClickEvent( QMouseEvent * );
		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );
		void paintEvent( QPaintEvent * );

	signals:
		void restoreWindow();
		void maximizeWindow();
		void minimizeWindow();

		void closeWindow();
		void titlebarMousePress( QMouseEvent * );
		void titlebarMouseMove( QMouseEvent * );

		void aboutNB();
		void aboutQt4();
};

#endif
