/*
	*
	* NBFileDialog.cpp - NewBreeze FileDialog Class
	*
*/

#include "NBFileDialog.hpp"
#include "NBNewNodeDialog.hpp"

NBFileDialog::NBFileDialog( QWidget *parent, QString wTitle, QString fLocation ) : QDialog( parent ) {

	setWindowTitle( wTitle );

	location = fLocation;

	createGUI();
	createAndSetupActions();
	setWindowProperties();
};

void NBFileDialog::createGUI() {

	sidePanel = new NBSidePanel( this );

	NBSegmentButton *dlgBtns = new NBSegmentButton( this );
	dlgBtns->setCount( 2 );

	dlgBtns->setSegmentIcon( 0, QIcon::fromTheme( "dialog-ok-apply", QIcon( ":/icons/ok.png" ) ) );
	dlgBtns->setSegmentText( 0, "&Okay" );
	dlgBtns->segment( 0 )->setObjectName( "okBtn" );
	dlgBtns->segment( 0 )->setFocusPolicy( Qt::NoFocus );
	connect( dlgBtns->segment( 0 ), SIGNAL( clicked() ), this, SLOT( accept() ) );

	dlgBtns->setSegmentIcon( 1, QIcon( ":/icons/delete.png" ) );
	dlgBtns->setSegmentText( 1, "&Cancel" );
	dlgBtns->segment( 1 )->setObjectName( "cancelBtn" );
	dlgBtns->segment( 1 )->setFocusPolicy( Qt::NoFocus );
	connect( dlgBtns->segment( 1 ), SIGNAL( clicked() ), this, SLOT( reject() ) );

	addressBar = new NBAddressWidget( this );

	fsModel = new NBItemViewModel( this );
	fsModel->setCategorizationEnabled( Settings->General.Grouping );
	fsModel->setRootPath( location );

	mainView = new NBIconView( fsModel, this );

	filtersCB = new QComboBox( this );

	NBSegmentButton *naviBtn = new NBSegmentButton( this );
	naviBtn->setCount( 3 );

	/* Back */
	naviBtn->setSegmentIcon( 0, QIcon::fromTheme( "arrow-left", QIcon( ":/icons/arrow-left.png" ) ) );
	naviBtn->segment( 0 )->setFocusPolicy( Qt::NoFocus );
	naviBtn->segment( 0 )->setShortcut( Settings->Shortcuts.GoLeft.at( 0 ) );
	connect( naviBtn->segment( 0 ), SIGNAL( clicked() ), fsModel, SLOT( goBack() ) );

	/* Home */
	naviBtn->setSegmentIcon( 1, QIcon::fromTheme( "go-home", QIcon( ":/icons/home.png" ) ) );
	naviBtn->segment( 1 )->setFocusPolicy( Qt::NoFocus );
	naviBtn->segment( 1 )->setShortcut( Settings->Shortcuts.GoHome.at( 0 ) );
	connect( naviBtn->segment( 1 ), SIGNAL( clicked() ), fsModel, SLOT( goHome() ) );

	/* Next */
	naviBtn->setSegmentIcon( 2, QIcon::fromTheme( "arrow-right", QIcon( ":/icons/arrow-right.png" ) ) );
	naviBtn->segment( 2 )->setFocusPolicy( Qt::NoFocus );
	naviBtn->segment( 2 )->setShortcut( Settings->Shortcuts.GoRight.at( 0 ) );
	connect( naviBtn->segment( 2 ), SIGNAL( clicked() ), fsModel, SLOT( goForward() ) );

	QHBoxLayout *addrLyt = new QHBoxLayout();
	addrLyt->addWidget( naviBtn );
	addrLyt->addWidget( addressBar );

	QHBoxLayout *bodyLyt = new QHBoxLayout();
	bodyLyt->addWidget( sidePanel );
	bodyLyt->addWidget( mainView );

	QHBoxLayout *fltrLyt = new QHBoxLayout();
	fltrLyt->setContentsMargins( QMargins( 0, 5, 0, 0 ) );
	fltrLyt->addWidget( new QLabel( "Filters:" ) );
	fltrLyt->addWidget( filtersCB );

	QHBoxLayout *btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins( 0, 5, 0, 0 ) );
	btnsLyt->addStretch( 0 );
	btnsLyt->addWidget( dlgBtns );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setSpacing( 0 );
	baseLyt->addLayout( addrLyt );
	baseLyt->addWidget( Separator::horizontal( this ) );
	baseLyt->addLayout( bodyLyt );
	baseLyt->addWidget( Separator::horizontal( this ) );
	baseLyt->addLayout( fltrLyt );
	baseLyt->addLayout( btnsLyt );

	setLayout( baseLyt );

	mainView->setFocus();
};

void NBFileDialog::createAndSetupActions() {

	connect( sidePanel, SIGNAL( driveClicked( QString ) ), this, SLOT( open( QString ) ) );

	connect( addressBar->crumbsBar, SIGNAL( openLocation( QString ) ), this, SLOT( open( QString ) ) );
	connect( addressBar->addressEdit, SIGNAL( returnPressed() ), this, SLOT( openAddressBar() ) );

	connect( mainView, SIGNAL( open( QModelIndex ) ), this, SLOT( open( QModelIndex ) ) );
	connect( mainView, SIGNAL( open( QString ) ), this, SLOT( open( QString ) ) );

	connect( fsModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( updateToolBar() ) );

	connect( filtersCB, SIGNAL( currentIndexChanged( int ) ), this, SLOT( resetFilters() ) );

	// Up
	QAction *actParDir = new QAction( QIcon( ":/icons/up.png" ), "&Up", this );
	actParDir->setShortcuts( Settings->Shortcuts.GoUp );

	connect( actParDir, SIGNAL( triggered() ), fsModel, SLOT( goUp() ) );
	addAction( actParDir );
};

void NBFileDialog::setNameFilters( QStringList filters, QString filter ) {

	int index = 0;
	if ( filters.contains( filter ) )
		index = filters.indexOf( filter );

	filtersCB->addItems( filters );
	filtersCB->setCurrentIndex( index );
};

QString NBFileDialog::selectedItem() {

	return fsModel->nodePath( mainView->selectionModel()->selectedRows().at( 0 ).data().toString() );
};

QStringList NBFileDialog::selectedItems() {

	QStringList selected;
	foreach( QModelIndex idx, mainView->selectionModel()->selectedRows() )
		selected << fsModel->nodePath( idx.data().toString() );

	return selected;
};

QString NBFileDialog::selectedFilter() {

	QString curFilter = filtersCB->currentText();
	QRegExp re( ".*\\(\\*([\\w.]*)\\)" );

	re.indexIn( curFilter );

	return re.cap( 1 );
};

void NBFileDialog::setWindowProperties() {

	setMinimumSize( 800, 600 );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 800 ) / 2 );
	int vpos = ( int )( ( dw.height() - 600 ) / 2 );
	move( hpos, vpos );

	resize( 800, 600 );
};

QString NBFileDialog::getFileName( QWidget *parent, QString title, QString location, QStringList filters, QString selectedFilter ) {

	NBFileDialog *fDlg = new NBFileDialog( parent, title, location );
	fDlg->setNameFilters( filters, selectedFilter );

	fDlg->exec();

	if ( fDlg->result() )
		return fDlg->selectedItem();

	else
		return QString();
};

// QString NBFileDialog::getSaveFileName( QWidget *parent, QString title, QString location, QStringList filters, QString selectedFilter ) {

	// NBFileDialog *fDlg = new NBFileDialog( icon, title, location, NBFileDialog::SaveFile, parent );
	// fDlg->setNameFilters( filters, selectedFilter );

	// fDlg->exec();

	// if ( fDlg->result() )
		// return fDlg->selectedItem();

	// else
		// return QString();
// };

void NBFileDialog::openAddressBar() {

	if ( !QFileInfo( addressBar->addressEdit->text() ).exists() ) {
		QString title = QString( "Invalid Location" );
		QString text = QString( "There is no file or directory named: "		\
			"<tt><b>%1</b></tt>. Please check the path entered."
		).arg(  addressBar->addressEdit->text() );

		NBMessageDialog::error( this, title, text );
		return;
	}

	fsModel->setRootPath( addressBar->addressEdit->text() );
	mainView->setFocus();
};

void NBFileDialog::open( QModelIndex idx ) {

	open( fsModel->nodePath( idx ) );
};

void NBFileDialog::open( QString path ) {

	if ( isDir( path ) )
		fsModel->setRootPath( path );

	else
		accept();

	mainView->setFocus();
};

void NBFileDialog::resetFilters() {

	QString filter = filtersCB->currentText();
	QRegExp rx( "[.]* [(](.*)[)]" );

	if ( rx.indexIn( filter ) != -1 ) {
		QStringList nFilters = rx.cap( 1 ).simplified().split( " " );
		fsModel->setNameFilters( nFilters );
	}
};

void NBFileDialog::updateToolBar() {

	QString url = fsModel->currentDir();

	addressBar->addressEdit->setText( url );
	addressBar->crumbsBar->setCurrentDirectory( url );
};

NBDirectoryDialog::NBDirectoryDialog( QWidget *parent, QString wTitle, QString fLocation ) : NBDialog( parent ) {

	setDialogTitle( wTitle );

	location = fLocation;

	createGUI();
	setWindowProperties();
};

void NBDirectoryDialog::createGUI() {

	sidePanel = new NBSidePanel( this );
	connect( sidePanel, SIGNAL( driveClicked( QString ) ), this, SLOT( open( QString ) ) );

	NBSegmentButton *dlgBtns = new NBSegmentButton( this );
	dlgBtns->setCount( 2 );

	dlgBtns->setSegmentIcon( 0, QIcon::fromTheme( "dialog-ok-apply", QIcon( ":/icons/ok.png" ) ) );
	dlgBtns->setSegmentText( 0, "&Okay" );
	dlgBtns->segment( 0 )->setObjectName( "okBtn" );
	dlgBtns->segment( 0 )->setFocusPolicy( Qt::NoFocus );
	connect( dlgBtns->segment( 0 ), SIGNAL( clicked() ), this, SLOT( accept() ) );

	dlgBtns->setSegmentIcon( 1, QIcon( ":/icons/delete.png" ) );
	dlgBtns->setSegmentText( 1, "&Cancel" );
	dlgBtns->segment( 1 )->setObjectName( "cancelBtn" );
	dlgBtns->segment( 1 )->setFocusPolicy( Qt::NoFocus );
	connect( dlgBtns->segment( 1 ), SIGNAL( clicked() ), this, SLOT( reject() ) );

	dirView = new NBDirectoryView( this );

	newFolderBtn = new QPushButton( QIcon::fromTheme( "folder-new" ), "New Folder", this );
	connect( newFolderBtn, SIGNAL( clicked() ), this, SLOT( addFolder() ) );

	QHBoxLayout *bodyLyt = new QHBoxLayout();
	bodyLyt->addWidget( sidePanel );
	bodyLyt->addWidget( dirView );

	QHBoxLayout *btnsLyt = new QHBoxLayout();
	btnsLyt->setContentsMargins( QMargins( 0, 5, 0, 0 ) );
	btnsLyt->addWidget( newFolderBtn );
	btnsLyt->addStretch();
	btnsLyt->addWidget( dlgBtns );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->setSpacing( 0 );
	baseLyt->addLayout( bodyLyt );
	baseLyt->addWidget( Separator::horizontal( this ) );
	baseLyt->addLayout( btnsLyt );

	setLayout( baseLyt );

	dirView->setCurrentBranch( location );
	dirView->setFocus();
};

QString NBDirectoryDialog::selectedItem() {

	return dirView->currentBranch();
};

void NBDirectoryDialog::setWindowProperties() {

	setMinimumSize( 800, 600 );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 800 ) / 2 );
	int vpos = ( int )( ( dw.height() - 600 ) / 2 );
	move( hpos, vpos );

	resize( 800, 600 );
};

QString NBDirectoryDialog::getDirectoryName( QWidget *parent, QString title, QString location ) {

	NBDirectoryDialog *fDlg = new NBDirectoryDialog( parent, title, location );
	fDlg->exec();

	if ( fDlg->result() )
		return fDlg->selectedItem();

	else
		return QString();
};

void NBDirectoryDialog::open( QString path ) {

	dirView->setCurrentBranch( path );
};

void NBDirectoryDialog::addFolder() {

	QString folder = dirView->currentBranch();

	NBNewNodeDialog *newFolder = new NBNewNodeDialog( "dir", QDir( folder ), QString(), this );
	if ( newFolder->exec() == QDialog::Accepted ) {
		dirView->setCurrentBranch( folder + "/" + newFolder->le->text() );
	}
};
