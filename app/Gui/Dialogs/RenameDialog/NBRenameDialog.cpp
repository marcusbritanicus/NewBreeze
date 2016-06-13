/*
	*
	* NBRenameDialog.cpp - NewBreeze Renamer Class
	*
*/

#include "NBRenameDialog.hpp"

NBRenameDialog::NBRenameDialog( QString origName, QDir curDir, QWidget *parent ) : NBDialog( parent ) {

	name = QString ( origName );
	dir = QDir( curDir );

	createGUI();
}

void NBRenameDialog::createGUI() {

	setDialogTitle( "Rename" );
	setDialogIcon( QIcon( ":/icons/rename.png" ) );

	renameOk = false;

	QVBoxLayout *lyt = new QVBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	QLabel *lbl = new QLabel();
	le = new NBLineEdit( this );
	replaceCB = new QCheckBox();

	lbl->setText( tr( "Rename the item '%1' to:" ).arg( name ) );

	le->setText( name );
	le->selectAll();

	replaceCB->setText( "Replace E&xisting" );
	replaceCB->setChecked( false );

	segBtns = new NBSegmentButton( this );
	segBtns->setCount( 2 );

	segBtns->setSegmentIcon( 0, QIcon( ":/icons/ok.png" ) );
	segBtns->setSegmentText( 0, tr( "&Rename" ) );
	segBtns->segment( 0 )->setObjectName( "okBtn" );
	segBtns->setSegmentDisabled( 0 );

	segBtns->setSegmentIcon( 1, QIcon( ":/icons/cancel.png" ) );
	segBtns->setSegmentText( 1, tr( "&Cancel" ) );
	segBtns->segment( 1 )->setObjectName( "cancelBtn" );

	connect( segBtns, SIGNAL( clicked( int ) ), this, SLOT( handleSegmentClick( int ) ) );

	btnLyt->addWidget( replaceCB );
	btnLyt->addStretch( 0 );
	btnLyt->addWidget( segBtns );

	lyt->addWidget( lbl );
	lyt->addWidget( le );
	lyt->addLayout( btnLyt );

	connect( le, SIGNAL( textEdited( QString ) ), this, SLOT( handleTextChanged( QString ) ) );
	connect( le, SIGNAL( returnPressed() ), this, SLOT( rename() ) );
	connect( replaceCB, SIGNAL( stateChanged( int ) ), this, SLOT( handleCBStateChanged() ) );

	setLayout( lyt );
	le->setFocus();
};

QString NBRenameDialog::newName() {

	return le->text();
};

bool NBRenameDialog::canRename() {

	return renameOk;
};

void NBRenameDialog::handleTextChanged( QString newText ) {

	if ( newText.isEmpty() ) {
		segBtns->setSegmentDisabled( 0 );
		return;
	}

	if ( newText == name )
		segBtns->setSegmentDisabled( 0 );

	else if ( replaceCB->isChecked() ) {
		if ( isDir( dir.filePath( newText ) ) )
			segBtns->setSegmentDisabled( 0 );

		else
			segBtns->setSegmentEnabled( 0 );
	}

	else {
		if ( dir.entryList().contains( newText ) )
			segBtns->setSegmentDisabled( 0 );

		else
			segBtns->setSegmentEnabled( 0 );
	}
};

void NBRenameDialog::handleCBStateChanged() {

	QString newText = le->text();

	if ( newText.isEmpty() ) {
		segBtns->setSegmentDisabled( 0 );
		return;
	}

	if ( newText == name )
		segBtns->setSegmentDisabled( 0 );

	else if ( replaceCB->isChecked() ) {
		if ( isDir( dir.filePath( newText ) ) )
			segBtns->setSegmentDisabled( 0 );

		else
			segBtns->setSegmentEnabled( 0 );
	}

	else {
		if ( dir.entryList().contains( newText ) )
			segBtns->setSegmentDisabled( 0 );

		else
			segBtns->setSegmentEnabled( 0 );
	}
};

void NBRenameDialog::handleSegmentClick( int seg ) {

	switch( seg ) {
		case 0:
			return rename();

		case 1:
			return cancel();
	}
};

void NBRenameDialog::rename() {

	if ( segBtns->segment( 0 )->isEnabled() ) {
		renameOk = true;
		close();
	}
};

void NBRenameDialog::cancel() {

	renameOk = false;
	close();
};
