/*
	*
	* NBLabels.hpp - NBLabels.cpp header file
	*
*/

#pragma once
#ifndef NBLABELS_HPP
#define NBLABELS_HPP

#include <Global.hpp>
#include <NBTools.hpp>

class NBClickLabel : public QLabel {
	Q_OBJECT

	public:
		NBClickLabel( QPixmap icon = QPixmap() );
		NBClickLabel( QString icon = QString() );
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

#endif
