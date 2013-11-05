/*
	*
	* NBRunCmdDialog.cpp - NewBreeze RunCommandWidget Class
	*
*/

#include <NBFVDialogs.hpp>
#include <NBAppEngine.hpp>
#include <NBAppFile.hpp>
#include <NBGuiWidgets.hpp>
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

	lyt->addWidget( Separator::horizontal() );
	lyt->addWidget( lbl1 );
	lyt->addWidget( appList );
	lyt->addWidget( lbl2 );
	lyt->addWidget( le );
	lyt->addLayout( btnLyt );

	setLayout( lyt );
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

	NBAppEngine *engine = NBAppEngine::instance();
	foreach( NBAppFile app, engine->allDesktops().toQList() ) {
		if ( app.value( NBAppFile::Type ).toString() != QString( "Application" ) )
			continue;

		// If we have added this executable once
		if ( addedExecList.contains( app.execArgs().at( 0 ) ) and addedNameList.contains( app.value( NBAppFile::Name ).toString() ) )
			continue;

		if ( not app.takesArgs() )
			continue;

		addedExecList << app.execArgs().at( 0 );
		addedNameList << app.value( NBAppFile::Name ).toString();

		QString name = app.value( NBAppFile::Name ).toString();
		QStringList exec = app.execArgs();
		QString icon = app.value( NBAppFile::Icon ).toString();

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
		item->setData( Qt::UserRole, app.value( NBAppFile::Exec ) );
		item->setData( Qt::UserRole + 10, QVariant( exec ) );

		appList->addItem( item );
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
