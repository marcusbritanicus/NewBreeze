/*
	*
	* NBWindowButton.hpp - NBWindowButton.cpp Class Header
	*
*/

#pragma once

#include <Global.hpp>

class NBWindowButton : public QWidget {
	Q_OBJECT

	public:
		/* Min/Max/Close button */
		NBWindowButton( QColor, QWidget *parent = NULL );

	private:
		QColor mColor;
		qreal cStop;

		QBasicTimer timer;

	protected:
		void enterEvent( QEvent* );
		void leaveEvent( QEvent* );

		void mouseReleaseEvent( QMouseEvent* );
		void paintEvent( QPaintEvent* );

		void timerEvent( QTimerEvent* );

	Q_SIGNALS:
		void clicked();
};
