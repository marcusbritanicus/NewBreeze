/*
	*
	* PasswordKeypad.hpp - PasswordKeypad class header
	*
*/

#pragma once

#ifdef STANDALONE
	#include "Global2.hpp"
#else
	#include "Global.hpp"
#endif
#include "NBCircle.hpp"

class PasswordKeypad : public QWidget {
	Q_OBJECT

	public:
		PasswordKeypad( QWidget *parent = NULL );

		QString password();
		void clear();

		void setInvalid();

	private:
		bool pressed;
		QPoint curPos;

		QList<QPoint> centers;
		QList<Circle> circles;
		QList<QPoint> points;

		QHash<QPoint, QChar> pointsCharMap;
		QString m_password;

		bool mInvalid;

	protected:
		void mousePressEvent( QMouseEvent* );
		void mouseMoveEvent( QMouseEvent* );
		void mouseReleaseEvent( QMouseEvent* );

		void paintEvent( QPaintEvent* );

	Q_SIGNALS:
		void inputComplete();
};
