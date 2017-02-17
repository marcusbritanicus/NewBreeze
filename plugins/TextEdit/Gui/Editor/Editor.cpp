/*
    *
    * Editor.cpp - TextEditor's Text Editor Class
    *
*/

#include <Editor.hpp>

Editor::Editor( QWidget *parent ) : QPlainTextEdit( parent ) {

	// Line Numbering space
	lineNumberWidth = 20;
	setViewportMargins( QMargins( lineNumberWidth, 0, 0, 0 ) );

	// Line Number Widget
	lineNumberWidget = new LineNumberWidget( this );

	// Basic Settings
	setFont( QFont( "Envy Code R", 9 ) );
	setTabStopWidth( 4 * font().pointSize() );
	setTabChangesFocus( false );

	// Font Metrics
	fm = new QFontMetrics( font() );

	// Remove Horizontal Scrollbar, put a Vertical one;
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

	// Scroll past the last line by one page
	setCenterOnScroll( true );

	// Enable word wrap;
	setLineWrapMode( QPlainTextEdit::WidgetWidth );
	setWordWrapMode( QTextOption::WordWrap );

	// Enable and Show Custom context Menu
	setContextMenuPolicy( Qt::CustomContextMenu );

	// Minimum Size
	setMinimumSize( QSize( 640, 480 ) );

	// No modification now
	setModified( false );

	// Setup the clipboard
	clipboard = QApplication::clipboard();

	// Search and Replace Widgets
	searchWidget = new SearchWidget( this );
	searchWidget->hide();

	// Setup various actions and connections
	setupActions();
	setupConnections();

	// Update Available Actions
	updateTextActions();
	updateSelectionActions();
	updateClipboardActions();

	// Auto Save Timer: 60 sec
	autoSaveTimer = new QBasicTimer();

	// Modification changed signal
	connect( document(), SIGNAL( modificationChanged( bool ) ), this, SIGNAL( modificationChanged( bool ) ) );

	#ifdef HAVE_GNU_SYNTAX_HILITE
		// Syntax Highlighting
		syntaxer = new Qt4SyntaxHighlighter( document() );
		syntaxer->init( "nohilite.lang" );
	#endif
};

void Editor::open( QString fn ) {

	filename = fn;
	QFile *file = new QFile( fn );
	if ( file->open( QIODevice::ReadOnly ) )
		setPlainText( QString::fromLocal8Bit( file->readAll() ) );

	setModified( false );
	file->close();

	#ifdef HAVE_GNU_SYNTAX_HILITE
		syntaxer->setDocument( document() );
		syntaxer->initFromFileName( filename );
	#endif

	autoSaveTimer->start( 60000, this );

	updateLineNumberWidth( 0 );
};

void Editor::save() {

	if ( autoSaveTimer->isActive() )
		autoSaveTimer->stop();

	QFile *file = new QFile( filename );

	if ( file->open( QIODevice::WriteOnly ) ) {
		file->write( text().toLocal8Bit() );
		file->close();
		document()->setModified( false );
	}

	else {
		qCritical() << "Write Error";
		QMessageBox::critical( this, "Write Error", "Unable to open file for writing." );
	}

	autoSaveTimer->start( 60000, this );
};

qint64 Editor::lineCount() {

	return document()->lineCount();
}

qint64 Editor::wordCount() {

	return toPlainText().split( QRegExp( "\\s", Qt::CaseInsensitive ), QString::SkipEmptyParts ).count();
}

qint64 Editor::charCount() {

	return text().length();
}

int Editor::position() {

	return textCursor().position();
}

void Editor::setPosition( int pos ) {

	QTextCursor cur = textCursor();
	cur.setPosition( pos );

	setTextCursor( cur );
}

bool Editor::isModified() {

	return document()->isModified();
};

void Editor::setModified( bool truth ) {

	return document()->setModified( truth );
};

QString Editor::fileName() {

	return filename;
};

void Editor::setFileName( QString fn) {

	filename = QString( fn );
};

QString Editor::text() {

	return toPlainText();
};

qint64 Editor::lineNumberWidgetWidth() {

	return lineNumberWidth;
};

void Editor::setupActions() {

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
};

void Editor::setupConnections() {

	connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showCustomMenu( const QPoint& ) ) );

    connect( this, SIGNAL( blockCountChanged( int ) ), this, SLOT( updateLineNumberWidth( int ) ) );
    connect( this, SIGNAL( updateRequest( QRect, int ) ), this, SLOT( updateLineNumberWidget( QRect, int ) ) );
    connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT( highlightCurrentLine() ) );

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

	connect( searchAct, SIGNAL( triggered() ), this, SLOT( toggleSearchVisibility() ) );
	connect( searchWidget, SIGNAL( hidden() ), this, SLOT( setFocus() ) );

	// Actions updated
	connect( this, SIGNAL( textChanged() ), this, SLOT( updateTextActions() ) );
	connect( this, SIGNAL( selectionChanged() ), this, SLOT( updateSelectionActions() ) );
	connect( clipboard, SIGNAL( dataChanged() ), this, SLOT( updateClipboardActions() ) );

	connect( searchWidget, SIGNAL( find( QString, bool, bool, bool, bool ) ),
		this, SLOT( findAction( QString, bool, bool, bool, bool ) ) );

	connect( searchWidget, SIGNAL( autofind( QString, bool, bool, bool ) ),
		this, SLOT( autoFindAction( QString, bool, bool, bool ) ) );

	/*
		* ===================================================================================================== *
		*	connect( replaceWidget, SIGNAL( replace( QString, QString, bool, bool, bool, bool ) ),				*
		*		this, SLOT( replaceAction( QString, QString, bool, bool, bool, bool ) ) );						*
        *																										*
		*	connect( replaceWidget, SIGNAL( replaceAndFind( QString, QString, bool, bool, bool, bool ) ),		*
		*		this, SLOT( replaceAndFindAction( QString, QString, bool, bool, bool, bool ) ) );				*
        *																										*
		*	connect( replaceWidget, SIGNAL( replaceInDoc( QString, QString, bool, bool, bool ) ),				*
		*		this, SLOT( replaceInDocumentAction( QString, QString, bool, bool, bool ) ) );					*
        *																										*
		*	connect( replaceWidget, SIGNAL( replaceInSel( QString, QString, bool, bool, bool, bool ) ),			*
		*		this, SLOT( replaceInSelectionAction( QString, QString, bool, bool, bool ) ) );					*
		* ===================================================================================================== *
	*/
};

bool Editor::isUpper( QString str ) {

	bool upper = true;
	foreach( QChar ch, str )
		upper &= ch.isUpper();

	return upper;
};

bool Editor::isLower( QString str ) {

	bool lower = true;
	foreach( QChar ch, str )
		lower &= ch.isLower();

	return lower;
};

void Editor::cutAction() {

	if ( textCursor().hasSelection() ) {
		textCursor().removeSelectedText();
		clipboard->setText( textCursor().selectedText() );
	};
};

void Editor::copyAction() {

	if ( textCursor().hasSelection() ) {
		clipboard->setText( textCursor().selectedText() );
	};
};

void Editor::pasteAction() {

	textCursor().beginEditBlock();
	if ( textCursor().hasSelection() )
		textCursor().removeSelectedText();

	paste();
	textCursor().endEditBlock();
};

void Editor::deleteAction() {

	if ( textCursor().hasSelection() )
		textCursor().removeSelectedText();
};

void Editor::selectAction() {

	selectAll();
};

void Editor::deselectAction() {

	QTextCursor tc = textCursor();
	tc.clearSelection();
	setTextCursor( tc );
};

void Editor::undoAction() {

	if ( document()->isUndoAvailable() )
		undo();
};

void Editor::redoAction() {

	if ( document()->isRedoAvailable() )
		redo();
};

void Editor::toggleCase() {

	QTextCursor cur = textCursor();
	if ( !cur.hasSelection() )
		return;

	textCursor().beginEditBlock();

	int selStart = cur.selectionStart();
	int selEnd = cur.selectionEnd();

	QString txt = cur.selectedText();
	cur.removeSelectedText();

	if ( isLower( txt ) )
		cur.insertText( txt.toUpper() );

	else
		cur.insertText( txt.toLower() );

	cur.setPosition( selStart );
	cur.setPosition( selEnd, QTextCursor::KeepAnchor );

	setTextCursor( cur );

	textCursor().endEditBlock();
};

void Editor::toggleSearchVisibility() {

	if ( searchWidget->isVisible() ) {
		if ( searchWidget->hasFocus() ) {
			searchWidget->hide();
			setFocus();
		}

		else {
			searchWidget->setFocus();
		}
	}

	else {
		searchWidget->move( width() - searchWidget->width(), 0 );
		searchWidget->show();
		searchWidget->setFocus();
	}
};

void Editor::autoFindAction( QString text, bool regex, bool wcase, bool whole ) {

	/* Reset the cursor to the original state */
	QTextCursor txtCursor = textCursor();
	txtCursor.setPosition( txtCursor.selectionStart() );
	setTextCursor( txtCursor );

	if ( regex ) {
		QRegExp rx( ( whole ? "\\b" + text + "\\b" : text ) );
		rx.setCaseSensitivity( ( wcase ? Qt::CaseSensitive : Qt::CaseInsensitive ) );

		txtCursor = document()->find( rx, textCursor() );
	}

	else {
		QTextDocument::FindFlags flags;
		if ( wcase )
			flags |= QTextDocument::FindCaseSensitively;

		if ( whole )
			flags |= QTextDocument::FindWholeWords;

		txtCursor = document()->find( text, textCursor(), flags );
	}

	if ( not txtCursor.isNull() )
		setTextCursor( txtCursor );
};

void Editor::findAction( QString text, bool regex, bool wcase, bool whole, bool forward ){

	QTextCursor txtCursor;
	if ( forward ) {
		if ( regex ) {
			QRegExp rx( ( whole ? "\\b" + text + "\\b" : text ) );
			rx.setCaseSensitivity( ( wcase ? Qt::CaseSensitive : Qt::CaseInsensitive ) );

			txtCursor = document()->find( rx, textCursor() );
		}

		else {
			QTextDocument::FindFlags flags;
			if ( wcase )
				flags |= QTextDocument::FindCaseSensitively;

			if ( whole )
				flags |= QTextDocument::FindWholeWords;

			txtCursor = document()->find( text, textCursor(), flags );
		}
	}

	else {
		if ( regex ) {
			QRegExp rx( ( whole ? "\\b" + text + "\\b" : text ) );
			rx.setCaseSensitivity( ( wcase ? Qt::CaseSensitive : Qt::CaseInsensitive ) );

			txtCursor = document()->find( rx, textCursor(), QTextDocument::FindBackward );
		}

		else {
			QTextDocument::FindFlags flags = QTextDocument::FindBackward;
			if ( wcase )
				flags |= QTextDocument::FindCaseSensitively;

			if ( whole )
				flags |= QTextDocument::FindWholeWords;

			txtCursor = document()->find( text, textCursor(), flags );
		}
	}

	if ( not txtCursor.isNull() )
		setTextCursor( txtCursor );
};

void Editor::replaceAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex, bool forward ) {
	Q_UNUSED( ftext );
	Q_UNUSED( rtext );
	Q_UNUSED( wcase );
	Q_UNUSED( whole );
	Q_UNUSED( regex );
	Q_UNUSED( forward );
};

void Editor::replaceAndFindAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex, bool forward ){
	Q_UNUSED( ftext );
	Q_UNUSED( rtext );
	Q_UNUSED( wcase );
	Q_UNUSED( whole );
	Q_UNUSED( regex );
	Q_UNUSED( forward );
};

void Editor::replaceInDocumentAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex ){
	Q_UNUSED( ftext );
	Q_UNUSED( rtext );
	Q_UNUSED( wcase );
	Q_UNUSED( whole );
	Q_UNUSED( regex );
};

void Editor::replaceInSelectionAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex ){
	Q_UNUSED( ftext );
	Q_UNUSED( rtext );
	Q_UNUSED( wcase );
	Q_UNUSED( whole );
	Q_UNUSED( regex );
};

void Editor::updateTextActions() {

	if ( !text().isEmpty() ) {
		selectAct->setEnabled( true );
		searchAct->setEnabled( true );
		replaceAct->setEnabled( true );
	}

	else {
		selectAct->setDisabled( true );
		searchAct->setDisabled( true );
		replaceAct->setDisabled( true );
	}

	if ( document()->isUndoAvailable() )
		undoAct->setEnabled( true );

	else
		undoAct->setDisabled( true );

	if ( document()->isRedoAvailable() )
		redoAct->setEnabled( true );

	else
		redoAct->setDisabled( true );
};

void Editor::updateClipboardActions() {

	if ( !clipboard->text().isEmpty() )
		pasteAct->setEnabled( true );

	else
		pasteAct->setDisabled( true );
};

void Editor::updateSelectionActions() {

	if ( textCursor().hasSelection() ) {
		cutAct->setEnabled( true );
		copyAct->setEnabled( true );
		deleteAct->setEnabled( true );
		deselectAct->setEnabled( true );
	}

	else {
		cutAct->setDisabled( true );
		copyAct->setDisabled( true );
		deleteAct->setDisabled( true );
		deselectAct->setDisabled( true );
	}
};

void Editor::changeFont() {

	bool mStatus = document()->isModified();

	bool ok;
	QFont newFont = QFontDialog::getFont( &ok, font(), this );

	if ( ok ) {
		setFont( newFont );
		update();
	}

	if ( !mStatus )
		setModified( false );
};

void Editor::showCustomMenu( const QPoint &position ) {

	contextMenu.clear();

	QMenu formatMenu( "&Format" );

	formatMenu.addAction( changeFontAct );
	if ( textCursor().hasSelection() )
		formatMenu.addAction( toggleCaseAct );

	contextMenu.addMenu( &formatMenu );
	contextMenu.addSeparator();

	if ( textCursor().hasSelection() ) {
		contextMenu.addAction( cutAct );
		contextMenu.addAction( copyAct );
	}

	if ( !clipboard->text().isEmpty() )
		contextMenu.addAction( pasteAct );

	if ( textCursor().hasSelection() )
		contextMenu.addAction( deleteAct );

	if ( !text().isEmpty() or textCursor().hasSelection() )
		contextMenu.addSeparator();

	if ( !text().isEmpty() )
		contextMenu.addAction( selectAct );

	if ( textCursor().hasSelection() )
		contextMenu.addAction( deselectAct );

	if ( document()->isUndoAvailable() or document()->isRedoAvailable() )
		contextMenu.addSeparator();

	if ( document()->isUndoAvailable() )
		contextMenu.addAction( undoAct );

	if ( document()->isRedoAvailable() )
		contextMenu.addAction( redoAct );

	if ( !text().isEmpty() ) {
		contextMenu.addSeparator();
		contextMenu.addAction( searchAct );
		contextMenu.addAction( replaceAct );
	}

	contextMenu.exec( QWidget::mapToGlobal( position ) );
};

void Editor::updateLineNumberWidth( int newBlockCount ) {

	if ( newBlockCount == 0 )
		newBlockCount = blockCount();

	/* We need buffer space of one digit on the left and one on the right */
    int digits = 3;
    int max = qMax( 1, newBlockCount );
    while ( max >= 10 ) {

        max /= 10;
        ++digits;
    }

    lineNumberWidth = fm->width( QLatin1Char( 'm' ) ) * digits;

    setViewportMargins( lineNumberWidth, 0, 0, 0 );
}

void Editor::updateLineNumberWidget( const QRect &rect, int dy ) {

    if ( dy )
        lineNumberWidget->scroll( 0, dy );

    else
        lineNumberWidget->update( 0, rect.y(), lineNumberWidget->width(), rect.height() );

    if ( rect.contains( viewport()->rect() ) )
        updateLineNumberWidth( 0 );
}

void Editor::highlightCurrentLine() {

    QList<QTextEdit::ExtraSelection> extraSelections;

	QTextEdit::ExtraSelection selection;

	QColor lineColor = palette().color( QPalette::Base ).darker( 120 );

	selection.format.setBackground( lineColor );
	selection.format.setProperty( QTextFormat::FullWidthSelection, true );
	selection.cursor = textCursor();
	selection.cursor.clearSelection();
	extraSelections.append( selection );

    setExtraSelections( extraSelections );
}

void Editor::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == autoSaveTimer->timerId() ) {
		autoSaveTimer->stop();
		if ( not document()->isModified() ) {
			autoSaveTimer->start( 60000, this );
			tEvent->accept();
		}

		else {
			/* If we have no name for the file or if its not modified, return */
			if ( filename.isEmpty() or not isModified() )
				return;

			save();
			emit autoSaved();
			tEvent->accept();
		}
	}
};

void Editor::changeEvent( QEvent *event ) {

	QPlainTextEdit::changeEvent( event );
};

void Editor::lineNumberPaintEvent( QPaintEvent *pEvent ) {

    QPainter painter( lineNumberWidget );
    painter.fillRect( pEvent->rect(), Qt::lightGray );
    painter.setFont( font() );

	updateLineNumberWidth( 0 );

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = ( int ) blockBoundingGeometry( block ).translated( contentOffset() ).top();
    int bottom = top + ( int ) blockBoundingRect( block ).height();

    while ( block.isValid() && top <= pEvent->rect().bottom() ) {

        if ( block.isVisible() && bottom >= pEvent->rect().top() ) {
            QString number = QString::number( blockNumber + 1 );
            painter.setPen( Qt::black );
            painter.drawText( 0, top, lineNumberWidget->width() - fm->width( "0" ), fm->height(), Qt::AlignRight, number );
        }

        block = block.next();
        top = bottom;
        bottom = top + ( int ) blockBoundingRect( block ).height();
        ++blockNumber;
    }

    pEvent->accept();
};

void Editor::resizeEvent( QResizeEvent *rEvent ) {

    QPlainTextEdit::resizeEvent( rEvent );

    QRect cr = contentsRect();
    lineNumberWidget->setGeometry( QRect( cr.left(), cr.top(), lineNumberWidgetWidth(), cr.height() ) );
}
