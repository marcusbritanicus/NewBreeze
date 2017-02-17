/*
	*
	* NBSideBarGroup.cpp - SideBarGruop class for NewBreeze
	*
*/

#include "NBSideBarGroup.hpp"

NBGroupHeader::NBGroupHeader( QString name, QString icon, QWidget *parent ) : QLabel( parent ) {

	mName = name;
	mIcon = icon;

	setText( QString( "<img src='%1' height = 13 width = 13> <b>%2</b>" ).arg( mIcon ).arg( mName ) );
	setContentsMargins( QMargins( 10, 0, 0, 0 ) );
	setCursor( Qt::PointingHandCursor );

	setFixedHeight( 20 );
};

void NBGroupHeader::mouseReleaseEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton )
		emit clicked();

	mEvent->accept();
};

NBSideBarGroup::NBSideBarGroup( QString name, QString icon, QWidget *parent ) : QWidget( parent ) {

	setObjectName( "SideBarGroup" );

	header = new NBGroupHeader( name, icon, this );

	itemsLayout = new QVBoxLayout();
	itemsLayout->setContentsMargins( QMargins() );
	itemsLayout->setSpacing( 0 );

	itemsBase = new QWidget( this );
	itemsBase->setLayout( itemsLayout );

	QVBoxLayout *baseLayout = new QVBoxLayout();

	baseLayout->setContentsMargins( QMargins() );
	baseLayout->setSpacing( 0 );

	baseLayout->addWidget( header );
	baseLayout->addWidget( itemsBase );

	setLayout( baseLayout );

	connect( header, SIGNAL( clicked() ), this, SLOT( showHideItems() ) );
};

int NBSideBarGroup::itemCount() {

	return itemList.count();
};

NBSideBarItem* NBSideBarGroup::item( int position ) {

	if ( ( position >= itemList.count() ) or ( position < 0 ) )
		return NULL;

	return itemList.at( position );
};

int NBSideBarGroup::addItem( QString name, QString icon, QString target ) {

	NBSideBarItem *itm = new NBSideBarItem( name, icon, target, this );
	connect( itm, SIGNAL( clicked( QString ) ), this, SIGNAL( clicked( QString ) ) );

	itemsLayout->addWidget( itm );
	itemList << itm;

	QWidget::resize( 0, 0 );
	updateGeometry();

	return itemList.indexOf( itm );
};

void NBSideBarGroup::clearHighlights() {

	Q_FOREACH( NBSideBarItem *itm, itemList )
		itm->setHighlighted( false );
};

void NBSideBarGroup::clear() {

	Q_FOREACH( NBSideBarItem *itm, itemList ) {
		itemsLayout->removeWidget( itm );
		itemList.removeAll( itm );
	}

	QWidget::resize( 0, 0 );
	updateGeometry();
};

void NBSideBarGroup::showHideItems() {

	if ( itemsBase->isVisible() )
		itemsBase->hide();

	else
		itemsBase->show();
};
