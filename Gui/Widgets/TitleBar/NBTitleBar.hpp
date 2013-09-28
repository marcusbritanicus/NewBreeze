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
		void setTitle( QString );
		void setIcon( QIcon );

		bool isMaximized;
		NBTitleIcon *iconLbl;

	private:
		QLabel *titleLbl;

		QToolButton *closeBtn, *maxBtn, *minBtn;

	public slots:
		void closeBtnClicked();
		void minBtnClicked();
		void maxBtnClicked();

	protected:
		void mouseDoubleClickEvent( QMouseEvent * );
		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );

	signals:
		void restoreWindow();
		void maximizeWindow();
		void minimizeWindow();
		void closeWindow();
		void titlebarMousePress( QMouseEvent * );
		void titlebarMouseMove( QMouseEvent * );
};

#endif
