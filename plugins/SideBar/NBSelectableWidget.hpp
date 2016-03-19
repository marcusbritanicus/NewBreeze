/*
	*
	* NBSelectableWidget.hpp - SelectableWidget Class for SideBar Plugin Header
	*
	* The original file was a part of Quartica, released under GPL v3
	* Copyright (c) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
	*
	* Modified: Marcus Britanicus
	*
*/

#pragma once

// Qt4 Headers
#include <QWidget>

// Quartica Headers

class NBSelectableWidget : public QWidget {
	Q_OBJECT

	Q_PROPERTY( bool isSelected READ isSelected WRITE select )

	public:
		NBSelectableWidget( QWidget *parent = 0 );
		~NBSelectableWidget();

		bool isSelected( void ) const;

	signals:
		void selected( void );
		void selected( NBSelectableWidget *widget );

	public slots:
		void select( void );
		void select( bool isSelected );
		void unselect( void );

	protected:
		void mouseReleaseEvent( QMouseEvent *event );

	private:
		class Private;
		Private *d;
};
