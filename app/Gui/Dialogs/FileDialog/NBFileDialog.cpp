/*
	*
	* NBFileDialog.cpp - NewBreeze FileDialog Class
	*
*/

#include "NBFileDialog.hpp"

NBFileDialog::NBFileDialog( QString wIcon, QString wTitle, QString fLocation, FileType dType, QWidget *parent ) : NBDialog( parent ) {

	icon = wIcon;
	title = wTitle;
	location = fLocation;
	type = dType;

	fsModel = new NBItemViewModel();
	fsModel->setShowHidden( Settings->General.ShowHidden );
	fsModel->setCategorizationEnabled( Settings->General.Grouping );
	fsModel->setFilterFolders( false );

	createGUI();
	createAndSetupActions();
	setWindowProperties();

	mainView->setFocus();
	fsModel->setRootPath( fLocation );
};

void NBFileDialog::createGUI() {

	QVBoxLayout *lyt = new QVBoxLayout();
	QVBoxLayout *viewLyt = new QVBoxLayout();
	QHBoxLayout *bodyLyt = new QHBoxLayout();
	QHBoxLayout *inputLyt = new QHBoxLayout();
	QHBoxLayout *filterLyt = new QHBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	addressWidget = new NBAddressWidget();
	sidePanel = new NBSidePanel( this );

	mainView = new NBIconViewRestricted( fsModel );
	switch( type ) {
		case ExistingFile :
		case SaveFile :
		case ExistingDirectory :
		case SaveDirectory : {
			mainView->setSelectionMode( QListView::SingleSelection );
			break;
		};
		case ExistingFiles :
		case ExistingDirectories : {
			mainView->setSelectionMode( QListView::ExtendedSelection );
			break;
		};
	};

	QLabel *nameLbl = new QLabel( "File&name:" );
	nameLE = new NBLineEdit( this );
	nameLE->setDisabled( true );
	nameLE->hide();
	nameLbl->setBuddy( nameLE );

	QLabel *filtersLbl = new QLabel( "&Filters:" );
	filtersCB = new QComboBox();

	filtersLbl->setBuddy( filtersCB );

	QPushButton *okBtn = new QPushButton( QIcon::fromTheme( "dialog-ok" ), "&Open" );
	okBtn->setObjectName( "okBtn" );
	okBtn->setFocusPolicy( Qt::NoFocus );

	QPushButton *cancelBtn = new QPushButton( QIcon::fromTheme( "dialog-close" ), "&Cancel" );
	cancelBtn->setObjectName( "cancelBtn" );
	cancelBtn->setFocusPolicy( Qt::NoFocus );

	connect( okBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	viewLyt->addWidget( addressWidget );
	viewLyt->addWidget( Separator::horizontal() );
	viewLyt->addWidget( mainView );

	bodyLyt->addWidget( sidePanel );
	bodyLyt->addWidget( Separator::vertical() );
	bodyLyt->addLayout( viewLyt );

	switch( type ) {
		case SaveFile :
		case SaveDirectory : {
			inputLyt->addWidget( nameLbl );
			inputLyt->addWidget( nameLE );
			break;
		};
		case ExistingFile :
		case ExistingDirectory :
		case ExistingFiles :
		case ExistingDirectories :
			break;
	};


	filterLyt->addWidget( filtersLbl );
	filterLyt->addWidget( filtersCB );

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( okBtn );
	btnLyt->addWidget( cancelBtn );

	lyt->addLayout( bodyLyt );
	lyt->addWidget( Separator::horizontal() );
	if( ( type == SaveFile ) or ( type == SaveDirectory ) )
		lyt->addLayout( inputLyt );
	lyt->addLayout( filterLyt );
	lyt->addLayout( btnLyt );

	setLayout( lyt );

	// Update the toolbar
	updateToolBar();
};

void NBFileDialog::createAndSetupActions() {

	connect( sidePanel, SIGNAL( driveClicked( QString ) ), this, SLOT( open( QString ) ) );

	connect( addressWidget->crumbsBar, SIGNAL( openLocation( QString ) ), this, SLOT( open( QString ) ) );
	connect( addressWidget->addressEdit, SIGNAL( returnPressed() ), this, SLOT( openAddressBar() ) );

	connect( fsModel, SIGNAL( directoryLoaded( QString ) ), this, SLOT( updateToolBar() ) );

	connect( mainView, SIGNAL( open( QModelIndex ) ), this, SLOT( open( QModelIndex ) ) );

	connect( filtersCB, SIGNAL( currentIndexChanged( int ) ), this, SLOT( resetFilters() ) );
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

	QString ext = selectedFilter();

	if ( nameLE->text().isEmpty() ) {
		QString item = fsModel->nodePath( mainView->selectionModel()->selectedRows().at( 0 ).data().toString() );
		if ( not item.endsWith( ext ) )
			return item += ext;

		else
			return item;
	}

	else {
		QString item = fsModel->nodePath( nameLE->text() );
		if ( not item.endsWith( ext ) )
			return item += ext;

		else
			return item;
	}
};

QStringList NBFileDialog::selectedItems() {

	QString ext = selectedFilter();

	if ( nameLE->text().isEmpty() ) {
		QStringList selected;
		foreach( QModelIndex idx, mainView->selectionModel()->selectedRows() ) {
			QString item = fsModel->nodePath( idx.data().toString() );
			if ( not item.endsWith( ext ) )
				selected << item += ext;

			else
				selected << item;
		}

		return selected;
	}

	else {
		QString item = fsModel->nodePath( nameLE->text() );
		if ( not item.endsWith( ext ) )
			return QStringList() << item += ext;

		else
			return QStringList() << item;
	}
};

QString NBFileDialog::selectedFilter() {

	QString curFilter = filtersCB->currentText();
	QRegExp re( ".*\\(\\*([\\w.]*)\\)" );

	re.indexIn( curFilter );

	return re.cap( 1 );
};

void NBFileDialog::setWindowProperties() {

	setDialogTitle( title );
	setDialogIcon( QIcon::fromTheme( icon, QIcon( icon ) ) );

	setMinimumSize( 800, 600 );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 800 ) / 2 );
	int vpos = ( int )( ( dw.height() - 600 ) / 2 );
	move( hpos, vpos );

	resize( 800, 600 );
};

QString NBFileDialog::getSaveFileName( QString icon, QString title, QString location, QStringList filters, QString selectedFilter ) {

	NBFileDialog *fDlg = new NBFileDialog( icon, title, location, NBFileDialog::SaveFile );
	fDlg->setNameFilters( filters, selectedFilter );

	fDlg->exec();

	if ( fDlg->result() )
		return fDlg->selectedItem();

	else
		return QString();
};

QString NBFileDialog::getExistingFileName( QString icon, QString title, QString location, QStringList filters, QString selectedFilter ) {

	NBFileDialog *fDlg = new NBFileDialog( icon, title, location, NBFileDialog::ExistingFile );
	fDlg->setNameFilters( filters, selectedFilter );

	fDlg->exec();

	if ( fDlg->result() )
		return fDlg->selectedItem();

	else
		return QString();
};

void NBFileDialog::openAddressBar() {

	if ( !QFileInfo( addressWidget->addressEdit->text() ).exists() ) {
		QString title = QString( "Invalid Location" );
		QString text = QString( "There is no file or directory named: "		\
			"<tt><b>%1</b></tt>. Please check the path entered."
		).arg(  addressWidget->addressEdit->text() );

		NBMessageDialog::error( this, title, text );
		return;
	}

	fsModel->setRootPath( addressWidget->addressEdit->text() );
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

	addressWidget->addressEdit->setText( url );
	addressWidget->crumbsBar->setCurrentDirectory( url );
};