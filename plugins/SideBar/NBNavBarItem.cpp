/*
	*
	* NBNavBarItem.cpp - NavBarItem Class for SideBar Plugin
	*
	* The original file was a part of Quartica, released under GPL v3
	* Copyright( c ) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
	*
	* Modified: Marcus Britanicus
	*
*/

#include <QtCore>
#include <QtGui>

#include <NBNavBarItem.hpp>

class NBNavBarItem::Private {

	public:
		QHBoxLayout *layout;
		QLabel *labelIcon;
		QLabel *labelText;

	public:
		void initialize( NBNavBarItem *item );
};

void NBNavBarItem::Private::initialize( NBNavBarItem *item ) {

	// Initialize Members
	layout = new QHBoxLayout;
	labelIcon = new QLabel;
	labelText = new QLabel;

	// Setup Text Label
	labelText->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
	labelIcon->setFixedWidth( 18 );

	// Setup Font
	QFont font = labelText->font();
	font.setBold( true );
	font.setPixelSize( 10 );
	labelText->setFont( font );
	labelText->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );

	// Add Items to Layout
	layout->addWidget( labelIcon );
	layout->addWidget( labelText );

	// Setup Layout
	layout->setContentsMargins( 1, 1, 1, 1 );
	item->setLayout( layout );
}

NBNavBarItem::NBNavBarItem( QWidget *parent ) : NBSelectableWidget( parent ), d( new NBNavBarItem::Private ) {

	d->initialize( this );
}

NBNavBarItem::NBNavBarItem( const QString& text, QWidget *parent ) : NBSelectableWidget( parent ), d( new NBNavBarItem::Private ) {

	d->initialize( this );
	d->labelText->setText( text );
}

NBNavBarItem::NBNavBarItem( const QPixmap& icon, const QString& text, QWidget *parent ) : NBSelectableWidget( parent ), d( new NBNavBarItem::Private ) {

	d->initialize( this );

	setIcon( icon );
	d->labelText->setText( text );
}

NBNavBarItem::~NBNavBarItem() {

	delete d;
	d = NULL;
}

void NBNavBarItem::addSpacing( int size ) {

	d->layout->addSpacing( size );
}

void NBNavBarItem::addWidget( QWidget *widget, int stretch ) {

	d->layout->addWidget( widget, stretch );
}


void NBNavBarItem::insertSpacing( int index, int size ) {

	d->layout->insertSpacing( index, size );
}

void NBNavBarItem::insertWidget( int index, QWidget *widget, int stretch ) {

	d->layout->insertWidget( index, widget, stretch );
}

QString NBNavBarItem::text( void ) const {

	return( d->labelText->text() );
}

const QPixmap *NBNavBarItem::icon( void ) const {

	return( d->labelIcon->pixmap() );
}

void NBNavBarItem::setIcon( const QPixmap& icon ) {

	if ( icon.height() > 20 )
		d->labelIcon->setPixmap( icon.scaledToHeight( 20, Qt::SmoothTransformation ) );

	else
		d->labelIcon->setPixmap( icon );
}

void NBNavBarItem::setText( const QString& text ) {

	d->labelText->setText( text );
}

void NBNavBarItem::setTextColor( const QColor& color ) {

	QPalette palette = d->labelText->palette();
	palette.setColor( QPalette::WindowText, color );
	d->labelText->setPalette( palette );
}
