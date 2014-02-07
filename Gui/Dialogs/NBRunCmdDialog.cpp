/*
	*
	* NBRunCmdDialog.cpp - NewBreeze 'Open file with' Class
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

QString NBRunCmdDialog::commandString() {

	return execCmd;
};

bool NBRunCmdDialog::canRun() {

	return runOk;
};

void NBRunCmdDialog::createGUI() {

	QVBoxLayout *lyt = new QVBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	QLabel *lbl1 = new QLabel( tr( "Open '<tt><b>%1</b></tt>' with: " ).arg( fileName ) );
	QLabel *lbl2 = new QLabel( tr( "Custom command:" ) );
	le = new QLineEdit();

	appList = new NBOpenWithView();

	okBtn = new QPushButton( QIcon( ":/icons/ok.png" ), tr( "&Open" ) );
	okBtn->setObjectName( "okBtn" );
	okBtn->setAutoDefault( true );
	okBtn->setDisabled( true );

	defaultAndRunBtn = new QPushButton( QIcon( ":/icons/ok.png" ), tr( "&Make default and open" ) );
	defaultAndRunBtn->setObjectName( "okBtn" );
	defaultAndRunBtn->setAutoDefault( false );
	defaultAndRunBtn->setDisabled( true );

	cancelBtn = new QPushButton( QIcon( ":/icons/cancel.png" ), tr( "&Cancel" ) );
	cancelBtn->setObjectName( "cancelBtn" );

	btnLyt->addWidget( defaultAndRunBtn );
	btnLyt->addStretch( 0 );
	btnLyt->addWidget( okBtn );
	btnLyt->addWidget( cancelBtn );

	lyt->addWidget( lbl1 );
	lyt->addWidget( appList );
	lyt->addWidget( lbl2 );
	lyt->addWidget( le );
	lyt->addLayout( btnLyt );

	setLayout( lyt );
	le->setFocus();
};

void NBRunCmdDialog::createAndSetupActions() {

	connect( appList, SIGNAL( clicked( QModelIndex ) ), this, SLOT( appSelected( QModelIndex ) ) );
	connect( appList, SIGNAL( doubleClicked( QModelIndex ) ), this, SLOT( runCommand( QModelIndex ) ) );
	connect( appList, SIGNAL( activated( QModelIndex ) ), this, SLOT( runCommand( QModelIndex ) ) );

	connect( appList->selectionModel(), SIGNAL( currentChanged( QModelIndex, QModelIndex ) ), this, SLOT( appSelected( QModelIndex ) ) );

	connect( le, SIGNAL( textEdited( QString ) ), this, SLOT( handleTextChanged( QString ) ) );
	connect( le, SIGNAL( returnPressed() ), this, SLOT( runCommand() ) );

	connect( defaultAndRunBtn, SIGNAL( clicked() ), this, SLOT( makeDefaultAndRunCommand() ) );

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

void NBRunCmdDialog::handleTextChanged( QString newText ) {

	if ( newText.isEmpty() ) {
		okBtn->setDisabled( true );
		return;
	}

	QStringList paths = QString( getenv( "PATH" ) ).split( ":" );
	foreach( QString path, paths ) {
		if ( QFileInfo( path + "/" + newText.split( " " )[ 0 ] ).exists() ) {
			execCmd = newText;
			okBtn->setEnabled( true );
			defaultAndRunBtn->setEnabled( true );
			return;
		}
	}

	okBtn->setDisabled( true );
};

void NBRunCmdDialog::appSelected( QModelIndex index ) {

	le->setText( index.data( NBApplicationsModel::ExecStr ).toString() );
	execCmd = index.data( NBApplicationsModel::ExecList ).toStringList().join( " " );
	okBtn->setEnabled( true );
	defaultAndRunBtn->setEnabled( true );
};

void NBRunCmdDialog::runCommand() {

	runOk = true;
	close();
};

void NBRunCmdDialog::runCommand( QModelIndex index ) {

	execCmd = index.data( NBApplicationsModel::ExecList ).toStringList().join( " " );
	runOk = true;
	close();
};

void NBRunCmdDialog::cancel() {

	runOk = false;
	close();
};

void NBRunCmdDialog::makeDefaultAndRunCommand() {

	QModelIndex idx = appList->currentIndex();

	NBAppEngine::setApplicationAsDefault( idx.data( NBApplicationsModel::DesktopFile ).toString(), getMimeType( fileName ) );
	execCmd = idx.data( NBApplicationsModel::ExecList ).toStringList().join( " " );
	runOk = true;
	close();
};
