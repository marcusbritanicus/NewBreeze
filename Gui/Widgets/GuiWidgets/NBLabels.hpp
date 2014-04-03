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

class NBPicLabel : public QLabel {
	Q_OBJECT

	public:
		NBPicLabel( QPixmap, QString );
		QString Style;

	protected:
		virtual void mousePressEvent( QMouseEvent * );

	signals:
		void clicked();
};

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

	signals:
		void clicked();
};

class NBInfoLabel : public QLabel {
	Q_OBJECT

	public:
		NBInfoLabel();
		void setText( QString, QString, QString, QString );
};

#endif
