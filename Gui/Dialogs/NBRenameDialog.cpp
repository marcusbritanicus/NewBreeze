/*
	*
	* NBRenamer.cpp - NewBreeze Renamer Class
	*
*/

#include <NBFVDialogs.hpp>
#include <NBTools.hpp>

NBRenameDialog::NBRenameDialog( QString origName, QDir curDir ) : NBDialog() {

	setDialogTitle( "Rename" );
	renameOk = false;

	QVBoxLayout *lyt = new QVBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	name = QString ( origName );
	dir = QDir( curDir );

	QLabel *lbl = new QLabel();
	le = new QLineEdit();
	replaceCB = new QCheckBox();
	lbl->setText( tr( "Rename the item '%1' to:" ).arg( origName ) );

	le->setText( origName );
	le->selectAll();

	replaceCB->setText( "Replace E&xisting" );
	replaceCB->setChecked( false );

	okBtn = new QPushButton( QIcon( ":/icons/ok.png" ), "&Rename" );
	okBtn->setObjectName( "okBtn" );
	okBtn->setAutoDefault( true );
	okBtn->setDisabled( true );

	cancelBtn = new QPushButton( QIcon( ":/icons/cancel.png" ), "&Cancel" );
	cancelBtn->setObjectName( "cancelBtn" );

	btnLyt->addWidget( replaceCB );
	btnLyt->addStretch( 0 );
	btnLyt->addWidget( okBtn );
	btnLyt->addWidget( cancelBtn );

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

	setFixedSize( 450, 150 );
};

void NBRenameDialog::handleTextChanged( QString newText ) {

	if ( newText.isEmpty() ) {
		okBtn->setDisabled( true );
		return;
	}

	if ( replaceCB->isChecked() )
		if ( newText == name )
			okBtn->setDisabled( true );

		else
			okBtn->setEnabled( true );

	else
		if ( ( newText == name ) or ( dir.entryList().contains( newText ) ) )
			okBtn->setDisabled( true );

		else
			okBtn->setEnabled( true );
}

void NBRenameDialog::handleCBStateChanged() {

	if ( le->text().isEmpty() )
		okBtn->setDisabled( true );

	else if ( replaceCB->isChecked() )
		okBtn->setEnabled( true );

	else {
		QString newText = le->text();
		if ( ( newText == name ) or ( dir.entryList().contains( newText ) ) )
			okBtn->setDisabled( true );

		else
			okBtn->setEnabled( true );
	}
}

void NBRenameDialog::rename() {

	renameOk = true;
	close();
}

void NBRenameDialog::cancel() {

	renameOk = false;
	close();
}
