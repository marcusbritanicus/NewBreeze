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

	addressEdit = new QLineEdit( this );
	crumbsBar = new NBBreadCrumbsBar( this, "/" );
	crumbsBar->setAutoLoadNewPath( false );

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

	setStyleSheet( getStyleSheet( "NBAddressBar", Settings->General.Style ) );
};

void NBAddressWidget::toggleCrumbsBarAndEdit() {

	if ( toggleBtn->isChecked() ) {
		crumbsBar->hide();
		addressEdit->show();
		if ( Settings->Session.ShowHidden )
			dModel->setFilter(  QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden );

		else
			dModel->setFilter(  QDir::Dirs | QDir::NoDotAndDotDot );
	}

	else {
		crumbsBar->show();
		addressEdit->hide();
	}
};

NBAddressBar::NBAddressBar( QWidget *parent ) : QFrame( parent ) {

	setFrameStyle( NoFrame );
	setFixedHeight( 24 );

	QHBoxLayout *fLyt = new QHBoxLayout();
	fLyt->setContentsMargins( QMargins() );

	reloadBtn = new  QPushButton();
	reloadBtn->setIcon( QIcon( ":/icons/reload.png" ) );
	reloadBtn->setFixedSize( QSize( 24, 24 ) );

	viewModeBtn = new NBViewModeButton();

	openVTEBtn = new QPushButton();
	openVTEBtn->setIcon( QIcon(  ":/icons/vte.png"  ) );
	openVTEBtn->setFixedSize( QSize( 24, 24 ) );

	addressWidget = new NBAddressWidget( this );
	searchBar = new NBSearchBar();
	addressButtons = new NBButtons();

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

	setLayout( fLyt );
};
