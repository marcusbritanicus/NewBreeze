/*
	*
	* NBNavBarGroup.cpp - NavBarGroup Class for SideBar Plugin
	*
	* The original file was a part of Quartica, released under GPL v3
	* Copyright (c) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
	*
	* Modified: Marcus Britanicus
	*
*/

#include <QVBoxLayout>
#include <QLabel>

#include <NBClickableLabel.hpp>
#include <NBNavBarGroup.hpp>

class NBNavBarGroup::Private {

	public:
		QList<NBNavBarGroupItem *> listItems;
		NBClickableLabel *labelTitle;
		QVBoxLayout *layout;
		bool isExpanded;

	public:
		void initialize( NBNavBarGroup *group );
};

void NBNavBarGroup::Private::initialize( NBNavBarGroup *group ) {

	// Initialize Members
	layout = new QVBoxLayout;
	labelTitle = new NBClickableLabel;

	// Setup Expanded Flag
	isExpanded = true;

	// Add Layout Items
	layout->addWidget( labelTitle );

	// Setup Label Title
	QFont font = labelTitle->font();
	font.setBold( true );
	font.setPixelSize( 10 );
	labelTitle->setFont( font );
	labelTitle->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );

	// Setup Layout
	layout->setSpacing( 1 );
	layout->setContentsMargins( 0, 0, 0, 0 );
	group->setLayout( layout );
}

NBNavBarGroup::NBNavBarGroup( QWidget *parent ) : QWidget( parent ), d( new NBNavBarGroup::Private ) {

	d->initialize( this );

	// Add Events
	connect( d->labelTitle, SIGNAL( clicked() ), this, SLOT( onTitleClicked() ) );
}

NBNavBarGroup::NBNavBarGroup( const QString& title, QWidget *parent ) : QWidget( parent ), d( new NBNavBarGroup::Private ) {

	d->initialize( this );
	d->labelTitle->setText( title );

	// Add Events
	connect( d->labelTitle, SIGNAL( clicked() ), this, SLOT( onTitleClicked() ) );
}

NBNavBarGroup::~NBNavBarGroup() {

	delete d;
	d = NULL;
}

void NBNavBarGroup::addItem( NBNavBarGroupItem *item ) {

	// Insert Item Space
	item->insertSpacing( 0, 10 );

	// Insert Item into Item List
	d->listItems.append( item );

	// Add Item to Layout
	d->layout->addWidget( item );

	// Add Event
	connect( item, SIGNAL( selected( NBSelectableWidget * ) ), this, SLOT( onItemSelected( NBSelectableWidget * ) ) );
}

void NBNavBarGroup::addItem( NBNavBarGroupItem *item, int index ) {

	// Insert Item Space
	item->insertSpacing( 0, 20 );

	// Insert Item into Item List
	d->listItems.insert( index, item );

	// Add Item to Layout
	d->layout->insertWidget( index, item );

	// Add Event
	connect( item, SIGNAL( selected( NBSelectableWidget * ) ), this, SLOT( onItemSelected( NBSelectableWidget * ) ) );
}

NBNavBarGroupItem *NBNavBarGroup::addItem( const QString& text ) {

	NBNavBarGroupItem *item = new NBNavBarGroupItem( text );
	addItem( item );
	return( item );
}

NBNavBarGroupItem *NBNavBarGroup::addItem( const QPixmap& icon, const QString& text ) {

	NBNavBarGroupItem *item = new NBNavBarGroupItem( icon, text );
	addItem( item );
	return( item );
}

NBNavBarGroupItem *NBNavBarGroup::addItem( const QPixmap& icon, const QString& text, int index ) {

	NBNavBarGroupItem *item = new NBNavBarGroupItem( icon, text );
	addItem( item, index );
	return( item );
}

bool NBNavBarGroup::containsItem( NBNavBarGroupItem *item ) {

	return( d->listItems.contains( item ) );
}

QString NBNavBarGroup::title( void ) const {

	return( d->labelTitle->text() );
}

bool NBNavBarGroup::isExpanded( void ) const {

	return( d->isExpanded );
}

void NBNavBarGroup::setTitle( const QString& title ) {

	d->labelTitle->setText( title );
}

void NBNavBarGroup::setTitleColor( const QColor& color ) {

	QPalette palette = d->labelTitle->palette();
	palette.setColor( QPalette::WindowText, color );
	d->labelTitle->setPalette( palette );
}

void NBNavBarGroup::expand( bool expand ) {

	if ( d->isExpanded == expand )
		return;

	if ( expand ) {
		Q_FOREACH( NBNavBarGroupItem *item, d->listItems ) {
			d->layout->addWidget( item );
			item->show();
		}
	}

	else {
		Q_FOREACH( NBNavBarGroupItem *item, d->listItems ) {
			d->layout->removeWidget( item );
			item->hide();
		}
	}

	d->isExpanded = expand;

	// Raise Expanded Event
	emit expanded( this );
}

void NBNavBarGroup::onItemSelected( NBSelectableWidget *item ) {

	emit selected( this,( NBNavBarGroupItem * ) item );
}

void NBNavBarGroup::onTitleClicked( void ) {

	expand( !d->isExpanded );
}
