/*
	*
	* NBClickableLabel.hpp - Clickable Label for SideBar Plugin
	*
	* The original file was a part of Quartica, released under GPL v3
	* Copyright ( c ) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
	*
	* Modified: Marcus Britanicus
	*
*/

#pragma once

#include <QLabel>

class NBClickableLabel : public QLabel {
	Q_OBJECT

	public:
		NBClickableLabel( QWidget *parent = 0, Qt::WindowFlags f = 0 );
		NBClickableLabel( const QString &text, QWidget *parent = 0, Qt::WindowFlags f = 0 );

	protected:
		void mousePressEvent( QMouseEvent * );
		void mouseReleaseEvent( QMouseEvent *event );

	Q_SIGNALS:
		void pressed();
		void released();

		void clicked( void );
		void clicked( NBClickableLabel *label );
};
