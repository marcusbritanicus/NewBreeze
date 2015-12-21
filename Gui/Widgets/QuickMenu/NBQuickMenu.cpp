/*
	*
	* NBQuickMenu.cpp - NewBreeze AddressBar Class
	*
*/

#include <NBQuickMenu.hpp>

NBQuickMenu::NBQuickMenu( QWidget *parent ) : QWidget( parent ) {

	setMouseTracking( true );
	setWindowFlags( Qt::FramelessWindowHint );

	QWidget *leftBase = new QWidget();
	leftBase->setFixedWidth( 252 );

	QWidget *rightBase = new QWidget();
	rightBase->setFixedWidth( 152 );

	QVBoxLayout *lLyt = new QVBoxLayout();
	lLyt->addWidget( new QLabel( "<h3>Documents</h3>" ) );
	lLyt->addStretch( 1 );
	lLyt->addWidget( new QLabel( "    /home/cosmos" ) );
	lLyt->addWidget( new QLabel( "    Directory" ) );
	lLyt->addWidget( new QLabel( "    13 items" ) );
	lLyt->addWidget( new QLabel( "    March 08, 2015" ) );
	lLyt->addStretch( 5 );

	QVBoxLayout *rLyt = new QVBoxLayout();
	rLyt->addWidget( new NBButton( QIcon::fromTheme( "folder" ), "&Open" ) );
	rLyt->addWidget( new NBButton( QIcon::fromTheme( "application-exit" ), "&Delete" ) );
	rLyt->addWidget( new NBButton( QIcon::fromTheme( "document-properties" ), "&Properties" ) );
	rLyt->addWidget( new NBButton( QIcon::fromTheme( "kgpg" ), "P&ermissions" ) );
	rLyt->addStretch();

	leftBase->setLayout( lLyt );
	rightBase->setLayout( rLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->addWidget( leftBase );
	baseLyt->addWidget( Separator::vertical() );
	baseLyt->addWidget( rightBase );

	setLayout( baseLyt );
};

void NBQuickMenu::keyPressEvent( QKeyEvent *kEvent ) {

	if ( isVisible() and kEvent->key() == Qt::Key_Escape ) {
		kEvent->accept();
		hide();

		return;
	}

	QWidget::keyPressEvent( kEvent );
};
