/*
	*
	* NBAddressWidget.cpp - AddressBar for NewBreeze
	*
*/

#include "NBAddressBar.hpp"

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

NBAddressEdit::NBAddressEdit( QWidget *parent ) : QLineEdit( parent ) {

	setStyleSheet( "background: transparent; border: none;" );
};

NBAddressWidget::NBAddressWidget( QWidget *parent ) : QWidget( parent ) {

	addressEdit = new NBAddressEdit( this );
	addressEdit->setFont( QFont( "DejaVu Sans Mono", 9 ) );
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
	base->setObjectName( "addressWidget" );
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
	setFont( QFont( "DejaVu Sans Mono", 9 ) );

	setFixedHeight( 28 );

	toggleBtn->setChecked( false );
	toggleBtn->setFocusPolicy( Qt::NoFocus );
	toggleBtn->setShortcut( Settings->Shortcuts.ToggleCrumbLE.at( 0 ).toString() );

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
	fLyt->setContentsMargins( QMargins( 3, 0, 0, 0 ) );

	// Previous button
	backBtn = new NBButton( QIcon( NBIconProvider::themeIcon( "go-previous", ":/icons/arrow-left.png" ) ), this );
	backBtn->setFocusPolicy( Qt::NoFocus );
	backBtn->setFlat( true );

	// Next Button
	forwardBtn = new NBButton( QIcon( NBIconProvider::themeIcon( "go-next", ":/icons/arrow-right.png" ) ), this );
	forwardBtn->setFocusPolicy( Qt::NoFocus );
	forwardBtn->setFlat( true );

	// Home Button
	homeBtn = new NBButton( QIcon( NBIconProvider::themeIcon( "go-home", ":/icons/home.png" ) ), this );
	homeBtn->setFocusPolicy( Qt::NoFocus );
	homeBtn->setFlat( true );

	NBSegmentButton *navBtns = new NBSegmentButton( this );
	navBtns->addSegment( backBtn );
	navBtns->addSegment( homeBtn );
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
	viewModes->setSegmentIcon( 0, QIcon( NBIconProvider::themeIcon( "view-list-details", ":/icons/view-list.png" ) ) );
	viewModes->segment( 0 )->setFocusPolicy( Qt::NoFocus );

	// Icons
	viewModes->setSegmentIcon( 1, QIcon( NBIconProvider::themeIcon( "view-list-icons", ":/icons/view-icon.png" ) ) );
	viewModes->segment( 1 )->setFocusPolicy( Qt::NoFocus );

	// Details
	viewModes->setSegmentIcon( 2, QIcon( NBIconProvider::themeIcon( "view-list-text", ":/icons/view-details.png" ) ) );
	viewModes->segment( 2 )->setFocusPolicy( Qt::NoFocus );

	if ( Settings->General.ViewMode == "Tiles" )
		viewModes->segment( 0 )->setChecked( true );

	else if ( Settings->General.ViewMode == "Icons" )
		viewModes->segment( 1 )->setChecked( true );

	else
		viewModes->segment( 2 )->setChecked( true );

	// FilterButton
	filterBtn = new NBButton( QIcon( NBIconProvider::themeIcon( "edit-finder", ":/icons/search.png" ) ) );
	filterBtn->setObjectName( "base" );
	filterBtn->setFocusPolicy( Qt::NoFocus );

	// Process Widget
	mProcWidget = new NBProcessManagerMini( this );
	mProcWidget->setObjectName( "base" );

	addressWidget->addressEdit->setFocusPolicy( Qt::ClickFocus );
	addressWidget->crumbsBar->setFocusPolicy( Qt::NoFocus );

	/* Layouts */
	fLyt->addWidget( navBtns );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( addressWidget );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( filterBtn );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( viewModes );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( mProcWidget );

	QWidget *base = new QWidget( this );
	base->setObjectName( "baseWidget" );
	base->setLayout( fLyt );

	QHBoxLayout *bLyt = new QHBoxLayout();
	bLyt->setContentsMargins( QMargins() );
	bLyt->setSpacing( 0 );
	bLyt->addWidget( base );

	setLayout( bLyt );

	/* Connections */
	connect( backBtn, SIGNAL( clicked() ), this, SIGNAL( goBack() ) );
	connect( homeBtn, SIGNAL( clicked() ), this, SIGNAL( goHome() ) );
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

	filterBtn->setIcon( QIcon( NBIconProvider::themeIcon( "edit-finder", ":/icons/search.png" ) ) );
};
