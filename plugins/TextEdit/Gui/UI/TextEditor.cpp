/*
	*
	* TextEditor.cpp - TextEditor UI Class
	*
*/

// Local Headers
#include <Global.hpp>
#include <TextEditor.hpp>
#include <FileProps.hpp>
#include <NBTools.hpp>

TextEditor::TextEditor( QString fn, QWidget *parent ) : QMainWindow( parent ) {

	filename = QString( fn );
	preview = false;

	// Setup base gui
	setupGui();
	setupConnections();
	setWindowProperties();

	QTimer::singleShot( 0, this, SLOT( loadFile() ) );
};

void TextEditor::setupGui()  {

	QMimeType mime = mimeDb.mimeTypeForFile( filename );
	ed = new QsciEditor( this );

	QLabel *iconLbl = new QLabel( this );
	iconLbl->setStyleSheet( "background-color: darkgray; border-top-left-radius: 2px; border-bottom-left-radius: 2px;" );
	iconLbl->setFixedSize( QSize( 24, 24 ) );
	iconLbl->setPixmap( QIcon::fromTheme( mime.iconName() ).pixmap( 16 ) );
	iconLbl->setAlignment( Qt::AlignCenter );

	QLabel *nameLbl = new QLabel( mime.name() );
	nameLbl->setFixedHeight( 24 );
	nameLbl->setStyleSheet( "background-color: lightgray; border-top-right-radius: 2px; border-bottom-right-radius: 2px; padding-right: 2px;" );

	buildToolBar();

	editBtn = new QToolButton( this );
	editBtn->setIcon( QIcon::fromTheme( "document-edit" ) );
	editBtn->setText( "Edit" );
	editBtn->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	editBtn->setAutoRaise( false );
	editBtn->setFixedSize( QSize( 52, 24 ) );
	editBtn->setFocusPolicy( Qt::NoFocus );
	editBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border-radius: 2px; }" );
	connect( editBtn, SIGNAL( clicked() ), this, SLOT( switchToEditMode() ) );
	editBtn->hide();

	openBtn = new QToolButton( this );
	openBtn->setIcon( QIcon( ":/icons/maximize.png" ) );
	openBtn->setAutoRaise( false );
	openBtn->setFixedSize( QSize( 24, 24 ) );
	openBtn->setFocusPolicy( Qt::NoFocus );
	openBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border-radius: 2px; }" );
	connect( openBtn, SIGNAL( clicked() ), this, SLOT( openInExternal() ) );
	openBtn->hide();

	QHBoxLayout *ToolLyt = new QHBoxLayout();
	ToolLyt->setContentsMargins( QMargins( 5, 4, 7, 7 ) );
	ToolLyt->setSpacing( 0 );

	ToolLyt->addWidget( iconLbl );
	ToolLyt->addWidget( nameLbl );
	ToolLyt->addStretch( 0 );
	ToolLyt->addWidget( toolBar );
	ToolLyt->addWidget( editBtn );
	ToolLyt->addWidget( Spacer::horizontal( 5, this ) );
	ToolLyt->addWidget( openBtn );

	BaseLayout = new QVBoxLayout();
	BaseLayout->setContentsMargins( QMargins( 0, 3, 0, 0 ) );
	BaseLayout->setSpacing( 0 );

	BaseLayout->addLayout( ToolLyt );
	BaseLayout->addWidget( ed );

	buildStatusBar();

	QWidget *Base = new QWidget( this );
	Base->setObjectName( "base" );
	Base->setLayout( BaseLayout );

	setCentralWidget( Base );
};

void TextEditor::buildStatusBar() {

	QStatusBar *sb = statusBar();
	sb->setStyleSheet(
		"QSizeGrip{ background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 transparent, stop: 0.49 transparent, stop: 0.5 gray, stop: 1 gray); }" );

	lineCountLbl = new QLabel( this );
	lineCountLbl->setStyleSheet( "padding-left: 10px; padding-right: 10px;" );

	cursorPosLbl = new QLabel( this );
	cursorPosLbl->setStyleSheet( "padding-left: 10px; padding-right: 10px;" );

	charCountLbl = new QLabel( this );
	charCountLbl->setStyleSheet( "padding-left: 10px; padding-right: 10px;" );

	selectionLbl = new QLabel( this );
	selectionLbl->setStyleSheet( "padding-left: 10px; padding-right: 10px;" );

	safetyLed = new LedLight( 8, this );
	safetyLed->setStyleSheet( "padding-left: 10px; padding-right: 10px;" );

	sb->addPermanentWidget( lineCountLbl );
	sb->addPermanentWidget( cursorPosLbl );
	sb->addPermanentWidget( charCountLbl );
	sb->addPermanentWidget( selectionLbl );
	sb->addPermanentWidget( safetyLed );
};

void TextEditor::buildToolBar() {

	/* File Actions */
	saveBtn = new QToolButton( this );
	saveBtn->setIcon( QIcon::fromTheme( "document-save" ) );
	saveBtn->setAutoRaise( false );
	saveBtn->setFixedSize( QSize( 24, 24 ) );
	saveBtn->setShortcut( tr( "Ctrl+S" ) );
	saveBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border-top-left-radius: 2px; border-bottom-left-radius: 2px; }" );
	connect( saveBtn, SIGNAL( clicked() ), this, SLOT( saveFile() ) );

	printBtn = new QToolButton( this );
	printBtn->setIcon( QIcon::fromTheme( "document-print" ) );
	printBtn->setAutoRaise( false );
	printBtn->setFixedSize( QSize( 24, 24 ) );
	printBtn->setShortcut( tr( "Ctrl+P" ) );
	printBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border: none; }" );
	connect( printBtn, SIGNAL( clicked() ), this, SLOT( printFile() ) );

	propsBtn = new QToolButton( this );
	propsBtn->setIcon( QIcon::fromTheme( "document-properties" ) );
	propsBtn->setAutoRaise( false );
	propsBtn->setFixedSize( QSize( 24, 24 ) );
	propsBtn->setShortcut( tr( "Ctrl+Alt+P" ) );
	propsBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border-top-right-radius: 2px; border-bottom-right-radius: 2px; }" );
	connect( propsBtn, SIGNAL( clicked() ), this, SLOT( fileProps() ) );

	/* Edit Actions */
	cutBtn = new QToolButton( this );
	cutBtn->setIcon( QIcon::fromTheme( "edit-cut" ) );
	cutBtn->setAutoRaise( false );
	cutBtn->setFixedSize( QSize( 24, 24 ) );
	cutBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border-top-left-radius: 2px; border-bottom-left-radius: 2px; }" );
	connect( cutBtn, SIGNAL( clicked() ), ed, SLOT( cut() ) );

	copyBtn = new QToolButton( this );
	copyBtn->setIcon( QIcon::fromTheme( "edit-copy" ) );
	copyBtn->setAutoRaise( false );
	copyBtn->setFixedSize( QSize( 24, 24 ) );
	copyBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border: none; }" );
	connect( copyBtn, SIGNAL( clicked() ), ed, SLOT( copy() ) );

	pasteBtn = new QToolButton( this );
	pasteBtn->setIcon( QIcon::fromTheme( "edit-paste" ) );
	pasteBtn->setAutoRaise( false );
	pasteBtn->setFixedSize( QSize( 24, 24 ) );
	pasteBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border-top-right-radius: 2px; border-bottom-right-radius: 2px; }" );
	connect( pasteBtn, SIGNAL( clicked() ), ed, SLOT( paste() ) );

	selectBtn = new QToolButton( this );
	selectBtn->setIcon( QIcon::fromTheme( "edit-select-all" ) );
	selectBtn->setAutoRaise( false );
	selectBtn->setFixedSize( QSize( 24, 24 ) );
	selectBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border-radius: 2px }" );
	connect( selectBtn, SIGNAL( clicked() ), ed, SLOT( selectAll() ) );

	undoBtn = new QToolButton( this );
	undoBtn->setIcon( QIcon::fromTheme( "edit-undo" ) );
	undoBtn->setAutoRaise( false );
	undoBtn->setFixedSize( QSize( 24, 24 ) );
	undoBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border-top-left-radius: 2px; border-bottom-left-radius: 2px; }" );
	connect( undoBtn, SIGNAL( clicked() ), ed, SLOT( undo() ) );

	redoBtn = new QToolButton( this );
	redoBtn->setIcon( QIcon::fromTheme( "edit-redo" ) );
	redoBtn->setAutoRaise( false );
	redoBtn->setFixedSize( QSize( 24, 24 ) );
	redoBtn->setStyleSheet( "QToolButton{ background-color: lightgray; border-top-right-radius: 2px; border-bottom-right-radius: 2px; }" );
	connect( redoBtn, SIGNAL( clicked() ), ed, SLOT( redo() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->setContentsMargins( QMargins() );
	btnLyt->setSpacing( 0 );

	btnLyt->addWidget( saveBtn );
	btnLyt->addWidget( printBtn );
	btnLyt->addWidget( propsBtn );

	btnLyt->addWidget( Spacer::horizontal( 5, this ) );

	btnLyt->addWidget( cutBtn );
	btnLyt->addWidget( copyBtn );
	btnLyt->addWidget( pasteBtn );

	btnLyt->addWidget( Spacer::horizontal( 5, this ) );

	btnLyt->addWidget( selectBtn );

	btnLyt->addWidget( Spacer::horizontal( 5, this ) );

	btnLyt->addWidget( undoBtn );
	btnLyt->addWidget( redoBtn );

	toolBar = new QWidget( this );
	toolBar->setLayout( btnLyt );

	updateToolBar();
};

void TextEditor::setupConnections() {

	connect( ed, SIGNAL( modificationChanged( bool ) ), this, SLOT( updateModStatus( bool ) ) );
	connect( ed, SIGNAL( autoSaved() ), this, SLOT( handleAutoSaved() ) );

	connect( ed, SIGNAL( cursorPositionChanged( int, int ) ), this, SLOT( updateStatusBar( int, int ) ) );
	// connect( ed, SIGNAL( blockCountChanged( int ) ), this, SLOT( updateStatusBar() ) );
	connect( ed, SIGNAL( textChanged() ), this, SLOT( updateStatusBar() ) );
	connect( ed, SIGNAL( selectionChanged() ), this, SLOT( updateStatusBar() ) );

	connect( ed, SIGNAL( modificationChanged( bool ) ), this, SLOT( updateToolBar() ) );
	connect( ed, SIGNAL( textChanged() ), this, SLOT( updateToolBar() ) );
	connect( ed, SIGNAL( selectionChanged() ), this, SLOT( updateToolBar() ) );
	connect( ed, SIGNAL( undoAvailable( bool ) ), this, SLOT( updateToolBar() ) );
	connect( ed, SIGNAL( redoAvailable( bool ) ), this, SLOT( updateToolBar() ) );
	connect( qApp->clipboard(), SIGNAL( dataChanged() ), this, SLOT( updateToolBar() ) );
};

void TextEditor::setWindowProperties() {

	setWindowTitle( "NewBreeze TextEditor - " + baseName( filename ) );
	setMinimumSize( 800, 600 );

	QRect scrnSize = QDesktopWidget().screenGeometry();
	int hpos = ( int )( ( scrnSize.width() - 800 ) / 2 );
	int vpos = ( int )( ( scrnSize.height() - 600 ) / 2 );

	move( hpos, vpos );
};

void TextEditor::setPreviewMode() {

	preview = true;

	closeSC = new QShortcut( Qt::Key_Escape, this, SLOT( close() ), SLOT( close() ), Qt::WindowShortcut );

	toolBar->hide();
	openBtn->show();
	editBtn->show();

	ed->setReadOnly( true );

	setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );
};

void TextEditor::switchToEditMode() {

	preview = false;

	closeSC->setEnabled( false );

	toolBar->show();
	openBtn->hide();
	editBtn->hide();

	ed->setReadOnly( false );
	setWindowFlags( Qt::Window );

	showMaximized();
};

void TextEditor::loadFile() {

	ed->loadFile( filename );

	updateModStatus( ed->isModified() );
	updateStatusBar( -1, -1 );
};

void TextEditor::saveFile() {

	ed->saveFile();
	ed->setModified( false );

	statusBar()->showMessage( "The file was saved successfully.", 1000 );
};

void TextEditor::printFile() {

	QPrinter *printer = new QPrinter();
	QPrintDialog *dialog = new QPrintDialog( printer, this );

	if ( dialog->exec() ) {
		QTextDocument *doc = new QTextDocument();
		doc->setPlainText( ed->text() );
		doc->setDefaultFont( ed->font() );
		doc->setIndentWidth( 4 );
		doc->print( printer );
	}
};

void TextEditor::fileProps() {

	wFileProps *props = new wFileProps( ed );
	props->exec();
};

void TextEditor::changeFont( QFont newFont ) {

	QSettings settings( "NewBreeze", "TextEditorPlugin" );
	settings.setValue( "Font", newFont );

	ed->setFont( newFont );
};

void TextEditor::updateModStatus( bool modified ) {

	safetyLed->setSafe( not modified );
};

void TextEditor::updateStatusBar( int line, int idx ) {

	if ( line == -1 or idx == -1 )
		ed->getCursorPosition( &line, &idx );

	lineCountLbl->setText( QString( "Line: %1/%2" ).arg( line + 1 ).arg( ed->lines() ) );
	cursorPosLbl->setText( QString( "Pos: %1" ).arg( idx + 1 ) );
	charCountLbl->setText( QString( "Chars: %1" ).arg( ed->charCount() ) );
	selectionLbl->setText( QString( "Sel: %1" ).arg( ed->selectedText().count() ) );
};

void TextEditor::updateStatusBar() {

	updateStatusBar( -1, -1 );
};

void TextEditor::updateToolBar() {

	saveBtn->setEnabled( ed->isModified() );
	printBtn->setEnabled( ed->charCount() );
	propsBtn->setEnabled( ed->charCount() );
	cutBtn->setEnabled( ed->hasSelectedText() );
	copyBtn->setEnabled( ed->hasSelectedText() );
	pasteBtn->setEnabled( ed->SendScintilla( QsciScintilla::SCI_CANPASTE ) );
	selectBtn->setEnabled( ed->charCount() );
	undoBtn->setEnabled( ed->isUndoAvailable() );
	redoBtn->setEnabled( ed->isRedoAvailable() );
};

void TextEditor::handleAutoSaved() {

	statusBar()->showMessage( "The file was automatically saved.", 1000 );
};

void TextEditor::openInExternal() {

	QProcess::startDetached( "xdg-open " + filename );
	close();
};

void TextEditor::resizeEvent( QResizeEvent *rEvent ) {

	QMainWindow::resizeEvent( rEvent );
	rEvent->accept();

	if ( isFullScreen() or isMaximized() ) {
		BaseLayout->setContentsMargins( QMargins( 0, 3, 0, 0 ) );
	}

	else {
		BaseLayout->setContentsMargins( QMargins( 1, 3, 1, 1 ) );
	}
};

void TextEditor::paintEvent( QPaintEvent *pEvent ) {

	QMainWindow::paintEvent( pEvent );

	if ( preview ) {
		QPainter *painter = new QPainter( this );

		painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 2.0 ) );
		painter->drawRect( rect().adjusted( +1, +1, -1, -1 ) );

		painter->end();
	}

	pEvent->accept();
};

void TextEditor::closeEvent( QCloseEvent *cEvent ) {

	/* If the widget in this tab is an instance of Editor, then save the changes if any. */
	/* Otherwise, the tab will be a RecentFiles tab, just close it. */
	if ( ed and ed->isModified() ) {
		long reply = QMessageBox::question( this, "Save changes", "This file contains unsaved changes. How would you like to proceed?",
					QMessageBox::Cancel, QMessageBox::Discard, QMessageBox::Save );

		if ( reply == QMessageBox::Save ) {
			saveFile();
		}

		else if ( reply == QMessageBox::Discard ) {
			ed->setModified( false );
		}

		else {
			cEvent->ignore();
			return;
		}
	}

	cEvent->accept();
};
