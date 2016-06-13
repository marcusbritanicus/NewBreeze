/*
	*
	* NBSegmentButton.cpp - The Segmented Buttons Class
	*
*/

#include "NBButtons.hpp"

NBSegmentButton::NBSegmentButton( QWidget *parent ) : QWidget( parent ) {

	btnGrp = new QButtonGroup();

	myButtonHeight = 28;
	myWidgetWidth = 28;
	btnGrp = new QButtonGroup();

	myButtonHeight = 28;
	myWidgetWidth = 28;
	setContentsMargins( QMargins() );

	btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins() );
	btnsLyt->setSpacing( 0 );

	QWidget *btnsBase = new QWidget( this );
	btnsBase->setObjectName( "base" );
	btnsBase->setLayout( btnsLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setSpacing( 0 );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	/* Internal styling hack: until we learn to draw smooth rounded rects */
	setStyleSheet( "QWidget#base { border: 1px solid lightgray; border-radius: 4px; }" );
};

NBSegmentButton::NBSegmentButton( QList<QIcon> icons, QWidget *parent ) : QWidget( parent ) {

	btnGrp = new QButtonGroup();

	myButtonHeight = 28;
	myWidgetWidth = 28;
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
		btnGrp->addButton( btn );
		btnsLyt->addWidget( btn );
	}

	QWidget *btnsBase = new QWidget( this );
	btnsBase->setObjectName( "base" );
	btnsBase->setLayout( btnsLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setSpacing( 0 );
	baseLyt->setSpacing( 0 );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	/* Internal styling hack: until we learn to draw smooth rounded rects */
	setStyleSheet( "QWidget#base { border: 1px solid lightgray; border-radius: 4px; }" );

	/* AutoResize */
	autoResize();
};

NBSegmentButton::NBSegmentButton( QStringList labels, QWidget *parent ) : QWidget( parent ) {

	btnGrp = new QButtonGroup();

	myButtonHeight = 28;
	myWidgetWidth = 28;
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
		btnGrp->addButton( btn );
		btnsLyt->addWidget( btn );
	}

	QWidget *btnsBase = new QWidget( this );
	btnsBase->setObjectName( "base" );
	btnsBase->setLayout( btnsLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setSpacing( 0 );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	/* Internal styling hack: until we learn to draw smooth rounded rects */
	setStyleSheet( "QWidget#base { border: 1px solid lightgray; border-radius: 4px; }" );

	/* AutoResize */
	autoResize();
};

NBSegmentButton::NBSegmentButton( QStringList labels, QList<QIcon> icons, QWidget *parent ) : QWidget( parent ) {

	btnGrp = new QButtonGroup();

	myButtonHeight = 28;
	myWidgetWidth = 28;
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
		btnGrp->addButton( btn );
		btnsLyt->addWidget( btn );
	}

	QWidget *btnsBase = new QWidget( this );
	btnsBase->setObjectName( "base" );
	btnsBase->setLayout( btnsLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setSpacing( 0 );
	baseLyt->setAlignment( Qt::AlignCenter );
	baseLyt->addWidget( btnsBase );

	setLayout( baseLyt );

	/* Internal styling hack: until we learn to draw smooth rounded rects */
	setStyleSheet( "QWidget#base { border: 1px solid lightgray; border-radius: 4px; }" );

	/* AutoResize */
	autoResize();
};

int NBSegmentButton::count() {

	return segments.count();
};

void NBSegmentButton::setCount( int count ) {

	if ( segments.count() > count ) {
		qWarning() << "You are attempting to remove some buttons, which is not possible";
		return;
	}

	else if ( segments.count() == count ) {
		qWarning() << "There are already" << count << "segments in this SegmentButton. What are you trying to do?";
		return;
	}

	else {
		for( int i = segments.count(); i < count; i++ )
			addSegment( new NBButton( this ) );

		resetStyleSheets();
	}

	/* AutoResize */
	autoResize();
};

NBButton* NBSegmentButton::segment( int sgmnt ) {

	return segments.at( sgmnt );
};

void NBSegmentButton::setSegmentIcon( int segment, QIcon segIcon ) {

	segments.at( segment )->setIcon( segIcon );

	/* AutoResize */
	autoResize();
};

void NBSegmentButton::setSegmentText( int segment, QString segText ) {

	segments.at( segment )->setText( segText );

	/* AutoResize */
	autoResize();
};

void NBSegmentButton::setSegmentWidth( int width ) {

	Q_FOREACH( NBButton *pBtn, segments )
		pBtn->resize( width, 0 );

	/* AutoResize */
	autoResize();
};

void NBSegmentButton::setSegmentShortcut( int segment, QKeySequence shortcut ) {

	segments.at( segment )->setShortcut( shortcut );
};

void NBSegmentButton::setSegmentDisabled( int segment ) {

	segments.at( segment )->setDisabled( true );
};

void NBSegmentButton::setSegmentEnabled( int segment ) {

	segments.at( segment )->setEnabled( true );
};

void NBSegmentButton::insertSegment( NBButton *button, int logicalPos ) {

	button->setParent( this );
	connect( button, SIGNAL( clicked() ), this, SLOT( handleSegmentClick() ) );

	if ( logicalPos >= segments.count() ) {
		segments << button;
		btnsLyt->addWidget( button );
	}

	else {
		segments.insert( logicalPos, button );
		btnsLyt->insertWidget( logicalPos * 2, button );
	}

	resetStyleSheets();

	/* AutoResize */
	autoResize();
};

void NBSegmentButton::addSegment( NBButton *button ) {

	button->setParent( this );

	connect( button, SIGNAL( clicked() ), this, SLOT( handleSegmentClick() ) );

	btnsLyt->addWidget( button );
	segments << button;
	btnGrp->addButton( button );

	resetStyleSheets();

	/* AutoResize */
	autoResize();
};

QSize NBSegmentButton::size() {

	return QSize( myWidgetWidth, myButtonHeight );
};

int NBSegmentButton::selectionBehavior() {

	return mSelectionBehavior;
};

void NBSegmentButton::setSelectionBehavior( SelectionBehavior behavior ) {

	mSelectionBehavior = behavior;

	switch( behavior ) {
		case NBSegmentButton::SelectOne: {
			btnGrp->setExclusive( true );
			Q_FOREACH( NBButton *btn, segments )
				btn->setCheckable( true );

			break;
		}

		case NBSegmentButton::SelectAll: {
			btnGrp->setExclusive( false );
			Q_FOREACH( NBButton *btn, segments )
				btn->setCheckable( true );

			break;
		}

		case NBSegmentButton::SelectNone: {
			btnGrp->setExclusive( false );
			Q_FOREACH( NBButton *btn, segments )
				btn->setCheckable( false );

			break;
		}
	}
};

QSize NBSegmentButton::sizeHint() {

	return QSize( myWidgetWidth, myButtonHeight );
};

void NBSegmentButton::resetStyleSheets() {

	if ( segments.count() == 1 ) {
		segments.at( 0 )->setSegment( NBButton::SingleButton );
		return;
	}

	for( int i = 0; i < segments.count(); i++ ) {
		if ( i == 0 )
			segments.at( i )->setSegment( NBButton::FirstSegment );

		else if ( i == segments.count() - 1 )
			segments.at( i )->setSegment( NBButton::LastSegment );

		else
			segments.at( i )->setSegment( NBButton::MiddleSegment );
	}
};

void NBSegmentButton::autoResize() {

	myWidgetWidth = 0;
	/* Sum of the width of all the segments */
	Q_FOREACH( NBButton *btn, segments )
		myWidgetWidth += btn->width();

	resize( myWidgetWidth, myButtonHeight );

	setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
};

void NBSegmentButton::handleSegmentClick() {

	NBButton *btn = qobject_cast<NBButton*>( sender() );
	emit clicked( segments.indexOf( btn ) );
};
