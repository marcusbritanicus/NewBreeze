/*
	*
	* NBAddressWidget.cpp - AddressBar for NewBreeze
	*
*/

#include <NBAddressBar.hpp>

NBToggleButton::NBToggleButton( QWidget *parent ) : QWidget( parent ) {

	checked = false;
	setFixedSize( 28, 28 );
};

bool NBToggleButton::isChecked() {

	return checked;
};

void NBToggleButton::setChecked( bool truth ) {

	checked = truth;
	repaint();
};

void NBToggleButton::setShortcut( QString kSeq ) {

	shortcut = new QAction( this );
	shortcut->setShortcut( kSeq );
	connect( shortcut, SIGNAL( triggered() ), this, SLOT( toggle() ) );
	connect( shortcut, SIGNAL( triggered() ), this, SIGNAL( clicked() ) );

	addAction( shortcut );
};

void NBToggleButton::toggle() {

	if ( checked )
		checked = false;

	else
		checked = true;

	repaint();
};

void NBToggleButton::mousePressEvent( QMouseEvent *mEvent ) {

	toggle();

	emit clicked();
	mEvent->accept();
};

void NBToggleButton::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );

	painter.setFont( QFont( font().family(), 14 ) );

	if ( palette().color( QPalette::Text ) == Qt::white ) {
		if ( checked )
			painter.setPen( Qt::lightGray );

		else
			painter.setPen( QColor( 60, 60, 60 ) );
	}

	else {
		if ( checked )
			painter.setPen( QColor( 60, 60, 60 ) );

		else
			painter.setPen( Qt::lightGray );
	}

	painter.drawText( QRectF( 0, 0, 28, 28 ), Qt::AlignCenter, QString::fromUtf8( "\u270D " ) );

	painter.end();
	pEvent->accept();
};

NBAddressWidget::NBAddressWidget( QWidget *parent ) : QWidget( parent ) {

	addressEdit = new NBLineEdit( this );
	addressEdit->setFont( QFont( "Envy Code R", 11 ) );
	addressEdit->setFocusPolicy( Qt::NoFocus );

	connect( addressEdit, SIGNAL( returnPressed() ), this, SLOT( revertToCrumbsBar() ) );

	crumbsBar = new NBCrumbsBar( this );
	connect( crumbsBar, SIGNAL( openLocation( QString ) ), this, SIGNAL( openLocation( QString ) ) );

	toggleBtn = new NBToggleButton( this );

	QHBoxLayout *bLyt = new QHBoxLayout();
	bLyt->setContentsMargins( QMargins() );
	bLyt->setSpacing( 0 );

	bLyt->addWidget( crumbsBar );
	bLyt->addWidget( addressEdit );
	bLyt->addWidget( toggleBtn );

	QWidget *base = new QWidget( this );
	base->setObjectName( "base" );
	base->setLayout( bLyt );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( QMargins() );
	lyt->setSpacing( 0 );

	lyt->addWidget( base );
	setLayout( lyt );

	addressEdit->hide();

	dModel = new QDirModel();
	dModel->setFilter( QDir::Dirs | QDir::NoDotAndDotDot );

	QCompleter *completer = new QCompleter();
	completer->setModel( dModel );
	addressEdit->setCompleter( completer );

	setWidgetProperties();
};

void NBAddressWidget::setFocus() {

	if ( not toggleBtn->isChecked() ) {
		toggleBtn->setChecked( true );
		crumbsBar->hide();
		addressEdit->show();
		addressEdit->setFocus();
	}
};

void NBAddressWidget::setShowHidden( bool shown ) {

	if ( shown )
		dModel->setFilter( QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden );

	else
		dModel->setFilter( QDir::Dirs | QDir::NoDotAndDotDot );
};

void NBAddressWidget::setWidgetProperties() {

	setContentsMargins( QMargins() );
	setFont( QFont( "Envy Code R", 10 ) );

	setFixedHeight( 28 );

	toggleBtn->setChecked( false );
	toggleBtn->setFocusPolicy( Qt::NoFocus );
	toggleBtn->setShortcut( Settings->Shortcuts.ToggleCrumbLE.at( 0 ) );

	connect( toggleBtn, SIGNAL( clicked() ), this, SLOT( toggleCrumbsBarAndEdit() ) );
};

void NBAddressWidget::toggleCrumbsBarAndEdit() {

	if ( toggleBtn->isChecked() ) {
		crumbsBar->hide();
		addressEdit->show();
		if ( Settings->General.ShowHidden )
			dModel->setFilter(  QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden );

		else
			dModel->setFilter(  QDir::Dirs | QDir::NoDotAndDotDot );
	}

	else {
		crumbsBar->show();
		addressEdit->hide();
	}
};

void NBAddressWidget::revertToCrumbsBar() {

	if ( toggleBtn->isChecked() ) {
		toggleBtn->setChecked( false );
		crumbsBar->show();
		addressEdit->hide();

		emit openLocation( addressEdit->text() );
	}
};

NBAddressBar::NBAddressBar( QWidget *parent ) : QFrame( parent ) {

	setFrameStyle( NoFrame );
	setFixedHeight( 28 );

	QHBoxLayout *fLyt = new QHBoxLayout();
	fLyt->setContentsMargins( QMargins() );

	// Previous button
	backBtn = new NBButton( QIcon::fromTheme( "go-previous" ), this );
	backBtn->setFocusPolicy( Qt::NoFocus );
	backBtn->setFlat( true );

	// Next Button
	forwardBtn = new NBButton( QIcon::fromTheme( "go-next" ), this );
	forwardBtn->setFocusPolicy( Qt::NoFocus );
	forwardBtn->setFlat( true );

	NBSegmentButton *navBtns = new NBSegmentButton( this );
	navBtns->addSegment( backBtn );
	navBtns->addSegment( forwardBtn );

	// AddressWidget
	addressWidget = new NBAddressWidget( this );

	// View Modes
	viewModes = new NBSegmentButton( this );
	viewModes->setObjectName( "base" );

	viewModes->setFocusPolicy( Qt::NoFocus );
	viewModes->setCount( 3 );
	viewModes->setSelectionBehavior( NBSegmentButton::SelectOne );

	// Tiles
	viewModes->setSegmentIcon( 0, QIcon::fromTheme( "view-list-details" ) );
	viewModes->segment( 0 )->setFocusPolicy( Qt::NoFocus );

	// Icons
	viewModes->setSegmentIcon( 1, QIcon::fromTheme( "view-list-icons" ) );
	viewModes->segment( 1 )->setFocusPolicy( Qt::NoFocus );

	// Details
	viewModes->setSegmentIcon( 2, QIcon::fromTheme( "view-list-text" ) );
	viewModes->segment( 2 )->setFocusPolicy( Qt::NoFocus );

	if ( Settings->General.ViewMode == "Tiles" )
		viewModes->segment( 0 )->setChecked( true );

	else if ( Settings->General.ViewMode == "Tiles" )
		viewModes->segment( 1 )->setChecked( true );

	else
		viewModes->segment( 1 )->setChecked( true );

	// FilterButton
	filterBtn = new NBButton( QIcon::fromTheme( "edit-find", QIcon( ":/icons/search.png" ) ) );
	filterBtn->setObjectName( "base" );
	filterBtn->setFocusPolicy( Qt::NoFocus );

	// Process Widget
	mProcWidget = new NBProcessManagerMini( this );
	mProcWidget->setObjectName( "base" );

	addressWidget->addressEdit->setFocusPolicy( Qt::ClickFocus );
	addressWidget->crumbsBar->setFocusPolicy( Qt::NoFocus );

	fLyt->addWidget( navBtns );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( addressWidget );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( filterBtn );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( viewModes );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( mProcWidget );

	setLayout( fLyt );

	connect( backBtn, SIGNAL( clicked() ), this, SIGNAL( goBack() ) );
	connect( forwardBtn, SIGNAL( clicked() ), this, SIGNAL( goForward() ) );
	connect( addressWidget, SIGNAL( openLocation( QString ) ), this, SIGNAL( openLocation( QString ) ) );
	connect( filterBtn, SIGNAL( clicked() ), this, SIGNAL( openSearch() ) );
	connect( viewModes, SIGNAL( clicked( int ) ), this, SIGNAL( changeViewMode( int ) ) );
};

void NBAddressBar::updateNavigationButtons( bool bBtn, bool fBtn ) {

	backBtn->setEnabled( bBtn );
	forwardBtn->setEnabled( fBtn );
};

QString NBAddressBar::address() {

	return addressWidget->addressEdit->text();
};

void NBAddressBar::setAddress( QString url ) {

	addressWidget->addressEdit->setText( url );
	addressWidget->crumbsBar->setCurrentDirectory( url );
};

int NBAddressBar::checkedAction() {

	return 1;
};

void NBAddressBar::focusAddressEdit() {

	addressWidget->setFocus();
};

void NBAddressBar::hideSearchButton() {

	filterBtn->setIcon( QIcon() );
};

void NBAddressBar::showSearchButton() {

	filterBtn->setIcon( QIcon::fromTheme( "edit-find", QIcon( ":/icons/search.png" ) ) );
};
