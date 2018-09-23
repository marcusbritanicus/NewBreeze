/*
	*
	* NBDefaultsWidget.cpp - Properties of the selected object
	*
*/

#include "NBDefaultsWidget.hpp"
#include "NBGuiFunctions.hpp"

NBDefaultsWidget::NBDefaultsWidget( QStringList paths, QWidget *parent ) : QWidget( parent ) {

	QVBoxLayout *lyt = new QVBoxLayout();

	mimeType = mimeDb.mimeTypeForFile( paths.at( 0 ) );
	Q_FOREACH( QString path, paths ) {
		if ( mimeDb.mimeTypeForFile( path ) != mimeType ) {
			QLabel *lbl = new QLabel();
			lbl->setWordWrap( true );

			lbl->setAlignment( Qt::AlignCenter );
			lbl->setText(
				"You have chosen multiple types of files/folders. "
				"Please select a single file or folder to choose default applications."
			);

			lyt->addStretch();
			lyt->addWidget( lbl );
			lyt->addStretch();

			setLayout( lyt );

			return;
		}
	}

	QLabel *defLbl = new QLabel();
	defLbl->setWordWrap( true );
	defLbl->setText( QString( "Choose default application for <tt>%1</tt>" ).arg( mimeType.name() ) );

	defaultCB = new QComboBox();

	QLabel *otherLbl = new QLabel( "Other applications:" );
	appList = new QListWidget( this );
	appList->setSelectionMode( QAbstractItemView::SingleSelection );

	QPushButton *addBtn = new QPushButton( QIcon( ":/icons/list-add.png" ), "Add Application" );
	connect( addBtn, SIGNAL( clicked() ), this, SLOT( addApplication() ) );

	QPushButton *delBtn = new QPushButton( QIcon( ":/icons/list-remove.png" ), "Remove Application" );
	connect( delBtn, SIGNAL( clicked() ), this, SLOT( removeApplication() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addStretch();
	btnLyt->addWidget( addBtn );
	btnLyt->addWidget( delBtn );

	NBAppsList appsList = NBAppEngine::instance()->appsForMimeType( mimeType );
	Q_FOREACH( NBAppFile app, appsList.toQList() ) {
		QIcon appIcon = QIcon::fromTheme( app.value( NBAppFile::Icon ).toString(), QIcon( app.value( NBAppFile::Icon ).toString() ) );
		QString appName = app.value( NBAppFile::Name ).toString();

		defaultCB->addItem( appIcon, appName );

		QListWidgetItem *item = new QListWidgetItem( appIcon, appName, appList );
		item->setFlags( Qt::ItemIsEnabled );
		appList->addItem( item );
	}

	QSettings appsSett( "NewBreeze", "MimeApps" );
	QString mimeName = mimeType.name().replace( "/", "-" );
	Q_FOREACH( QString desktopFile, appsSett.value( mimeName ).toStringList() ) {
		NBAppFile appFile( desktopFile );
		if ( not appFile.isValid() )
			continue;

		QIcon appIcon = QIcon::fromTheme( appFile.value( NBAppFile::Icon ).toString(), QIcon( appFile.value( NBAppFile::Icon ).toString() ) );
		QString appName = appFile.value( NBAppFile::Name ).toString();

		defaultCB->addItem( appIcon, appName );

		QListWidgetItem *item = new QListWidgetItem( appIcon, appName, appList );
		item->setData( Qt::UserRole + 1, appFile.desktopFileName() );
		appList->addItem( item );
	}

	connect( defaultCB, SIGNAL( currentIndexChanged( int ) ), this, SLOT( makeDefault( int ) ) );

	lyt->addStretch();
	lyt->addWidget( defLbl );
	lyt->addWidget( defaultCB );
	lyt->addWidget( otherLbl );
	lyt->addWidget( appList );
	lyt->addLayout( btnLyt );
	lyt->addStretch();

	setLayout( lyt );
};

void NBDefaultsWidget::addApplication() {

	NBAppSelector *appSelDlg = new NBAppSelector( this );
	if ( appSelDlg->exec() != QDialog::Accepted )
		return;

	QString app = appSelDlg->selectedApp();
	NBAppFile appFile( app );
	if ( not appFile.isValid() )
		return;

	QIcon appIcon = ::icon( appFile.value( NBAppFile::Icon ).toStringList() );
	QString appName = appFile.value( NBAppFile::Name ).toString();

	defaultCB->addItem( appIcon, appName );

	QListWidgetItem *item = new QListWidgetItem( appIcon, appName, appList );
	item->setData( Qt::UserRole + 1, appFile.desktopFileName() );
	appList->addItem( item );

	QSettings appsSett( "NewBreeze", "MimeApps" );
	QStringList apps = appsSett.value( mimeType.name().replace( "/", "-" ) ).toStringList();
	apps << appFile.desktopFileName();
};

void NBDefaultsWidget::removeApplication() {

	int curIdx = appList->currentRow();
	QListWidgetItem *curItm = appList->currentItem();
	if ( not curItm )
		return;

	QString desktop = curItm->data( Qt::UserRole + 1 ).toString();
	if ( not desktop.count() )
		return;

	QSettings appsSett( "NewBreeze", "MimeApps" );
	QString key = mimeType.name().replace( "/", "-" );
	QStringList apps = appsSett.value( key ).toStringList();
	apps.removeAll( desktop );
	appsSett.setValue( key, apps );

	appsSett.sync();

	defaultCB->removeItem( curIdx );
	appList->takeItem( curIdx );
};

void NBDefaultsWidget::makeDefault( int idx ) {

	QListWidgetItem *itm = appList->item( idx );
	NBAppEngine::setApplicationAsDefault( itm->data( Qt::UserRole + 9 ).toString(), mimeType.name() );
};

NBAppSelector::NBAppSelector( QWidget *parent ) : NBDialog( parent ) {

	createGUI();
	setWindowProperties();
}

QString NBAppSelector::selectedApp() {

	return mSelectedApp;
};

void NBAppSelector::createGUI() {

	QVBoxLayout *lyt = new QVBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	QLabel *lbl1 = new QLabel( "Choose Application" );

	appList = new NBOpenWithView();
	connect( appList, SIGNAL( clicked( QModelIndex ) ), this, SLOT( appSelected( QModelIndex ) ) );
	connect( appList->selectionModel(), SIGNAL( currentChanged( QModelIndex, QModelIndex ) ), this, SLOT( appSelected( QModelIndex ) ) );

	okBtn = new NBButton( QIcon( ":/icons/ok.png" ), tr( "&Open" ), this );
	okBtn->setObjectName( "okBtn" );
	okBtn->setDisabled( true );
	connect( okBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );

	cancelBtn = new NBButton( QIcon( ":/icons/cancel.png" ), tr( "&Cancel" ), this );
	cancelBtn->setObjectName( "cancelBtn" );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	NBSegmentButton *segBtns = new NBSegmentButton( this );
	segBtns->addSegment( okBtn );
	segBtns->addSegment( cancelBtn );
	segBtns->setSegmentWidth( 90 );

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( segBtns );

	lyt->addWidget( lbl1 );
	lyt->addWidget( appList );
	lyt->addLayout( btnLyt );

	setLayout( lyt );
};

void NBAppSelector::setWindowProperties() {

	setDialogTitle( "NewBreeze - Choose Application" );
	setDialogIcon( QIcon( ":/icons/openWith.png" ) );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 480 ) / 2 );
	int vpos = ( int )( ( dw.height() - 640 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 480, 640 );
};

void NBAppSelector::appSelected( QModelIndex index ) {

	mSelectedApp = index.data( Qt::UserRole + 9 ).toString();
	okBtn->setEnabled( true );
};
