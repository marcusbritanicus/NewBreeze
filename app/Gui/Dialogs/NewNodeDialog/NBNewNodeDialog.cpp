/*
	*
	* NBNewNodeDialog.hpp - Dialog for creating a new file/folder for NewBreeze
	*
*/

#include "NBNewNodeDialog.hpp"

NBNewNodeDialog::NBNewNodeDialog( QString type, QDir curDir, QString text, QWidget *parent ) : NBDialog( parent ) {

	if ( type == QString( "dir" ) )
		setDialogTitle( "New Folder" );

	else if ( type == QString( "file" ) )
		setDialogTitle( "New File" );

	else
		setDialogTitle( "Paste Into File" );

	QVBoxLayout *lyt = new QVBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	dir = QDir( curDir );
	dir.makeAbsolute();

	if ( !text.isNull() )
		data = QString( text );

	QLabel *lbl = new QLabel();
	le = new NBLineEdit( this );

	okBtn = new NBButton( QIcon( ":/icons/ok.png" ), "&Ok", this );
	okBtn->setObjectName( tr( "okBtn" ) );

	cancelBtn = new NBButton( QIcon( ":/icons/cancel.png" ), "&Cancel", this );
	cancelBtn->setObjectName( tr( "cancelBtn" ) );

	NBSegmentButton *segBtns = new NBSegmentButton( this );
	segBtns->addSegment( okBtn );
	segBtns->addSegment( cancelBtn );
	segBtns->setSegmentWidth( 90 );

	if ( type == QString( "dir" ) ) {
		lbl->setText( tr( "Create new folder in:\n%1" ).arg( curDir.path() ) );
		le->setText( "New Folder" );
	}
	else if ( type == QString( "file" ) ) {
		lbl->setText( tr( "Create new file in:\n%1" ).arg( curDir.path() ) );
		le->setText( "New File" );
	}
	else {
		lbl->setText( tr( "Paste clipboard contents in:" ) );
		le->setText( "New File" );
	}

	le->selectAll();

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( segBtns );

	lyt->addWidget( lbl );
	lyt->addWidget( le );
	lyt->addWidget( Separator::horizontal() );
	lyt->addLayout( btnLyt );

	connect( le, SIGNAL( textEdited( QString ) ), this, SLOT( handleTextChanged( QString ) ) );
	connect( le, SIGNAL( returnPressed() ), this, SLOT( createFileFolder() ) );

	connect( okBtn, SIGNAL( clicked() ), this, SLOT( createFileFolder() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( cancel() ) );

	setLayout( lyt );
	le->setFocus();

	setFixedSize( 450, 150 );
};

void NBNewNodeDialog::handleTextChanged( QString newText ) {

	if ( newText.isEmpty() ) {
		okBtn->setDisabled( true );
		return;
	}

	if ( ( newText == name ) or ( dir.entryList().contains( newText ) ) )
		okBtn->setDisabled( true );

	else
		okBtn->setEnabled( true );
}

void NBNewNodeDialog::createFileFolder() {

	if ( windowTitle() == QString( "New Folder" ) ) {
		if ( !dir.mkdir( le->text() ) ) {
			NBMessageDialog::critical( this, "Access Error",
				"There was an error creating the folder. May be you donot have sufficient permission to write here." );

			return;
		}

		emit nodeCreated( dir.filePath( le->text() ) );
	}

	else if ( windowTitle() == QString( "New File" ) ) {
		QFile file( dir.filePath( le->text() ) );
		if ( !file.open( QFile::WriteOnly ) ) {
			NBMessageDialog::critical( this, "Access Error",
				"There was an error creating the file. May be you donot have sufficient permission to write here." );

			return;
		}

		emit nodeCreated( dir.filePath( le->text() ) );
		file.close();
	}

	else {
		QFile file( dir.filePath( le->text() ) );
		if ( !file.open( QFile::WriteOnly ) ) {
			NBMessageDialog::critical( this, "Access Error",
				"There was an error creating the file. May be you donot have sufficient permission to write here." );

			return;
		}

		file.write( data.toStdString().c_str(), data.length() );
		file.close();

		emit nodeCreated( dir.filePath( le->text() ) );
	}

	close();
}

void NBNewNodeDialog::cancel() {

	le->clear();
	close();
}
