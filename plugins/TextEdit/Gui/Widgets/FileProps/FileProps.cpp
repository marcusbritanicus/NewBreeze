/*
    *
    * wFileProps.cpp - TextEditor's File Properties Class
    *
*/

#include <Global.hpp>
#include <FileProps.hpp>
#include <Tools.hpp>
#include <Editor.hpp>

wFileProps::wFileProps( Editor *editor ) : QDialog( editor ) {

	fileName = editor->fileName();
	if ( fileName.isEmpty() ) {
		fileName = QString( "Untitled" );
		noFileProps = true;
	}
	else
		noFileProps = false;

	lines = QString::number( editor->blockCount() );
	words = QString::number( editor->wordCount() );
	chars = QString::number( editor->document()->characterCount() );

	createGUI();
	setWindowProperties();
};

void wFileProps::createGUI() {

	QString mimeType = getMimeType( fileName );

	QFontDatabase fdb = QFontDatabase();
	fdb.addApplicationFont( ":/fonts/ArchitectsDaughter.ttf" );
	setStyleSheet( "font-family: Architects Daughter; font-size: 10;" );

	iconLbl = new QLabel();
	iconLbl->setPixmap( getPixmap( fileName ).scaled( 64, 64, Qt::KeepAspectRatio ) );

	nameLbl = new QLabel();
	nameLbl->setText( "<b>" + QFileInfo( fileName ).fileName() + "</b>" );
	nameLbl->setFont( QFont( "Architects Daughter", 15 ) );

	linesLbl = new QLabel( lines );
	wordsLbl = new QLabel( words );
	charsLbl= new QLabel( chars );

	if ( !noFileProps ) {
		typeLbl = new QLabel( mimeType );
		sizeLbl = new QLabel( formatSize( getSize( fileName ) ) );
		locLbl = new QLabel( QFileInfo( fileName ).absolutePath() );

		time1Lbl = new QLabel( QFileInfo( fileName ).created().toString( "ddd MMM dd, yyyy hh:mm:ss" ) );
		time2Lbl = new QLabel( QFileInfo( fileName ).lastRead().toString( "ddd MMM dd, yyyy hh:mm:ss" ) );
		time3Lbl = new QLabel( QFileInfo( fileName ).lastModified().toString( "ddd MMM dd, yyyy hh:mm:ss" ) );
	}

	QPushButton *closeBtn = new QPushButton();
	closeBtn->setText( "&Close" );
	connect( closeBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	QHBoxLayout *nameLyt = new QHBoxLayout();
	QFormLayout *textInfoLyt = new QFormLayout();
	QFormLayout *fileInfoLyt = new QFormLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	nameLyt->setAlignment( Qt::AlignCenter );
	nameLyt->addWidget( iconLbl );
	nameLyt->addWidget( nameLbl );

	textInfoLyt->addRow( new QLabel( "<b>Lines: </b>" ), linesLbl );
	textInfoLyt->addRow( new QLabel( "<b>Words: </b>" ), wordsLbl );
	textInfoLyt->addRow( new QLabel( "<b>Chars: </b>" ), charsLbl );
	textInfoLyt->setFormAlignment( Qt::AlignHCenter | Qt::AlignTop );

	if ( !noFileProps ) {
		fileInfoLyt->addRow( new QLabel( "<b>Type: </b>" ), typeLbl );
		fileInfoLyt->addRow( new QLabel( "<b>Size: </b>" ), sizeLbl );
		fileInfoLyt->addRow( new QLabel( "<b>Location: </b>" ), locLbl );
		fileInfoLyt->addRow( new QLabel( "<b>Created: </b>" ), time1Lbl );
		fileInfoLyt->addRow( new QLabel( "<b>Accessed: </b>" ), time2Lbl );
		fileInfoLyt->addRow( new QLabel( "<b>Modified: </b>" ), time3Lbl );
	}

	btnLyt->addStretch( 0 );
	btnLyt->addWidget( closeBtn );
	btnLyt->addStretch( 0 );

	QFrame *textFrame = new QFrame();
	textFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Plain );
	textFrame->setLayout( textInfoLyt );

	QFrame *fileFrame = new QFrame();
	fileFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Plain );
	fileFrame->setLayout( fileInfoLyt );

	baseLyt->addLayout( nameLyt );
	baseLyt->addStretch( 0 );
	baseLyt->addWidget( textFrame );
	if ( !noFileProps ) {
		baseLyt->addStretch( 0 );
		baseLyt->addWidget( fileFrame );
	}
	baseLyt->addStretch( 0 );
	baseLyt->addLayout( btnLyt );

	setLayout( baseLyt );
};

void wFileProps::setWindowProperties() {

	if ( noFileProps )
		setFixedSize( 500, 210 );
	else
		setFixedSize( 500, 390 );

	setWindowTitle( QFileInfo( fileName ).fileName() + " - Properties" );
};
