/*
	*
	* NBNewNodeDialog.hpp - Dialog for creating a new file/folder for NewBreeze
	*
*/

#include "NBNewNodeDialog.hpp"

NBNewNodeDialog::NBNewNodeDialog( QString type, QDir curDir, QString text, QWidget *parent ) : NBDialog( parent ) {

	if ( type == QString( "dir" ) )
		setDialogTitle( "New Folder" );

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
		lbl->setText( tr( "Create a new folder in <b>%1</b> named:" ).arg( curDir.path() ) );
		lbl->setWordWrap( true );
		le->setText( "New Folder" );
	}

	else {
		lbl->setText( tr( "Paste clipboard contents in the file named:" ) );
		le->setText( "New File" );
	}

	le->selectAll();

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( segBtns );

	lyt->addWidget( lbl );
	lyt->addWidget( le );
	lyt->addLayout( btnLyt );

	connect( le, SIGNAL( textEdited( QString ) ), this, SLOT( handleTextChanged( QString ) ) );
	connect( le, SIGNAL( returnPressed() ), this, SLOT( createFileFolder() ) );

	connect( okBtn, SIGNAL( clicked() ), this, SLOT( createFileFolder() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( cancel() ) );

	setLayout( lyt );
	le->setFocus();

	setFixedSize( 450, 120 );
};

void NBNewNodeDialog::handleTextChanged( QString newText ) {

	if ( newText.isEmpty() ) {
		okBtn->setDisabled( true );
		return;
	}

	if ( dir.entryList().contains( newText ) )
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

NBNewFileDialog::NBNewFileDialog( QDir curDir, QWidget *parent ) : NBDialog( parent ) {

	setDialogTitle( "Create New File" );

	QVBoxLayout *lyt = new QVBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	dir = QDir( curDir );
	dir.makeAbsolute();

	// [ "Empty File", "Open Document Text", "Open Document Presentation", "Open Document SpreadSheet", "Open Document Drawing", "HTML File", "Text File" ]
	mimetypes << "application/octet-stream";
	mimetypes << "application/vnd.oasis.opendocument.graphics";
	mimetypes << "application/vnd.oasis.opendocument.presentation";
	mimetypes << "application/vnd.oasis.opendocument.spreadsheet";
	mimetypes << "application/vnd.oasis.opendocument.text";
	mimetypes << "text/html";
	mimetypes << "text/plain";

	cb = new QComboBox( this );
	Q_FOREACH( QString mimename, mimetypes ) {
		QMimeType mime = mimeDb.mimeTypeForName( mimename );
		cb->addItem( QIcon::fromTheme( mime.iconName(), QIcon::fromTheme( mime.genericIconName() ) ), mime.comment() );
	}

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

	lbl->setText( tr( "Create a new file in <b>%1</b> named:" ).arg( curDir.path() ) );
	le->setText( "New File" );

	le->selectAll();

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( segBtns );

	lyt->addWidget( lbl );
	lyt->addWidget( cb );
	lyt->addWidget( le );
	lyt->addLayout( btnLyt );

	connect( le, SIGNAL( textEdited( QString ) ), this, SLOT( handleTextChanged( QString ) ) );
	connect( le, SIGNAL( returnPressed() ), this, SLOT( createFile() ) );

	connect( okBtn, SIGNAL( clicked() ), this, SLOT( createFile() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( cancel() ) );

	setLayout( lyt );
	le->setFocus();

	setFixedSize( 450, 150 );
};

void NBNewFileDialog::handleTextChanged( QString newText ) {

	if ( newText.isEmpty() ) {
		okBtn->setDisabled( true );
		return;
	}

	if ( dir.entryList().contains( newText ) )
		okBtn->setDisabled( true );

	else
		okBtn->setEnabled( true );
}

void NBNewFileDialog::createFile() {

	/* Current Index */
	int index = cb->currentIndex();

	/* Return value */
	bool ret = false;

	/* Suffix List */
	QStringList suffixes = QStringList() << "" << ".odg" << ".odp" << ".ods" << ".odt" << ".html" << ".txt";
	QString sfx = suffixes.value( index );

	/* File Name */
	QString name = le->text();

	/* Add the suffix */
	if ( not name.endsWith( sfx ) )
		name += sfx;

	switch( index ) {
		case 0:
		case 5:
		case 6: {
			QFile file( dir.filePath( name ) );
			ret = file.open( QFile::WriteOnly );
			file.close();

			break;
		}

		case 1:
		case 2:
		case 3:
		case 4: {
			ret = QFile::copy( "/usr/share/newbreeze/templates/empty" + suffixes.value( index ), name );
			break;
		}
	}

	if ( ret )
		emit nodeCreated( dir.filePath( name ) );

	else
		NBMessageDialog::critical( this, "Access Error", "There was an error creating the file. May be you donot have sufficient permission to write here." );

	close();
}

void NBNewFileDialog::cancel() {

	le->clear();
	close();
}
