/*
	*
	* QsciEditor - TextEditor Core based on QScintilla
	*
*/

#include "QsciEditor.hpp"

QsciEditor::QsciEditor( QWidget *parent ) : QsciScintilla( parent ) {

	setFocus();

	/* Set tab width, and fonts */
	setTabWidth( 4 );
	setFont( QFont( "Envy Code R", 9 ) );
	setTabIndents( true );
	setIndentationsUseTabs( true );

	/* Set folding style */
	setFolding( QsciScintilla::BoxedTreeFoldStyle );

	/* Remove Horizontal Scrollbar, put a Vertical one */
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

	/* Whitespace, indentation markings, and eol */
	setIndentationGuides( false );
	setWhitespaceVisibility( WsInvisible );
	setEolMode( EolUnix );

	/* Create Margins */
	setMarginWidth( 0, QString( " %1 " ).arg( lines() ) );
	setMarginWidth( 1, " " );
	setMarginWidth( 2, 10 );
	setMarginsFont( QFont( "Envy Code R", 9 ) );
	setWrapMode( QsciScintilla::WrapWord );

	/* Current Line Visibility */
	ensureCursorVisible();
	setCaretLineVisible( true );
	setCaretLineBackgroundColor( QColor( "#555555" ) );
	setCaretForegroundColor( QColor( "#FAFFAF" ) );
	setCaretWidth( 2 );

	/* Selection Color */
	// setSelectionBackgroundColor( "#333964" );
	// setSelectionForegroundColor( "*" );

	/* Margin and folding margins background colors */
	setMarginsBackgroundColor( QColor( "#A0A0A0" ) );
	setFoldMarginColors( QColor( "#666666" ), Qt::black );

	/* Set the width of the line */
	setLineWidth( 144 );
	ensureLineVisible( true );

	/* Set the paper and pen color - Editor BG and FG color */
	setPaper( Qt::black );
	setColor( Qt::white );

	/* Brace coloring */
	setBraceMatching( QsciScintilla::SloppyBraceMatch );
	setUnmatchedBraceForegroundColor( Qt::red );
	setUnmatchedBraceBackgroundColor( Qt::black );
	setMatchedBraceForegroundColor( Qt::darkGreen );
	setMatchedBraceBackgroundColor( Qt::black );

	/* Multi-selection and typing */
	SendScintilla( SCI_SETADDITIONALSELECTIONTYPING, true );
	SendScintilla( SCI_SETMULTIPLESELECTION, true );
	SendScintilla( SCI_SETMULTIPASTE, true );

	/* Scroll beyond the last line */
	SendScintilla( SCI_SETENDATLASTLINE, false );

	/* Set the editor state as not modified */
	setModified( false );

	/* Auto Save Timer: 60 sec */
	autoSaveTimer = new QBasicTimer();

	/* Enable and Show Custom context Menu */
	setContextMenuPolicy( Qt::CustomContextMenu );

	/* Connections */
	setupActions();

	/* Default Lexer */
	setLexer( new QLexerDefault() );

    QsciScintilla::FoldStyle state = static_cast<QsciScintilla::FoldStyle>( ( !folding() ) * 5 );
    if ( !state )
        foldAll( false );
    setFolding( state );
};

bool QsciEditor::loadFile( QString filename) {
	/* Load the file */

	mFilePath = QString( filename );
	QFileInfo info( filename );

	/* Check if we have a file */
	if ( not info.isFile() )
		return false;

	/* Check if we can open a file */
	QFile file( filename );
	if ( not file.open( QFile::ReadOnly ) )
		return false;

	/* Set the Lexer */
	setAutoLexer();

	/* We will use a UTF8 charset */
	setUtf8( true );

	/* Set cursor busy */
	QApplication::setOverrideCursor( Qt::WaitCursor );

	/* Load the file into the editor */
	while ( not file.atEnd() )
		append( QString::fromLocal8Bit( file.read( 8192 ) ).replace( "\r\n", "\n" ) );

	/* Restore cursor and close file */
	QApplication::restoreOverrideCursor();
	file.close();

	/* Set editor as readonly if we dont have write permissions */
	if ( not info.isWritable() )
		setReadOnly( true );

	/* Focus the editor */
	setFocus();

	/* Start the 1m Autosave timer */
	autoSaveTimer->start( 60000, this );

	/* Set the editor as not modified */
	setModified( false );

	return true;
};

bool QsciEditor::saveFile( QString filepath ) {

	if ( autoSaveTimer->isActive() )
		autoSaveTimer->stop();

	/* Save file */
	if ( filepath.count() ) {
		mFilePath = filepath;
		setAutoLexer();
	}

	QFile file( mFilePath );
	if ( not file.open( QIODevice::WriteOnly ) )
		return false;

	file.write( text().toLocal8Bit() );
	file.close();

	setModified( false );

	autoSaveTimer->start( 60000, this );

	return true;
};

bool QsciEditor::reloadFile() {

	/* Check of we have loaded a file */
	qWarning() << "Empty file:" << mFilePath.isEmpty();
	if ( mFilePath.isEmpty() )
		return false;

	if ( isModified() ) {
		int reply = QMessageBox::question(
			this,
			"Wordy - Reload?",
			"<p>This file has been modified. If you reload the file now, all the changes will be lost. "
			"Do you want to reload the file?</p>"
			"<p>Press <tt>[Yes]</tt> to reload the file, <tt>[No]</tt> go back.</p>",
			QMessageBox::Yes, QMessageBox::No
		);

		if ( reply == QMessageBox::No )
			return false;
	}

	loadFile( mFilePath );
	return true;
};

qint64 QsciEditor::lineCount() {

	return lines();
};

qint64 QsciEditor::wordCount() {

	return text().split( QRegExp( "\\s", Qt::CaseInsensitive ), QString::SkipEmptyParts ).count();
};

qint64 QsciEditor::charCount() {

	return text().length();
};

QString QsciEditor::fileName() {

	return mFilePath;
};

void QsciEditor::setFileName( QString fn) {

	mFilePath = QString( fn );
	setAutoLexer();
};

void QsciEditor::resizeMargins() {

	setMarginWidth( 0, QString( " %1 " ).arg( lines() ) );
};

void QsciEditor::setAutoLexer() {

	QString sfx = QFileInfo( mFilePath ).suffix();
	QString mimeType = mimeDb.mimeTypeForFile( mFilePath ).name().replace( "text/", "" );

	if ( mimeType.contains( "python" ) )
		setLexer( new QLexerPython() );

	else if ( mimeType.contains( "html" ) )
		setLexer( new QsciLexerHTML() );

	else if ( mimeType.contains( "tex" ) )
		setLexer( new QsciLexerLaTeX() );

	else if ( mimeType.contains( "makefile" ) )
		setLexer( new QsciLexerMakefile() );

	else if ( mimeType.contains( "perl" ) )
		setLexer( new QsciLexerPerl() );

	else if ( mimeType.contains( "xml" ) )
		setLexer( new QsciLexerHTML() );

	else if ( mimeType.contains( "shell" ) )
		setLexer( new QsciLexerBash() );

	else if ( QStringList( QStringList() << "c" << "cc" << "cpp" << "h" << "hh" << "hpp" ).contains( sfx ) )
		setLexer( new QsciLexerCPP() );

	else if ( sfx == "jl" )
		setLexer( new QLexerJulia() );

	else
		setLexer( new QLexerDefault() );

	/* Monospace fonts */
	lexer()->setFont( QFont( "Monospace", 9 ) );
};

void QsciEditor::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == autoSaveTimer->timerId() ) {
	autoSaveTimer->stop();
		if ( not isModified() ) {
			autoSaveTimer->start( 60000, this );
			tEvent->accept();
		}

		else {
			/* If we have no name for the file, return */
			if ( mFilePath.isEmpty() )
				return;

			saveFile();
			emit autoSaved();
			tEvent->accept();
		}
	}
};

void QsciEditor::setupActions() {

	cutAct = new QAction( QIcon::fromTheme( "edit-cut" ), "Cu&t", this );
	cutAct->setStatusTip( "Cut the selected text" );
	cutAct->setShortcut( tr( "Ctrl+X" ) );

	copyAct = new QAction( QIcon::fromTheme( "edit-copy" ), "&Copy", this );
	copyAct->setStatusTip( "Copy the selected text" );
	copyAct->setShortcut( tr( "Ctrl+C" ) );

	pasteAct = new QAction( QIcon::fromTheme( "edit-paste" ), "&Paste", this );
	pasteAct->setStatusTip( "Paste the text on the clipboard" );
	pasteAct->setShortcut( tr( "Ctrl+V" ) );

	deleteAct = new QAction( QIcon::fromTheme( "edit-delete" ), "&Delete", this );
	deleteAct->setStatusTip( "Delete the selected Text" );
	deleteAct->setShortcut( tr( "Delete" ) );

	selectAct = new QAction( QIcon::fromTheme( "edit-select-all" ), "Select &All", this );
	selectAct->setStatusTip( "Select all the text" );
	selectAct->setShortcut( tr( "Ctrl+A" ) );

	deselectAct = new QAction( QIcon(), "Deselect &All", this );
	deselectAct->setStatusTip( "Deselect all the text" );
	deselectAct->setShortcut( tr( "Ctrl+Shift+A" ) );

	undoAct = new QAction( QIcon::fromTheme( "edit-undo" ), "&Undo", this );
	undoAct->setStatusTip( "Undo the last change" );
	undoAct->setShortcut( tr( "Ctrl+Z" ) );

	redoAct = new QAction( QIcon::fromTheme( "edit-redo" ), "&Redo", this );
	redoAct->setStatusTip( "Redo the undone change" );
	redoAct->setShortcut( tr( "Ctrl+Y" ) );

	searchAct = new QAction( QIcon::fromTheme( "edit-find" ), "&Search", this );
	searchAct->setStatusTip( "Search the document" );
	searchAct->setShortcut( tr( "Ctrl+F" ) );

	replaceAct = new QAction( QIcon::fromTheme( "edit-find-replace" ), "&Replace", this );
	replaceAct->setStatusTip( "Replace text in the document" );
	replaceAct->setShortcut( tr( "Ctrl+R" ) );

	changeFontAct = new QAction( QIcon::fromTheme( "preferences-desktop-font" ), "Change &Font", this );
	changeFontAct->setStatusTip( "Change the font of the document" );
	changeFontAct->setShortcut( tr( "Ctrl+Shift+F" ) );

	toggleCaseAct = new QAction( "T&oggle Case of Selection", this );
	toggleCaseAct->setStatusTip( "Toggle the case of the selected text" );
	toggleCaseAct->setShortcut( tr( "Ctrl+Alt+U" ) );

	connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showCustomMenu( const QPoint& ) ) );

	connect( cutAct, SIGNAL( triggered() ), this, SLOT( cutAction() ) );
	connect( copyAct, SIGNAL( triggered() ), this, SLOT( copyAction() ) );
	connect( pasteAct, SIGNAL( triggered() ), this, SLOT( pasteAction() ) );
	connect( deleteAct, SIGNAL( triggered() ), this, SLOT( deleteAction() ) );

	connect( undoAct, SIGNAL( triggered() ), this, SLOT( undoAction() ) );
	connect( redoAct, SIGNAL( triggered() ), this, SLOT( redoAction() ) );

	connect( selectAct, SIGNAL( triggered() ), this, SLOT( selectAction() ) );
	connect( deselectAct, SIGNAL( triggered() ), this, SLOT( deselectAction() ) );

	connect( changeFontAct, SIGNAL( triggered() ), this, SLOT( changeFont() ) );
	connect( toggleCaseAct, SIGNAL( triggered() ), this, SLOT( toggleCase() ) );

	connect( searchAct, SIGNAL( triggered() ), this, SLOT( showSearch() ) );
	connect( this, SIGNAL( linesChanged() ), this, SLOT( resizeMargins() ) );

	addAction( cutAct );
	addAction( copyAct );
	addAction( pasteAct );
	addAction( deleteAct );
	addAction( selectAct );
	addAction( deselectAct );
	addAction( undoAct );
	addAction( redoAct );
	addAction( searchAct );
	addAction( replaceAct );
	addAction( changeFontAct );
	addAction( toggleCaseAct );

	/* Change keyBindings */
	SendScintilla( SCI_ASSIGNCMDKEY, SCK_DOWN + ( SCMOD_CTRL << 16 ), SCI_PARADOWN );
	SendScintilla( SCI_ASSIGNCMDKEY, SCK_UP + ( SCMOD_CTRL << 16 ), SCI_PARAUP );
	SendScintilla( SCI_ASSIGNCMDKEY, SCK_DOWN + ( SCMOD_ALT << 16 ), SCI_LINESCROLLDOWN );
	SendScintilla( SCI_ASSIGNCMDKEY, SCK_UP + ( SCMOD_ALT << 16 ), SCI_LINESCROLLUP );

	/* Remove Ctrl+D as duplicate line */
	SendScintilla( SCI_CLEARCMDKEY, 68 + ( SCMOD_CTRL << 16 ) );
};

void QsciEditor::showCustomMenu( const QPoint &position ) {

	contextMenu.clear();

	QMenu formatMenu( "&Format" );

	formatMenu.addAction( changeFontAct );
	if ( hasSelectedText() )
		formatMenu.addAction( toggleCaseAct );

	contextMenu.addMenu( &formatMenu );
	contextMenu.addSeparator();

	if ( hasSelectedText() ) {
		contextMenu.addAction( cutAct );
		contextMenu.addAction( copyAct );
	}

	if ( SendScintilla( SCI_CANPASTE ) )
		contextMenu.addAction( pasteAct );

	if ( hasSelectedText() )
		contextMenu.addAction( deleteAct );

	if ( not text().isEmpty() or hasSelectedText() )
		contextMenu.addSeparator();

	if ( not text().isEmpty() )
		contextMenu.addAction( selectAct );

	if ( hasSelectedText() )
		contextMenu.addAction( deselectAct );

	if ( isUndoAvailable() or isRedoAvailable() )
		contextMenu.addSeparator();

	if ( isUndoAvailable() )
		contextMenu.addAction( undoAct );

	if ( isRedoAvailable() )
		contextMenu.addAction( redoAct );

	if ( not text().isEmpty() ) {
		contextMenu.addSeparator();
		contextMenu.addAction( searchAct );
		contextMenu.addAction( replaceAct );
	}

	contextMenu.exec( QWidget::mapToGlobal( position ) );
};

void QsciEditor::showSearch() {

	SearchWidget *search = new SearchWidget( this );
	search->show();
};

void QsciEditor::cutAction() {

	if ( hasSelectedText() )
		SendScintilla( SCI_CUT );
};

void QsciEditor::copyAction() {

	if ( hasSelectedText() )
		SendScintilla( SCI_COPY );
};

void QsciEditor::pasteAction() {

	if ( hasSelectedText() )
		removeSelectedText();

	SendScintilla( SCI_PASTE );
};

void QsciEditor::deleteAction() {

	if ( hasSelectedText() )
		removeSelectedText();
};

void QsciEditor::selectAction() {

	selectAll();
};

void QsciEditor::deselectAction() {

	SendScintilla( SCI_CLEARSELECTIONS );
};

void QsciEditor::undoAction() {

	if ( isUndoAvailable() )
		undo();
};

void QsciEditor::redoAction() {

	if ( isRedoAvailable() )
		redo();
};

void QsciEditor::toggleCase() {

	int lF, iF, lT, iT;

	getSelection( &lF, &iF, &lT, &iT );

	if ( not hasSelectedText() )
		return;

	QString txt = selectedText();
	QString newTxt;

	bool caps = true;

	Q_FOREACH( QChar ch, txt )
		if ( ch.isLetter() )
			caps &= ch.isUpper();

	if ( caps )
		newTxt = txt.toLower();

	else
		newTxt = txt.toUpper();

	removeSelectedText();
	insert( newTxt );

	setSelection( lF, iF, lT, iT );
};

void QsciEditor::changeFont() {

	bool ok;
	QFont newFont = QFontDialog::getFont( &ok, font(), this );

	if ( ok ) {
		setFont( newFont );
		update();
	}
};
