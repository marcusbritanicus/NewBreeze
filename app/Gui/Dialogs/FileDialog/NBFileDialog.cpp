/*
	*
	* NBFileDialog.cpp - NewBreeze FileDialog Class
	*
*/

#include "NBFileDialog.hpp"

NBFileDialog::NBFileDialog( QWidget *parent, QString wTitle, QString fLocation, DialogType dType ) : NBDialog( parent ) {

	setWindowTitle( wTitle );

	location = fLocation;
	type = dType;

	createGUI();
	createAndSetupActions();
	setWindowProperties();
};

void NBFileDialog::createGUI() {

	sidePanel = new NBSideBar( this );

	NBSegmentButton *dlgBtns = new NBSegmentButton( this );
	dlgBtns->setCount( 2 );

	dlgBtns->setSegmentIcon( 0, QIcon::fromTheme( "dialog-ok-apply", QIcon( ":/icons/ok.png" ) ) );
	dlgBtns->setSegmentText( 0, "&Okay" );
	dlgBtns->segment( 0 )->setObjectName( "okBtn" );
	connect( dlgBtns->segment( 0 ), SIGNAL( clicked() ), this, SLOT( accept() ) );

	dlgBtns->setSegmentIcon( 1, QIcon( ":/icons/delete.png" ) );
	dlgBtns->setSegmentText( 1, "&Cancel" );
	dlgBtns->segment( 0 )->setObjectName( "cancelkBtn" );
	connect( dlgBtns->segment( 1 ), SIGNAL( clicked() ), this, SLOT( reject() ) );

	switch( type ) {
		case Icon: {

			break;
		}

		case File: {

			addressBar = new NBAddressWidget( this );
			connect( addressBar->crumbsBar, SIGNAL( openLocation( QString ) ), this, SLOT( open( QString ) ) );
			connect( addressBar->addressEdit, SIGNAL( returnPressed() ), this, SLOT( openAddressBar() ) );

			fsModel = new NBItemViewModel( this );
			fsModel->setShowHidden( Settings->General.ShowHidden );
			fsModel->setCategorizationEnabled( Settings->General.Grouping );
			fsModel->setFilterFolders( false );
			connect( fsModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( updateToolBar() ) );
			fsModel->setRootPath( location );

			mainView = new NBIconView( fsModel, this );
			connect( mainView, SIGNAL( open( QModelIndex ) ), this, SLOT( open( QModelIndex ) ) );
			connect( mainView, SIGNAL( open( QString ) ), this, SLOT( doOpen( QString ) ) );

			nameLE = new NBLineEdit( this );

			filtersCB = new QComboBox( this );
			connect( filtersCB, SIGNAL( currentIndexChanged( int ) ), this, SLOT( resetFilters() ) );

			NBSegmentButton *naviBtn = new NBSegmentButton( this );
			naviBtn->setCount( 3 );

			/* Back */
			naviBtn->setSegmentIcon( 0, QIcon::fromTheme( "arrow-left", QIcon( ":/icons/arrow-left.png" ) ) );
			connect( naviBtn->segment( 0 ), SIGNAL( clicked() ), fsModel, SLOT( goBack() ) );

			/* Home */
			naviBtn->setSegmentIcon( 1, QIcon::fromTheme( "go-home", QIcon( ":/icons/home.png" ) ) );
			connect( naviBtn->segment( 1 ), SIGNAL( clicked() ), fsModel, SLOT( goHome() ) );

			/* Next */
			naviBtn->setSegmentIcon( 2, QIcon::fromTheme( "arrow-right", QIcon( ":/icons/arrow-right.png" ) ) );
			connect( naviBtn->segment( 2 ), SIGNAL( clicked() ), fsModel, SLOT( goForward() ) );

			QHBoxLayout *addrLyt = new QHBoxLayout();
			addrLyt->addWidget( naviBtn );
			addrLyt->addWidget( addressBar );

			QHBoxLayout *bodyLyt = new QHBoxLayout();
			bodyLyt->addWidget( sidePanel );
			bodyLyt->addWidget( mainView );

			QHBoxLayout *nameLyt = new QHBoxLayout();
			nameLyt->addWidget( new QLabel( "Name:" ) );
			nameLyt->addWidget( nameLE );

			QHBoxLayout *fltrLyt = new QHBoxLayout();
			fltrLyt->addWidget( new QLabel( "Filters:" ) );
			fltrLyt->addWidget( filtersCB );

			QHBoxLayout *btnLyt = new QHBoxLayout();
			btnLyt->addStretch( 0 );
			btnLyt->addWidget( dlgBtns );

			QVBoxLayout *baseLyt = new QVBoxLayout();
			baseLyt->setContentsMargins( QMargins() );
			baseLyt->setSpacing( 0 );
			baseLyt->addLayout( addrLyt );
			baseLyt->addWidget( Separator::horizontal( this ) );
			baseLyt->addLayout( bodyLyt );
			baseLyt->addWidget( Separator::horizontal( this ) );
			baseLyt->addLayout( nameLyt );
			baseLyt->addLayout( fltrLyt );
			baseLyt->addLayout( btnLyt );

			setLayout( baseLyt );

			mainView->setFocus();

			break;
		}

		case Directory: {

			dirView = new NBDirectoryView( this );
			nameLE = new NBLineEdit( this );

			QHBoxLayout *bodyLyt = new QHBoxLayout();
			bodyLyt->addWidget( sidePanel );
			bodyLyt->addWidget( dirView );

			QHBoxLayout *nameLyt = new QHBoxLayout();
			nameLyt->addWidget( new QLabel( "Name:" ) );
			nameLyt->addWidget( nameLE );

			QHBoxLayout *btnsLyt = new QHBoxLayout();
			btnsLyt->addWidget( dlgBtns );

			QVBoxLayout *baseLyt = new QVBoxLayout();
			baseLyt->setContentsMargins( QMargins() );
			baseLyt->setSpacing( 0 );
			baseLyt->addLayout( bodyLyt );
			baseLyt->addLayout( nameLyt );
			baseLyt->addLayout( btnsLyt );

			setLayout( baseLyt );

			dirView->setCurrentBranch( location );
			dirView->setFocus();

			break;
		}
	}
};

void NBFileDialog::createAndSetupActions() {

	connect( sidePanel, SIGNAL( driveClicked( QString ) ), this, SLOT( open( QString ) ) );

};

void NBFileDialog::setNameFilters( QStringList filters, QString filter ) {

	int index = 0;
	if ( filters.contains( filter ) )
		index = filters.indexOf( filter );

	filtersCB->addItems( filters );
	filtersCB->setCurrentIndex( index );

	resetFilters();
};

QString NBFileDialog::selectedItem() {

	switch( type ) {
		case File:
			return fsModel->nodePath( mainView->selectionModel()->selectedRows().at( 0 ).data().toString() );

		case Directory:
			return dirView->currentBranch();

		case Icon:
			return QString();
	}
};

QStringList NBFileDialog::selectedItems() {

	QString ext = selectedFilter();

	switch( type ) {
		case File: {
			QStringList selected;
			foreach( QModelIndex idx, mainView->selectionModel()->selectedRows() )
				selected << fsModel->nodePath( idx.data().toString() );

			return selected;
		}

		case Directory: {

			return QStringList() << dirView->currentBranch();
		}

		case Icon: {

			return QStringList();
		}
	}
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

	NBFileDialog *fDlg = new NBFileDialog( parent, title, location, NBFileDialog::File );
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

QString NBFileDialog::getDirectoryName( QWidget *parent, QString title, QString location ) {

	NBFileDialog *fDlg = new NBFileDialog( parent, title, location, NBFileDialog::Directory );
	fDlg->exec();

	if ( fDlg->result() )
		return fDlg->selectedItem();

	else
		return QString();
};

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

	fsModel->setRootPath( fsModel->nodePath( idx ) );
	mainView->setFocus();
};

void NBFileDialog::open( QString path ) {

	fsModel->setRootPath( path );
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
