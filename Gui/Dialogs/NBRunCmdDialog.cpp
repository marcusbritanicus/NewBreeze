/*
	*
	* NBRunCmdDialog.cpp - NewBreeze RunCommandWidget Class
	*
*/

// #include <NBFolderView.hpp>
#include <NBFVDialogs.hpp>
#include <NBConfigParser.hpp>
#include <NBDesktopFile.hpp>
#include <NBTools.hpp>

NBRunCmdDialog::NBRunCmdDialog( QString origName ) : NBDialog() {

	fileName = QString( origName );

	createGUI();
	createAndSetupActions();

	setWindowProperties();
	runOk = false;
}

void NBRunCmdDialog::createGUI() {

	QVBoxLayout *lyt = new QVBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	QLabel *lbl1 = new QLabel( tr( "Open '%1' with: " ).arg( fileName ) );
	QLabel *lbl2 = new QLabel( tr( "Custom command:" ) );
	le = new QLineEdit();

	appList = new QListWidget();
	appList->setGridSize( QSize( 256, 32 ) );
	appList->setIconSize( QSize( 24, 24 ) );
	appList->setFlow( QListView::TopToBottom );
	appList->setMovement( QListView::Static );
	appList->setSelectionMode( QListView::SingleSelection );

	loadApplications();

	okBtn = new QPushButton();
	okBtn->setObjectName( "okBtn" );
	okBtn->setIcon( QIcon( ":/icons/ok.png" ) );
	okBtn->setText( "&Open" );
	okBtn->setAutoDefault( true );
	okBtn->setDisabled( true );

	cancelBtn = new QPushButton();
	cancelBtn->setObjectName( "cancelBtn" );
	cancelBtn->setIcon( QIcon( ":/icons/cancel.png" ) );
	cancelBtn->setText( "&Cancel" );

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( okBtn );
	btnLyt->addWidget( cancelBtn );

	lyt->addWidget( lbl1 );
	lyt->addWidget( appList );
	lyt->addWidget( lbl2 );
	lyt->addWidget( le );
	lyt->addLayout( btnLyt );

	QWidget *widget = new QWidget();
	widget->setObjectName( "guiBase" );
	widget->setContentsMargins( 0, 0, 0, 0 );
	widget->setLayout( lyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->addWidget( widget );

	setLayout( baseLyt );
	le->setFocus();
};

void NBRunCmdDialog::createAndSetupActions() {

	connect( appList, SIGNAL( itemClicked( QListWidgetItem* ) ), this, SLOT( appSelected( QListWidgetItem* ) ) );
	connect( appList, SIGNAL( itemActivated( QListWidgetItem* ) ), this, SLOT( appSelected( QListWidgetItem* ) ) );

	connect( le, SIGNAL( textEdited( QString ) ), this, SLOT( handleTextChanged( QString ) ) );
	connect( le, SIGNAL( returnPressed() ), this, SLOT( runCommand() ) );

	connect( okBtn, SIGNAL( clicked() ), this, SLOT( runCommand() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( cancel() ) );
};

void NBRunCmdDialog::setWindowProperties() {

	setWindowTitle( "NewBreeze - Run Command" );
	setWindowIcon( QIcon( ":/icons/openWith.png" ) );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );
	if ( not Settings->General.NativeTitleBar )
		setWindowFlags( Qt::FramelessWindowHint );

	setStyleSheet( getStyleSheet( "NBDialog", Settings->General.Style ) );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 480 ) / 2 );
	int vpos = ( int )( ( dw.height() - 640 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 480, 640 );
};

void NBRunCmdDialog::loadApplications() {

	QStringList paths = QStringList() << "/usr/share/applications" << QDir::home().filePath( ".local/share/applications/" );
	paths << "/usr/share/applications/kde4" << "/usr/share/gnome/applications/" << "/usr/local/share/applications/";

	QStringList addedExecList;
	QStringList addedNameList;

	QTime time = QTime::currentTime();
	time.start();

	foreach( QString path, paths ) {
		QDir desktopDir( path );
		desktopDir.setFilter( QDir::Files );
		desktopDir.setNameFilters( QStringList() << "*.desktop" );

		foreach( QString desktopEntry, desktopDir.entryList() ) {
			NBDesktopFile dFile( desktopDir.filePath( desktopEntry ) );
			if ( dFile.type != QString( "Application" ) )
				continue;

			// If we have added this executable once
			if ( addedExecList.contains( dFile.execArgs[ 0 ] ) and addedNameList.contains( dFile.name ) )
				continue;

			addedExecList << dFile.execArgs[ 0 ];
			addedNameList << dFile.name;

			QString name = dFile.name;
			QStringList exec = dFile.execArgs;
			QString icon = dFile.icon;

			QIcon progIcon = QIcon::fromTheme( icon, QIcon( icon ) );
			if ( progIcon.pixmap( QSize( 16, 16 ) ).isNull() )
				progIcon = QIcon( "/usr/share/pixmaps/" + icon + ".png" );

			if ( progIcon.pixmap( QSize( 16, 16 ) ).isNull() )
				progIcon = QIcon( "/usr/share/pixmaps/" + icon + ".xpm" );

			if ( progIcon.pixmap( QSize( 16, 16 ) ).isNull() )
				progIcon = QIcon( ":/icons/exec.png" );

			QListWidgetItem *item = new QListWidgetItem();
			item->setIcon( progIcon );
			item->setText( name );
			item->setData( Qt::UserRole, QVariant( dFile.exec ) );
			item->setData( Qt::UserRole + 10, QVariant( exec ) );

			appList->addItem( item );
		}
	}

	appList->sortItems();
};

void NBRunCmdDialog::handleTextChanged( QString newText ) {

	if ( newText.isEmpty() ) {
		okBtn->setDisabled( true );
		return;
	}

	QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
	foreach( QString path, paths ) {
		if ( QFileInfo( path + "/" + newText.split( " " )[ 0 ] ).exists() ) {
			okBtn->setEnabled( true );
			return;
		}
	}

	okBtn->setDisabled( true );
};

void NBRunCmdDialog::appSelected( QListWidgetItem* item ) {

	le->setText( item->data( Qt::UserRole ).toString() );
	okBtn->setEnabled( true );
};

void NBRunCmdDialog::runCommand() {

	runOk = true;
	close();
	le->setText( appList->currentItem()->data( Qt::UserRole + 10 ).toStringList().join( " " ) );
};

void NBRunCmdDialog::cancel() {

	runOk = false;
	close();
};
