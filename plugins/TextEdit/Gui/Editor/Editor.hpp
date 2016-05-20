/*
	*
	* Editor.hpp - Editor.cpp Header for TextEditor
	*
*/

#pragma once

#include <Global.hpp>
#include <Qt4SyntaxHighlighter.h>
#include <SR.hpp>

class LineNumberWidget;

class Editor : public QPlainTextEdit {
	Q_OBJECT

	friend class LineNumberWidget;

	public:
		Editor( QWidget *parent = 0 );

		void open( QString fn );
		void save();

		qint64 lineCount();
		qint64 wordCount();
		qint64 charCount();

		int position();
		void setPosition( int );

		QString fileName();
		void setFileName( QString );

		QString text();

		qint64 lineNumberWidgetWidth();

	private :
		void setupActions();
		void setupConnections();

		bool isUpper( QString );
		bool isLower( QString );

		QAction *cutAct, *copyAct, *pasteAct, *deleteAct, *selectAct;
		QAction *deselectAct, *undoAct, *redoAct, *searchAct, *replaceAct;
		QAction *changeFontAct, *toggleCaseAct;
		QMenu contextMenu;
		QClipboard *clipboard;
		QString prevSearch, prevReplace;

		QString filename;

		SearchWidget *searchWidget;

		QBasicTimer *autoSaveTimer;
		Qt4SyntaxHighlighter *syntaxer;

		quint64 lineNumberWidth;
		LineNumberWidget *lineNumberWidget;

		QFontMetrics *fm;

	public slots:
		bool isModified();
		void setModified( bool );

	private slots:
		// Edit Menu Actions
		void cutAction();
		void copyAction();
		void pasteAction();

		void deleteAction();

		void selectAction();
		void deselectAction();

		void undoAction();
		void redoAction();

		void toggleCase();
		void toggleSearchVisibility();

		void autoFindAction( QString text, bool regex, bool wcase, bool whole );
		void findAction( QString text, bool regex, bool wcase, bool whole, bool forward );
		void replaceAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex, bool forward );
		void replaceAndFindAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex, bool forward );
		void replaceInDocumentAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex );
		void replaceInSelectionAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex );

		void updateTextActions();
		void updateSelectionActions();
		void updateClipboardActions();
		void changeFont();
		void showCustomMenu( const QPoint& );

		void updateLineNumberWidth( int newBlockCount );
		void highlightCurrentLine();
		void updateLineNumberWidget( const QRect &, int );

	protected:
		void timerEvent( QTimerEvent * );
		void changeEvent( QEvent * );
		void lineNumberPaintEvent( QPaintEvent * );
		void resizeEvent( QResizeEvent * );

	Q_SIGNALS:
		void modificationChanged( bool );
		void autoSaved();
};

class LineNumberWidget : public QWidget {
	Q_OBJECT

	friend class Editor;

	public:
		LineNumberWidget( Editor *editor) : QWidget( editor ) {

			textEditor = editor;
		}

		QSize sizeHint() const {

			return QSize( textEditor->lineNumberWidgetWidth(), 0 );
		}

	protected:
		void paintEvent( QPaintEvent *pEvent ) {

			textEditor->lineNumberPaintEvent( pEvent );
		}

	private:
		Editor *textEditor;
};
