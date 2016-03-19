/*
	*
	* NBNavBar.hpp - Navigation Sidebar Class for SideBar Plugin
	*
	* The original file was a part of Quartica, released under GPL v3
	* Copyright (c) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
	*
	* Modified: Marcus Britanicus
	*
*/

#pragma once

#include <QWidget>

#include <NBNavBarItem.hpp>
#include <NBNavBarGroup.hpp>

class NBNavBar : public QWidget {
	Q_OBJECT

	Q_PROPERTY( QColor selectionColor READ selectionColor WRITE setSelectionColor )
	Q_PROPERTY( QColor backgroundColor READ backgroundColor WRITE setBackgroundColor )

	public:
		NBNavBar( QWidget *parent = 0 );
		~NBNavBar( );

		// Methods - Groups Related
		void addGroup( NBNavBarGroup *group );
		NBNavBarGroup *addGroup( const QString& title );

		bool containsGroup( NBNavBarGroup *group );

		// Methods - Items Related
		void addItem( NBNavBarItem *item );
		void addItem( NBNavBarGroup *group, NBNavBarGroupItem *item );
		NBNavBarGroupItem *addItem( NBNavBarGroup *group, const QString& text );
		NBNavBarGroupItem *addItem( NBNavBarGroup *group, const QPixmap& icon, const QString& text );

		// GET Properties
		QColor selectionColor( void ) const;
		QColor backgroundColor( void ) const;

		// SET Properties
		void setBackgroundColor( const QColor& bgColor );
		void setSelectionColor( const QColor& selectionColor );

	protected:
		void paintEvent( QPaintEvent *event );

	private slots:
		void onGroupExpanded( NBNavBarGroup *group );
		void onItemSelected( NBNavBarGroup *group, NBNavBarGroupItem *item );

	private:
		class Private;
		Private *d;
};
