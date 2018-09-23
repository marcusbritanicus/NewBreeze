/*
	*
	* NBLabels.hpp - NBLabels.cpp header file
	*
*/

#pragma once

#include "Global.hpp"

class NBClickLabel : public QLabel {
	Q_OBJECT

	public:
		NBClickLabel( QPixmap icon = QPixmap(), QWidget *parent = NULL );
		NBClickLabel( QString text = QString(), QWidget *parent = NULL );

		void setClickable( bool );

	private :
		bool clickEnabled;

	protected:
		void mousePressEvent( QMouseEvent * );
		void mouseReleaseEvent( QMouseEvent * );

	signals:
		void pressed();
		void clicked();
		void released();
};

class NBInfoLabel : public QLabel {
	Q_OBJECT

	public:
		NBInfoLabel();
		void setText( QString, QString, QString, QString );
};
