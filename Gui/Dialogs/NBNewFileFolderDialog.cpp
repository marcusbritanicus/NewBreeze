/*
	*
	* NBNewFileFolderDialog.cpp - NewBreeze NewFileFolderWidget Class
	*
*/

#include <iostream>
#include <cstdio>

// #include <NBFolderView.hpp>
#include <NBFVDialogs.hpp>
#include <NBDialog.hpp>
#include <NBMessageDialog.hpp>
#include <NBTools.hpp>

NBNewFileFolderDialog::NBNewFileFolderDialog( QString type, QDir curDir, QString text ) : NBDialog() {

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
	le = new QLineEdit();

	okBtn = new QPushButton();
	okBtn->setObjectName( tr( "okBtn" ) );

	cancelBtn = new QPushButton();
	cancelBtn->setObjectName( tr( "cancelBtn" ) );

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

	okBtn->setIcon( QIcon( ":/icons/ok.png" ) );
	okBtn->setText( "&Ok" );
	okBtn->setAutoDefault( true );

	cancelBtn->setIcon( QIcon( ":/icons/cancel.png" ) );
	cancelBtn->setText( "&Cancel" );

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( okBtn );
	btnLyt->addWidget( cancelBtn );

	lyt->addWidget( lbl );
	lyt->addWidget( le );
	lyt->addLayout( btnLyt );

	connect( le, SIGNAL( textEdited( QString ) ), this, SLOT( handleTextChanged( QString ) ) );
	connect( le, SIGNAL( returnPressed() ), this, SLOT( createFileFolder() ) );

	connect( okBtn, SIGNAL( clicked() ), this, SLOT( createFileFolder() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( cancel() ) );

	setLayout( lyt );
	le->setFocus();

	setFixedSize( 450, 150 );
};

void NBNewFileFolderDialog::handleTextChanged( QString newText ) {

	if ( newText.isEmpty() ) {
		okBtn->setDisabled( true );
		return;
	}

	if ( ( newText == name ) or ( dir.entryList().contains( newText ) ) )
		okBtn->setDisabled( true );

	else
		okBtn->setEnabled( true );
}

void NBNewFileFolderDialog::createFileFolder() {

	if ( windowTitle() == QString( "New Folder" ) ) {
		if ( !dir.mkdir( le->text() ) ) {
			NBMessageDialog::critical( "Access Error",
				"There was an error creating the folder. May be you donot have sufficient permission to write here." );

			return;
		}

		emit nodeCreated( dir.filePath( le->text() ) );
	}

	else if ( windowTitle() == QString( "New File" ) ) {
		QFile file( dir.filePath( le->text() ) );
		if ( !file.open( QFile::WriteOnly ) ) {
			NBMessageDialog::critical( "Access Error",
				"There was an error creating the file. May be you donot have sufficient permission to write here." );

			return;
		}

		emit nodeCreated( dir.filePath( le->text() ) );
		file.close();
	}

	else {
		QFile file( dir.filePath( le->text() ) );
		if ( !file.open( QFile::WriteOnly ) ) {
			NBMessageDialog::critical( "Access Error",
				"There was an error creating the file. May be you donot have sufficient permission to write here." );

			return;
		}

		file.write( data.toStdString().c_str(), data.length() );
		file.close();

		emit nodeCreated( dir.filePath( le->text() ) );
	}

	close();
}

void NBNewFileFolderDialog::cancel() {

	le->clear();
	close();
}
