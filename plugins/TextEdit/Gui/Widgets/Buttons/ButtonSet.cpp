/*
	*
	* ButtonSet.cpp - The Segmented Buttons Class
	*
*/

#include <ButtonSet.hpp>

ButtonSet::ButtonSet() : QWidget() {

	myButtonHeight = 24;
	myButtonWidth = 100;
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
};

ButtonSet::ButtonSet( QList<QIcon> icons ) : QWidget() {

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
		btn->setFixedWidth( myButtonWidth );
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
};

ButtonSet::ButtonSet( QStringList labels ) : QWidget() {

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
		btn->setFixedWidth( myButtonWidth );
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
};

ButtonSet::ButtonSet( QStringList labels, QList<QIcon> icons ) : QWidget() {

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
		btn->setFixedWidth( myButtonWidth );
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
};

QPushButton* ButtonSet::segment( int sgmnt ) {

	return segments.at( sgmnt );
};

void ButtonSet::setSegmentIcon( int segment, QIcon segIcon ) {

	segments.at( segment )->setIcon( segIcon );
};

void ButtonSet::setSegmentText( int segment, QString segText ) {

	segments.at( segment )->setText( segText );
};

void ButtonSet::setSegmentWidth( int width ) {

	Q_FOREACH( QPushButton *pBtn, segments )
		pBtn->setFixedWidth( width );

	setFixedWidth( segments.count() * width + segments.count() * 2 );
};

void ButtonSet::setSegmentHeight( int height ) {

	Q_FOREACH( QPushButton *pBtn, segments )
		pBtn->setFixedHeight( height );
};

void ButtonSet::setSegmentShortcut( int segment, QKeySequence shortcut ) {

	segments.at( segment )->setShortcut( shortcut );
};

void ButtonSet::setSegmentDisabled( int segment ) {

	segments.at( segment )->setDisabled( true );
};

void ButtonSet::setSegmentEnabled( int segment ) {

	segments.at( segment )->setEnabled( true );
};

void ButtonSet::insertSegment( QPushButton *button, int logicalPos ) {

	if ( logicalPos >= segments.count() ) {
		segments << button;
		btnsLyt->addWidget( Separator::vertical() );
		btnsLyt->addWidget( button );
		button->setFixedWidth( myButtonWidth );
		button->setFixedHeight( myButtonHeight );
	}

	else {
		connect( button, SIGNAL( clicked() ), this, SLOT( handleSegmentClick() ) );
		segments.insert( logicalPos, button );
		btnsLyt->insertWidget( logicalPos * 2, button );
		btnsLyt->insertWidget( logicalPos * 2 + 1, Separator::vertical() );
		button->setFixedWidth( myButtonWidth );
		button->setFixedHeight( myButtonHeight );
	}
};

void ButtonSet::addSegment( QPushButton *button ) {

	if ( segments.count() )
		btnsLyt->addWidget( Separator::vertical() );

	btnsLyt->addWidget( button );
	segments << button;
	button->setFixedWidth( myButtonWidth );
	button->setFixedHeight( myButtonHeight );
};

void ButtonSet::handleSegmentClick() {

	QPushButton *btn = qobject_cast<QPushButton*>( sender() );
	emit segmentClicked( segments.indexOf( btn ) );
};

// ===========================

ButtonSet2::ButtonSet2() : QWidget() {

	myButtonHeight = 24;
	myButtonWidth = 24;
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
};

ButtonSet2::ButtonSet2( QList<QIcon> icons ) : QWidget() {

	setContentsMargins( QMargins() );

	btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins() );
	btnsLyt->setSpacing( 0 );

	segments.clear();
	int max = icons.count();
	for( int i = 0; i < max; i++ ) {
		QToolButton *btn = new QToolButton( this );
		btn->setIcon( icons.value( i ) );

		btn->setFixedHeight( myButtonHeight );
		btn->setFixedWidth( myButtonWidth );
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
};

QToolButton* ButtonSet2::segment( int sgmnt ) {

	return segments.at( sgmnt );
};

void ButtonSet2::setSegmentIcon( int segment, QIcon segIcon ) {

	segments.at( segment )->setIcon( segIcon );
};

void ButtonSet2::setSegmentWidth( int width ) {

	Q_FOREACH( QToolButton *pBtn, segments )
		pBtn->setFixedWidth( width );

	setFixedWidth( segments.count() * width + segments.count() * 2 );
};

void ButtonSet2::setSegmentHeight( int height ) {

	Q_FOREACH( QToolButton *pBtn, segments )
		pBtn->setFixedHeight( height );
};

void ButtonSet2::setSegmentShortcut( int segment, QKeySequence shortcut ) {

	segments.at( segment )->setShortcut( shortcut );
};

void ButtonSet2::setSegmentDisabled( int segment ) {

	segments.at( segment )->setDisabled( true );
};

void ButtonSet2::setSegmentEnabled( int segment ) {

	segments.at( segment )->setEnabled( true );
};

void ButtonSet2::insertSegment( QToolButton *button, int logicalPos ) {

	if ( logicalPos >= segments.count() ) {
		segments << button;
		btnsLyt->addWidget( Separator::vertical() );
		btnsLyt->addWidget( button );
		button->setFixedWidth( myButtonWidth );
		button->setFixedHeight( myButtonHeight );
	}

	else {
		connect( button, SIGNAL( clicked() ), this, SLOT( handleSegmentClick() ) );
		segments.insert( logicalPos, button );
		btnsLyt->insertWidget( logicalPos * 2, button );
		btnsLyt->insertWidget( logicalPos * 2 + 1, Separator::vertical() );
		button->setFixedWidth( myButtonWidth );
		button->setFixedHeight( myButtonHeight );
	}
};

void ButtonSet2::addSegment( QToolButton *button ) {

	if ( segments.count() )
		btnsLyt->addWidget( Separator::vertical() );

	btnsLyt->addWidget( button );
	segments << button;
	button->setFixedWidth( myButtonWidth );
	button->setFixedHeight( myButtonHeight );
};

void ButtonSet2::handleSegmentClick() {

	QToolButton *btn = qobject_cast<QToolButton*>( sender() );
	emit segmentClicked( segments.indexOf( btn ) );
};
