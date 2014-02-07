/*
	*
	* NBAddressWidget.cpp - AddressBar for NewBreeze
	*
*/

#include <NBAddressBar.hpp>

NBAddressWidget::NBAddressWidget() : QWidget() {

	addressEdit = new QLineEdit();
	crumbsBar = new NBBreadCrumbsBar( this, "/" );
	crumbsBar->setAutoLoadNewPath( false );

	toggleBtn = new NBToolButton( "edit-rename", ":/icons/rename.png" );
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

	toggleBtn->setCheckable( true );
	toggleBtn->setChecked( false );
	toggleBtn->setFocusPolicy( Qt::NoFocus );
	toggleBtn->setShortcut( Settings->Shortcuts.ToggleCrumbLE.at( 0 ) );

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

NBAddressBar::NBAddressBar() : QFrame() {

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

	addressWidget = new NBAddressWidget();
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
	fLyt->addWidget( searchBar );
	fLyt->addWidget( addressButtons );

	setLayout( fLyt );
};
