/*
	*
	* NBNavBarGroupItem.cpp - NavBarItem Class for SideBar Plugin
	*
	* The original file was a part of Quartica, released under GPL v3
	* Copyright( c ) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
	*
	* Modified: Marcus Britanicus
	*
*/

#include <QHBoxLayout>
#include <QLabel>

#include <NBNavBarGroupItem.hpp>

class NBNavBarGroupItem::Private {

	public:
		QHBoxLayout *layout;
		QLabel *labelIcon;
		QLabel *labelText;

	public:
		void initialize( NBNavBarGroupItem *item );
};

void NBNavBarGroupItem::Private::initialize( NBNavBarGroupItem *item ) {

	// Initialize Members
	layout = new QHBoxLayout;
	labelIcon = new QLabel;
	labelText = new QLabel;

	// Setup Text Label
	labelText->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
	labelIcon->setFixedWidth( 18 );

	// Setup Font
	QFont font = labelText->font();
	font.setPixelSize( 10 );
	labelText->setFont( font );

	// Add Items to Layout
	layout->addWidget( labelIcon );
	layout->addWidget( labelText );

	// Setup Layout
	layout->setContentsMargins( 1, 1, 1, 1 );
	item->setLayout( layout );
}

NBNavBarGroupItem::NBNavBarGroupItem( QWidget *parent ) : NBSelectableWidget( parent ), d( new NBNavBarGroupItem::Private ) {

	d->initialize( this );
}

NBNavBarGroupItem::NBNavBarGroupItem( const QString& text, QWidget *parent ) : NBSelectableWidget( parent ), d( new NBNavBarGroupItem::Private ) {

	d->initialize( this );
	d->labelText->setText( text );
}

NBNavBarGroupItem::NBNavBarGroupItem( const QPixmap& icon, const QString& text, QWidget *parent ) : NBSelectableWidget( parent ), d( new NBNavBarGroupItem::Private ) {

	d->initialize( this );

	setIcon( icon );
	d->labelText->setText( text );
}

NBNavBarGroupItem::~NBNavBarGroupItem() {

	delete d;
	d = NULL;
}

void NBNavBarGroupItem::addSpacing( int size ) {

	d->layout->addSpacing( size );
}

void NBNavBarGroupItem::addWidget( QWidget *widget, int stretch ) {

	d->layout->addWidget( widget, stretch );
}


void NBNavBarGroupItem::insertSpacing( int index, int size ) {

	d->layout->insertSpacing( index, size );
}

void NBNavBarGroupItem::insertWidget( int index, QWidget *widget, int stretch ) {

	d->layout->insertWidget( index, widget, stretch );
}

QString NBNavBarGroupItem::text( void ) const {

	return( d->labelText->text() );
}

const QPixmap *NBNavBarGroupItem::icon( void ) const {

	return( d->labelIcon->pixmap() );
}

void NBNavBarGroupItem::setIcon( const QPixmap& icon ) {

	if( icon.height() > 20 )
		d->labelIcon->setPixmap( icon.scaledToHeight( 20, Qt::SmoothTransformation ) );

	else
		d->labelIcon->setPixmap( icon );
}

void NBNavBarGroupItem::setText( const QString& text ) {

	d->labelText->setText( text );
}

void NBNavBarGroupItem::setTextColor( const QColor& color ) {

	QPalette palette = d->labelText->palette();
	palette.setColor( QPalette::WindowText, color );
	d->labelText->setPalette( palette );
}
