/*
	*
	* NBButtons.cpp - The Segmented Buttons Class
	*
*/

#include <NBButtons.hpp>

NBButtons::NBButtons() : QWidget() {

	myButtonHeight = 24;
	setContentsMargins( QMargins() );

	btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins() );
	btnsLyt->setSpacing( 0 );

	QWidget *btnsBase = new QWidget();
	btnsBase->setObjectName( "btnsBase" );
	btnsBase->setLayout( btnsLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	setStyleSheet( getStyleSheet( "NBButtons", Settings->General.Style ) );
};

NBButtons::NBButtons( QList<QIcon> icons ) : QWidget() {

	setContentsMargins( QMargins() );

	btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins() );
	btnsLyt->setSpacing( 0 );

	segments.clear();
	int max = icons.count();
	for( int i = 0; i < max; i++ ) {
		QPushButton *btn = new QPushButton( this );
		btn->setIcon( icons.value( i ) );

		btn->setFixedHeight( myButtonHeight );
		btn->setFocusPolicy( Qt::NoFocus );

		connect( btn, SIGNAL( clicked() ), this, SLOT( handleSegmentClick() ) );

		segments << btn;
		btnsLyt->addWidget( btn );
		if ( i < max - 1 )
			btnsLyt->addWidget( Separator::vertical() );
	}

	QWidget *btnsBase = new QWidget();
	btnsBase->setObjectName( "btnsBase" );
	btnsBase->setLayout( btnsLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	setStyleSheet( getStyleSheet( "NBButtons", Settings->General.Style ) );
};

NBButtons::NBButtons( QStringList labels ) : QWidget() {

	setContentsMargins( QMargins() );

	btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins() );
	btnsLyt->setSpacing( 0 );

	segments.clear();
	int max = labels.count();
	for( int i = 0; i < max; i++ ) {
		QPushButton *btn = new QPushButton( this );
		btn->setText( labels.value( i ) );

		btn->setFixedHeight( myButtonHeight );
		btn->setFocusPolicy( Qt::NoFocus );

		connect( btn, SIGNAL( clicked() ), this, SLOT( handleSegmentClick() ) );

		segments << btn;
		btnsLyt->addWidget( btn );
		if ( i < max - 1 )
			btnsLyt->addWidget( Separator::vertical() );
	}

	QWidget *btnsBase = new QWidget();
	btnsBase->setObjectName( "btnsBase" );
	btnsBase->setLayout( btnsLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	setStyleSheet( getStyleSheet( "NBButtons", Settings->General.Style ) );
};

NBButtons::NBButtons( QStringList labels, QList<QIcon> icons ) : QWidget() {

	setContentsMargins( QMargins() );

	btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins() );
	btnsLyt->setSpacing( 0 );

	segments.clear();
	int max = qMax( labels.count(), icons.count() );
	for( int i = 0; i < max; i++ ) {
		QPushButton *btn = new QPushButton( this );
		btn->setText( labels.value( i ) );
		btn->setIcon( icons.value( i ) );

		btn->setFixedHeight( myButtonHeight );
		btn->setFocusPolicy( Qt::NoFocus );

		connect( btn, SIGNAL( clicked() ), this, SLOT( handleSegmentClick() ) );

		segments << btn;
		btnsLyt->addWidget( btn );
		if ( i < max - 1 )
			btnsLyt->addWidget( Separator::vertical() );
	}

	QWidget *btnsBase = new QWidget();
	btnsBase->setObjectName( "btnsBase" );
	btnsBase->setLayout( btnsLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	setStyleSheet( getStyleSheet( "NBButtons", Settings->General.Style ) );
};

QPushButton* NBButtons::segment( int sgmnt ) {

	return segments.at( sgmnt );
};

void NBButtons::setSegmentIcon( int segment, QIcon segIcon ) {

	segments.at( segment )->setIcon( segIcon );
};

void NBButtons::setSegmentText( int segment, QString segText ) {

	segments.at( segment )->setText( segText );
};

void NBButtons::setSegmentWidth( int width ) {

	Q_FOREACH( QPushButton *pBtn, segments )
		pBtn->setFixedWidth( width );

	setFixedWidth( segments.count() * width + segments.count() * 2 );
};

void NBButtons::setSegmentHeight( int height ) {

	Q_FOREACH( QPushButton *pBtn, segments )
		pBtn->setFixedHeight( height );
};

void NBButtons::setSegmentShortcut( int segment, QKeySequence shortcut ) {

	segments.at( segment )->setShortcut( shortcut );
};

void NBButtons::setSegmentDisabled( int segment ) {

	segments.at( segment )->setDisabled( true );
};

void NBButtons::setSegmentEnabled( int segment ) {

	segments.at( segment )->setEnabled( true );
};

void NBButtons::insertSegment( QPushButton *button, int logicalPos ) {

	if ( logicalPos >= segments.count() ) {
		segments << button;
		btnsLyt->addWidget( Separator::vertical() );
		btnsLyt->addWidget( button );
	}

	else {
		connect( button, SIGNAL( clicked() ), this, SLOT( handleSegmentClick() ) );
		segments.insert( logicalPos, button );
		btnsLyt->insertWidget( logicalPos * 2, button );
		btnsLyt->insertWidget( logicalPos * 2 + 1, Separator::vertical() );
	}
};

void NBButtons::addSegment( QPushButton *button ) {

	if ( segments.count() )
		btnsLyt->addWidget( Separator::vertical() );
	btnsLyt->addWidget( button );
	segments << button;
};

void NBButtons::handleSegmentClick() {

	QPushButton *btn = qobject_cast<QPushButton*>( sender() );
	emit segmentClicked( segments.indexOf( btn ) );
};
