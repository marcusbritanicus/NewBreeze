/*
	*
	* NBRenameDialog.cpp - NewBreeze Renamer Class
	*
*/

#include <NBRenameDialog.hpp>

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

	le->setFixedHeight( 24 );
	le->setText( name );
	le->selectAll();

	replaceCB->setText( "Replace E&xisting" );
	replaceCB->setChecked( false );

	okBtn = new NBButton( QIcon( ":/icons/ok.png" ), "&Rename", this );
	okBtn->setObjectName( "okBtn" );
	okBtn->setDisabled( true );

	cancelBtn = new NBButton( QIcon( ":/icons/cancel.png" ), "&Cancel", this );
	cancelBtn->setObjectName( "cancelBtn" );

	NBButtons *segBtns = new NBButtons( this );

	segBtns->addSegment( okBtn );
	segBtns->addSegment( cancelBtn );
	segBtns->setSegmentWidth( 90 );

	btnLyt->addWidget( replaceCB );
	btnLyt->addStretch( 0 );
	btnLyt->addWidget( segBtns );

	lyt->addWidget( lbl );
	lyt->addWidget( le );
	lyt->addLayout( btnLyt );

	connect( le, SIGNAL( textEdited( QString ) ), this, SLOT( handleTextChanged( QString ) ) );
	connect( le, SIGNAL( returnPressed() ), this, SLOT( rename() ) );
	connect( replaceCB, SIGNAL( stateChanged( int ) ), this, SLOT( handleCBStateChanged() ) );

	connect( okBtn, SIGNAL( clicked() ), this, SLOT( rename() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( cancel() ) );

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
		okBtn->setDisabled( true );
		return;
	}

	if ( newText == name )
		okBtn->setDisabled( true );

	else if ( replaceCB->isChecked() ) {
		if ( isDir( dir.filePath( newText ) ) )
			okBtn->setDisabled( true );

		else
			okBtn->setEnabled( true );
	}

	else {
		if ( dir.entryList().contains( newText ) )
			okBtn->setDisabled( true );

		else
			okBtn->setEnabled( true );
	}
};

void NBRenameDialog::handleCBStateChanged() {

	QString newText = le->text();

	if ( newText.isEmpty() ) {
		okBtn->setDisabled( true );
		return;
	}

	if ( newText == name )
		okBtn->setDisabled( true );

	else if ( replaceCB->isChecked() ) {
		if ( isDir( dir.filePath( newText ) ) )
			okBtn->setDisabled( true );

		else
			okBtn->setEnabled( true );
	}

	else {
		if ( dir.entryList().contains( newText ) )
			okBtn->setDisabled( true );

		else
			okBtn->setEnabled( true );
	}
};

void NBRenameDialog::rename() {

	if ( okBtn->isEnabled() ) {
		renameOk = true;
		close();
	}
};

void NBRenameDialog::cancel() {

	renameOk = false;
	close();
};
