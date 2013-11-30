/*
	*
	* NBActionButtons.cpp - Minimize/Maximize/Close Button Widget Class
	*
*/

#ifndef NBACTIONBUTTONS_HPP
#define NBACTIONBUTTONS_HPP

#include <Global.hpp>

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
