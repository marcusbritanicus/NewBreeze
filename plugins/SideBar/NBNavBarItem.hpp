/*
	*
	* NBNavBarItem.hpp - NavBarItem Class for SideBar Plugin Header
	*
	* The original file was a part of Quartica, released under GPL v3
	* Copyright( c ) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
	*
	* Modified: Marcus Britanicus
	*
*/

#pragma once

#include <QWidget>
#include <QMouseEvent>

#include <NBSelectableWidget.hpp>

class NBNavBarItem : public NBSelectableWidget {
	Q_OBJECT

	Q_PROPERTY( QString text READ text WRITE setText )
	Q_PROPERTY( QPixmap icon READ icon WRITE setIcon )

	public:
		NBNavBarItem( QWidget *parent = 0 );
		NBNavBarItem( const QString& text, QWidget *parent = 0 );
		NBNavBarItem( const QPixmap& icon, const QString& text, QWidget *parent = 0 );
		~NBNavBarItem();

		// Methods
		void addSpacing( int size );
		void addWidget( QWidget *widget, int stretch = 0 );

		void insertSpacing( int index, int size );
		void insertWidget( int index, QWidget *widget, int stretch = 0 );

		// GET Properties
		QString text( void ) const;
		const QPixmap *icon( void ) const;

		// SET Properties
		void setIcon( const QPixmap& icon );
		void setText( const QString& text );
		void setTextColor( const QColor& color );

	private:
		class Private;
		Private *d;
};
