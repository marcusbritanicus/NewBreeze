/*
	*
	* NBAddressWidget.cpp - AddressBar for NewBreeze
	*
*/

#include "NBAddressBar.hpp"
#include "NBGuiFunctions.hpp"

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

NBAddressBar::NBAddressBar( QWidget *parent ) : QWidget( parent ) {

	setFixedHeight( 28 );

	QHBoxLayout *fLyt = new QHBoxLayout();
	fLyt->setContentsMargins( QMargins( 3, 0, 3, 0 ) );

	// Previous button

	backBtn = new NBButton( icon( NBIconManager::instance()->icon( "go-previous" ) ), this );
	backBtn->setFocusPolicy( Qt::NoFocus );
	backBtn->setFlat( true );

	// Next Button
	forwardBtn = new NBButton( icon( NBIconManager::instance()->icon( "go-next" ) ), this );
	forwardBtn->setFocusPolicy( Qt::NoFocus );
	forwardBtn->setFlat( true );

	// Home Button
	homeBtn = new NBButton( icon( NBIconManager::instance()->icon( "go-home" ) ), this );
	homeBtn->setFocusPolicy( Qt::NoFocus );
	homeBtn->setFlat( true );

	// SuperStart Button
	ssBtn = new NBButton( QIcon( ":/icons/superstart.png" ), this );
	ssBtn->setFocusPolicy( Qt::NoFocus );
	ssBtn->setFlat( true );

	NBSegmentButton *navBtns = new NBSegmentButton( this );
	navBtns->addSegment( backBtn );
	navBtns->addSegment( homeBtn );
	navBtns->addSegment( ssBtn );
	navBtns->addSegment( forwardBtn );

	// AddressWidget
	addressWidget = new NBAddressWidget( this );

	// FilterButton
	filterBtn = new NBButton( icon( NBIconManager::instance()->icon( "edit-find" ) ) );
	filterBtn->setObjectName( "base" );
	filterBtn->setFocusPolicy( Qt::NoFocus );

	// Process Widget
	mProcWidget = new NBProcessManagerMini( this );
	mProcWidget->setObjectName( "base" );

	// Menu Button
	menuBtn = new NBMenuButton( this );

	addressWidget->addressEdit->setFocusPolicy( Qt::ClickFocus );
	addressWidget->crumbsBar->setFocusPolicy( Qt::NoFocus );

	/* Layouts */
	fLyt->addWidget( navBtns );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( addressWidget );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( filterBtn );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( mProcWidget );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( menuBtn );

	QWidget *base = new QWidget( this );
	base->setObjectName( "baseWidget" );
	base->setStyleSheet( "#baseWidget{ border-bottom: 1px solid darkgray;}" );
	base->setLayout( fLyt );

	QHBoxLayout *bLyt = new QHBoxLayout();
	bLyt->setContentsMargins( QMargins() );
	bLyt->setSpacing( 0 );
	bLyt->addWidget( base );

	setLayout( bLyt );

	/* Connections */
	connect( backBtn, SIGNAL( clicked() ), this, SIGNAL( goBack() ) );
	connect( homeBtn, SIGNAL( clicked() ), this, SIGNAL( goHome() ) );
	connect( ssBtn, SIGNAL( clicked() ), this, SIGNAL( openSuperStart() ) );
	connect( forwardBtn, SIGNAL( clicked() ), this, SIGNAL( goForward() ) );
	connect( addressWidget, SIGNAL( openLocation( QString ) ), this, SIGNAL( openLocation( QString ) ) );
	connect( filterBtn, SIGNAL( clicked() ), this, SIGNAL( openSearch() ) );

	connect( menuBtn, SIGNAL( newWindow() ), this, SIGNAL( newWindow() ) );
	connect( menuBtn, SIGNAL( zoomIn() ), this, SIGNAL( zoomIn() ) );
	connect( menuBtn, SIGNAL( zoomOut() ), this, SIGNAL( zoomOut() ) );
	connect( menuBtn, SIGNAL( cut() ), this, SIGNAL( cut() ) );
	connect( menuBtn, SIGNAL( copy() ), this, SIGNAL( copy() ) );
	connect( menuBtn, SIGNAL( paste() ), this, SIGNAL( paste() ) );
	connect( menuBtn, SIGNAL( openVTE() ), this, SIGNAL( openVTE() ) );
	connect( menuBtn, SIGNAL( changeViewMode( int ) ), this, SIGNAL( changeViewMode( int ) ) );
	connect( menuBtn, SIGNAL( sortByName() ), this, SIGNAL( sortByName() ) );
	connect( menuBtn, SIGNAL( sortByType() ), this, SIGNAL( sortByType() ) );
	connect( menuBtn, SIGNAL( sortBySize() ), this, SIGNAL( sortBySize() ) );
	connect( menuBtn, SIGNAL( sortByDate() ), this, SIGNAL( sortByDate() ) );
	connect( menuBtn, SIGNAL( toggleHidden() ), this, SIGNAL( toggleHidden() ) );
	connect( menuBtn, SIGNAL( toggleGrouping() ), this, SIGNAL( toggleGrouping() ) );
	connect( menuBtn, SIGNAL( showSettings() ), this, SIGNAL( showSettings() ) );
	connect( menuBtn, SIGNAL( closeWindow() ), this, SIGNAL( closeWindow() ) );
	connect( menuBtn, SIGNAL( quit() ), this, SIGNAL( quit() ) );
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

	menuBtn->updateAddress( url );
};

void NBAddressBar::focusAddressEdit() {

	addressWidget->setFocus();
};

void NBAddressBar::hideSearchButton() {

	filterBtn->setIcon( QIcon() );
};

void NBAddressBar::showSearchButton() {

	filterBtn->setIcon( icon( NBIconManager::instance()->icon( "edit-find" ) ) );
};
