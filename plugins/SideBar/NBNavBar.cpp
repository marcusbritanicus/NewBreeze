/*
	*
	* NBNavBar.cpp - Navigation Sidebar Class for SideBar Plugin
	*
	* The original file was a part of Quartica, released under GPL v3
	* Copyright (c) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
	*
	* Modified: Marcus Britanicus
	*
*/

#include <QPaintEvent>
#include <QVBoxLayout>
#include <QPainter>

#include <NBNavBar.hpp>

class NBNavBar::Private {

	public:
		QVBoxLayout *layout;

		// Colors
		QColor colorBackground;
		QColor colorSelection;

		// Current Selection
		NBNavBarGroup *groupSelected;
		NBNavBarGroupItem *itemSelected;
};

NBNavBar::NBNavBar( QWidget *parent ) : QWidget( parent ), d( new NBNavBar::Private ) {

	// Initialize Members
	d->layout = new QVBoxLayout;
	d->itemSelected = NULL;
	d->groupSelected = NULL;

	// Setup Colors
	d->colorBackground = QColor( 0xdf, 0xe4, 0xea );
	d->colorSelection = QColor( 0xa6, 0xa6, 0xa6 );

	// Setup Layout
	d->layout->addStretch( 2 );
	setLayout( d->layout );
}

NBNavBar::~NBNavBar() {

	delete d;
	d = NULL;
}

void NBNavBar::addGroup( NBNavBarGroup *group ) {

	// Set Font
	group->setTitleColor( QColor( 0x65, 0x71, 0x80 ) );

	// Add Events
	connect( group, SIGNAL( selected( NBNavBarGroup *, NBNavBarGroupItem * ) ), this, SLOT( onItemSelected( NBNavBarGroup *, NBNavBarGroupItem * ) ) );

	// Add to Layout
	d->layout->insertWidget( d->layout->count() - 1, group );
}

NBNavBarGroup *NBNavBar::addGroup( const QString& title ) {

	NBNavBarGroup *group = new NBNavBarGroup( title );
	addGroup( group );
	return( group );
}

bool NBNavBar::containsGroup( NBNavBarGroup *group ) {

	int itemCount = d->layout->count();

	for( int i = 0; i < itemCount; ++i ) {
		QWidget *widget = d->layout->itemAt( i )->widget();
		if( widget == group ) return( true );
	}

	return( false );
}

void NBNavBar::addItem( NBNavBarItem *item ) {

	d->layout->insertWidget( d->layout->count() - 1, item );
}

void NBNavBar::addItem( NBNavBarGroup *group, NBNavBarGroupItem *item ) {

	group->addItem( item );
}

NBNavBarGroupItem *NBNavBar::addItem( NBNavBarGroup *group, const QString& text ) {

	return( group->addItem( text ) );
}

NBNavBarGroupItem *NBNavBar::addItem( NBNavBarGroup *group, const QPixmap& icon, const QString& text ) {

	return( group->addItem( icon, text ) );
}


QColor NBNavBar::backgroundColor( void ) const {

	return( d->colorBackground );
}

QColor NBNavBar::selectionColor( void ) const {

	return( d->colorSelection );
}

void NBNavBar::setBackgroundColor( const QColor& bgColor ) {

	d->colorBackground = bgColor;
}

void NBNavBar::setSelectionColor( const QColor& selectionColor ) {

	d->colorSelection = selectionColor;
}

void NBNavBar::paintEvent( QPaintEvent *event ) {

	QWidget::paintEvent( event );

	/* Update if painting is not complete */
	if( event->rect().x() > 0 || event->rect().y() > 0 )
		update();

	QPainter p( this );

	p.setRenderHint( QPainter::Antialiasing );
	p.fillRect( event->rect(), d->colorBackground );

	/* Draw Selection */
	if( d->groupSelected != NULL && d->groupSelected->isExpanded() && d->itemSelected != NULL ) {
		QPoint pos = d->groupSelected->pos() + d->itemSelected->pos();
		int width = geometry().width();

		int r = d->colorSelection.red();
		int g = d->colorSelection.green();
		int b = d->colorSelection.blue();

		p.fillRect( 0, pos.y() - 1, width, 1, QColor( r - 0x26, g - 0x26, b - 0x26 ) );

		QLinearGradient linearGrad( QPointF( 0, pos.y() ), QPointF( 0, pos.y() + d->itemSelected->height() ) );
		linearGrad.setColorAt( 0, d->colorSelection );
		linearGrad.setColorAt( 1, QColor( r - 0x3b, g - 0x3b, b - 0x3b ) );
		p.fillRect( 0, pos.y(), width, d->itemSelected->height(), linearGrad );
	}

	p.end();
}

void NBNavBar::onItemSelected( NBNavBarGroup *group, NBNavBarGroupItem *item ) {

	/* If some other item is selected, then unselect the previous item */
	if( d->itemSelected != NULL && d->itemSelected != item ) {
		d->itemSelected->setFont( item->font() );
		d->itemSelected->setTextColor( Qt::black );
		d->itemSelected->unselect();
	}

	d->groupSelected = group;
	d->itemSelected = item;

	QFont font = d->itemSelected->font();
	font.setPixelSize( 9 );
	font.setBold( true );
	d->itemSelected->setFont( font );
	d->itemSelected->setTextColor( Qt::white );

	update();
}

void NBNavBar::onGroupExpanded( NBNavBarGroup *group ) {

	Q_UNUSED( group )

	update();
}
