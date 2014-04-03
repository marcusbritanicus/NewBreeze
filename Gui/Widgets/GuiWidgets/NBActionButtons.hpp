/*
	*
	* NBActionButtons.cpp - Minimize/Maximize/Close Button Widget Class
	*
*/

#pragma once
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

		QRect minRect;
		QRect maxRect;
		QRect clsRect;

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
