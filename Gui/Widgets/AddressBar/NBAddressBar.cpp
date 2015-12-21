/*
	*
	* NBAddressWidget.cpp - AddressBar for NewBreeze
	*
*/

#include <NBAddressBar.hpp>

NBToggleButton::NBToggleButton( QWidget *parent ) : QWidget( parent ) {

	checked = false;
	setFixedSize( 24, 24 );
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

	painter.drawText( QRectF( 0, 0, 24, 24 ), Qt::AlignCenter, QString::fromUtf8( "\u270D " ) );

	painter.end();
	pEvent->accept();
};

NBAddressWidget::NBAddressWidget( QWidget *parent ) : QWidget( parent ) {

	addressEdit = new NBLineEdit( this );
	addressEdit->setFont( QFont( "Envy Code R", 11 ) );
	addressEdit->setStyleSheet( "border: none; background: transparent;" );
	addressEdit->setFocusPolicy( Qt::NoFocus );

	connect( addressEdit, SIGNAL( returnPressed() ), this, SLOT( revertToCrumbsBar() ) );

	crumbsBar = new NBCrumbsBar( this );
	connect( crumbsBar, SIGNAL( openLocation( QString ) ), this, SIGNAL( openLocation( QString ) ) );

	toggleBtn = new NBToggleButton( this );
	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( QMargins() );

	lyt->addWidget( crumbsBar );
	lyt->addWidget( addressEdit );
	lyt->addWidget( toggleBtn );

	addressEdit->hide();

	setLayout( lyt );

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

	setFixedHeight( 24 );

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
	setFixedHeight( 24 );

	QHBoxLayout *fLyt = new QHBoxLayout();
	fLyt->setContentsMargins( QMargins() );

	reloadBtn = new NBButton( QIcon( ":/icons/reload.png" ), this );

	viewModeBtn = new NBViewModeButton( this );

	openVTEBtn = new NBButton( QIcon( ":/icons/vte.png" ), this );

	addressWidget = new NBAddressWidget( this );
	searchBar = new NBSearchBar();
	addressButtons = new NBButtons( this );
	mProcWidget = new NBIOManagerMini( this );

	searchBar->setFixedHeight( 24 );

	addressButtons->addSegment( reloadBtn );
	addressButtons->addSegment( viewModeBtn );
	addressButtons->addSegment( openVTEBtn );

	addressButtons->setFixedHeight( 24 );

	openVTEBtn->setFocusPolicy( Qt::NoFocus );
	reloadBtn->setFocusPolicy( Qt::NoFocus );
	viewModeBtn->setFocusPolicy( Qt::NoFocus );

	addressWidget->addressEdit->setFocusPolicy( Qt::ClickFocus );
	addressWidget->crumbsBar->setFocusPolicy( Qt::NoFocus );

	reloadBtn->setToolTip( tr( "Reload view" ) );
	viewModeBtn->setToolTip( tr( "Switch to list mode" ) );
	openVTEBtn->setToolTip( tr( "Open a terminal emulator here" ) );

	fLyt->addWidget( addressWidget );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( searchBar );
	fLyt->addWidget( Separator::vertical() );
	fLyt->addWidget( addressButtons );
	if ( Settings->General.NativeTitleBar ) {
		fLyt->addWidget( Separator::vertical() );
		fLyt->addWidget( mProcWidget );
	}

	else
		mProcWidget->hide();

	setLayout( fLyt );

	connect( openVTEBtn, SIGNAL( clicked() ), this, SIGNAL( openTerminal() ) );
	connect( reloadBtn, SIGNAL( clicked() ), this, SIGNAL( reload() ) );
	connect( viewModeBtn, SIGNAL( switchToNextView() ), this, SIGNAL( switchToNextView() ) );
	connect( viewModeBtn, SIGNAL( changeViewMode() ), this, SIGNAL( changeViewMode() ) );
	connect( addressWidget, SIGNAL( openLocation( QString ) ), this, SIGNAL( openLocation( QString ) ) );
	connect( searchBar, SIGNAL( searchString( QString ) ), this, SIGNAL( search( QString ) ) );
	connect( searchBar, SIGNAL( searchCleared() ), this, SIGNAL( clearSearch() ) );
};

NBIOManagerMini *NBAddressBar::procWidget() {

	return mProcWidget;
};

QString NBAddressBar::address() {

	return addressWidget->addressEdit->text();
};

void NBAddressBar::setAddress( QString url ) {

	addressWidget->addressEdit->setText( url );
	addressWidget->crumbsBar->setCurrentDirectory( url );
};

int NBAddressBar::checkedAction() {

	return viewModeBtn->checkedAction();
};

void NBAddressBar::focusAddressEdit() {

	addressWidget->setFocus();
};

void NBAddressBar::focusSearchBar() {

	searchBar->searchLE->setFocus();
};

void NBAddressBar::clearSearchBar() {

	searchBar->searchLE->clear();
};
