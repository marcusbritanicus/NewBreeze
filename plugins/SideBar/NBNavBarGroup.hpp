/*
	*
	* .hpp -  Class for SideBar Plugin
	*
	* The original file was a part of Quartica, released under GPL v3
	* Copyright (c) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
	*
	* Modified: Marcus Britanicus
	*
*/

#pragma once

#include <QWidget>

#include <NBNavBarGroupItem.hpp>

class NBNavBarGroup : public QWidget {
	Q_OBJECT

	Q_PROPERTY( QString title READ title WRITE setTitle )
	Q_PROPERTY( bool isExpanded READ isExpanded WRITE expand )

	public:
		NBNavBarGroup( QWidget *parent = 0 );
		NBNavBarGroup( const QString& title, QWidget *parent = 0 );
		~NBNavBarGroup();

		// Methods - Add Item
		void addItem( NBNavBarGroupItem *item );
		void addItem( NBNavBarGroupItem *item, int index );

		// Methods - Create and Add Item
		NBNavBarGroupItem *addItem( const QString& text );
		NBNavBarGroupItem *addItem( const QPixmap& icon, const QString& text );
		NBNavBarGroupItem *addItem( const QPixmap& icon, const QString& text, int index );

		// Methods
		bool containsItem( NBNavBarGroupItem *item );

		// GET Properties
		QString title( void ) const;
		bool isExpanded( void ) const;

		// SET Properties
		void setTitle( const QString& title );
		void setTitleColor( const QColor& color );

	signals:
		void selected( NBNavBarGroup *group, NBNavBarGroupItem *item );
		void expanded( NBNavBarGroup *group );

	public slots:
		void expand( bool expand );

	private slots:
		void onItemSelected( NBSelectableWidget *item );
		void onTitleClicked( void );

	private:
		class Private;
		Private *d;
};
