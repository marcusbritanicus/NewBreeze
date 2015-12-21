/*
	*
	* NBButtons.cpp - The Segmented Buttons Class
	*
*/

#include <NBButtons.hpp>

NBButtons::NBButtons( QWidget *parent ) : QWidget( parent ) {

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
	baseLyt->setSpacing( 0 );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	/* Internal styling hack: until we learn to draw smooth rounded rects */
	setStyleSheet( "QWidget#btnsBase { border: 1px solid gray; border-radius: 4px; }" );
};

NBButtons::NBButtons( QList<QIcon> icons, QWidget *parent ) : QWidget( parent ) {

	setContentsMargins( QMargins() );

	btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins() );
	btnsLyt->setSpacing( 0 );

	segments.clear();
	int max = icons.count();
	for( int i = 0; i < max; i++ ) {
		NBButton *btn = new NBButton( icons.value( i ), this );
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
	baseLyt->setSpacing( 0 );
	baseLyt->setSpacing( 0 );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	/* Internal styling hack: until we learn to draw smooth rounded rects */
	setStyleSheet( "QWidget#btnsBase { border: 1px solid gray; border-radius: 4px; }" );
};

NBButtons::NBButtons( QStringList labels, QWidget *parent ) : QWidget( parent ) {

	setContentsMargins( QMargins() );

	btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins() );
	btnsLyt->setSpacing( 0 );

	segments.clear();
	int max = labels.count();
	for( int i = 0; i < max; i++ ) {
		NBButton *btn = new NBButton( labels.value( i ), this );
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
	baseLyt->setSpacing( 0 );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	/* Internal styling hack: until we learn to draw smooth rounded rects */
	setStyleSheet( "QWidget#btnsBase { border: 1px solid gray; border-radius: 4px; }" );
};

NBButtons::NBButtons( QStringList labels, QList<QIcon> icons, QWidget *parent ) : QWidget( parent ) {

	setContentsMargins( QMargins() );

	btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins() );
	btnsLyt->setSpacing( 0 );

	segments.clear();
	int max = qMax( labels.count(), icons.count() );
	for( int i = 0; i < max; i++ ) {
		NBButton *btn = new NBButton( icons.value( i ), labels.value( i ), this );
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
	baseLyt->setSpacing( 0 );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	/* Internal styling hack: until we learn to draw smooth rounded rects */
	setStyleSheet( "QWidget#btnsBase { border: 1px solid gray; border-radius: 4px; }" );
};

NBButton* NBButtons::segment( int sgmnt ) {

	return segments.at( sgmnt );
};

void NBButtons::setSegmentIcon( int segment, QIcon segIcon ) {

	segments.at( segment )->setIcon( segIcon );
};

void NBButtons::setSegmentText( int segment, QString segText ) {

	segments.at( segment )->setText( segText );
};

void NBButtons::setSegmentWidth( int width ) {

	Q_FOREACH( NBButton *pBtn, segments )
		pBtn->resize( width, 0 );

	/* Sum of the width of all the segments + sum of the width of the separators; */
	resize( segments.count() * width + ( segments.count() - 1 ) * 2, myButtonHeight );
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

void NBButtons::insertSegment( NBButton *button, int logicalPos ) {

	button->setParent( this );
	connect( button, SIGNAL( clicked() ), this, SLOT( handleSegmentClick() ) );

	if ( logicalPos >= segments.count() ) {
		segments << button;
		btnsLyt->addWidget( Separator::vertical() );
		btnsLyt->addWidget( button );
	}

	else {
		segments.insert( logicalPos, button );
		btnsLyt->insertWidget( logicalPos * 2, button );
		btnsLyt->insertWidget( logicalPos * 2 + 1, Separator::vertical() );
	}
};

void NBButtons::addSegment( NBButton *button ) {

	button->setParent( this );

	if ( segments.count() )
		btnsLyt->addWidget( Separator::vertical() );

	connect( button, SIGNAL( clicked() ), this, SLOT( handleSegmentClick() ) );

	btnsLyt->addWidget( button );
	segments << button;
};

void NBButtons::handleSegmentClick() {

	NBButton *btn = qobject_cast<NBButton*>( sender() );
	emit segmentClicked( segments.indexOf( btn ) );
};
