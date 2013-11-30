/*
	*
	* NBTitleBar.hpp - NewBreeze TitleBar Class HEader
	*
*/

#ifndef NBTITLEBAR_HPP
#define NBTITLEBAR_HPP

#include <Global.hpp>
#include <NBGuiWidgets.hpp>
#include <NBActionButtons.hpp>

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

		NBActionButtons *actBtns;
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

#endif
