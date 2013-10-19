/*
	*
	* NBRunCmdDialog.cpp - NewBreeze NewFileFolderWidget Class
	*
*/

#include <iostream>
#include <cstdio>

#include <NBFVDialogs.hpp>
#include <NBTools.hpp>

using namespace std;

NBRunCmdDialog::NBRunCmdDialog( QString origName ) : NBDialog() {

	setWindowProperties();
	runOk = false;

	QVBoxLayout *lyt = new QVBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	QLabel *lbl = new QLabel( tr( "Open '%1' with command: " ).arg( origName ) );
	le = new QLineEdit();

	okBtn = new QPushButton();
	okBtn->setObjectName( tr( "okBtn" ) );
	okBtn->setIcon( QIcon( ":/icons/ok.png" ) );
	okBtn->setText( "&Open" );
	okBtn->setAutoDefault( true );
	okBtn->setDisabled( true );

	cancelBtn = new QPushButton();
	cancelBtn->setObjectName( tr( "cancelBtn" ) );
	cancelBtn->setIcon( QIcon( ":/icons/cancel.png" ) );
	cancelBtn->setText( "&Cancel" );

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( okBtn );
	btnLyt->addWidget( cancelBtn );

	lyt->addWidget( lbl );
	lyt->addWidget( le );
	lyt->addLayout( btnLyt );

	connect( le, SIGNAL( textEdited( QString ) ), this, SLOT( handleTextChanged( QString ) ) );
	connect( le, SIGNAL( returnPressed() ), this, SLOT( runCommand() ) );
	connect( okBtn, SIGNAL( clicked() ), this, SLOT( runCommand() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( cancel() ) );

	QWidget *widget = new QWidget();
	widget->setObjectName( "guiBase" );
	widget->setContentsMargins( 0, 0, 0, 0 );
	widget->setLayout( lyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->addWidget( widget );

	setLayout( baseLyt );
	le->setFocus();
};

void NBRunCmdDialog::setWindowProperties() {

	setWindowTitle( "NewBreeze - Run Command" );
	setWindowIcon( QIcon( ":/icons/openWith.png" ) );

	setAttribute( Qt::WA_TranslucentBackground );
	setWindowFlags( Qt::FramelessWindowHint );

	setStyleSheet( getStyleSheet( "NBDialog", Settings->General.Style ) );

	QDesktopWidget dw;
	int hpos = ( int )( ( dw.width() - 450 ) / 2 );
	int vpos = ( int )( ( dw.height() - 150 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 450, 150 );
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

void NBRunCmdDialog::runCommand() {

	runOk = true;
	close();
};

void NBRunCmdDialog::cancel() {

	runOk = false;
	close();
};
