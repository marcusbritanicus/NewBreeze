/*
	*
	* QsciEditor - TextEditor Core based on QScintilla
	*
*/

#pragma once

#include "Global.hpp"
#include "SR.hpp"
#include "QsciLexer.hpp"
#include "QsciLexerLaTeX.hpp"
#include <Qsci/qsciscintilla.h>

class QsciEditor : public QsciScintilla {
	Q_OBJECT

	public:
		QsciEditor( QWidget *parent = 0 );

		bool loadFile( QString filename );
		bool saveFile( QString filename = QString() );
		bool reloadFile();

		qint64 lineCount();
		qint64 wordCount();
		qint64 charCount();

		int position();
		void setPosition( int );

		QString fileName();
		void setFileName( QString );

	private Q_SLOTS:
		void resizeMargins();
		void showCustomMenu( const QPoint& );

		void showSearch();

		void cutAction();
		void copyAction();
		void pasteAction();

		void deleteAction();

		void selectAction();
		void deselectAction();

		void undoAction();
		void redoAction();

		void toggleCase();
		void changeFont();

	private :
		void setupActions();
		void setAutoLexer();

		QAction *cutAct, *copyAct, *pasteAct, *deleteAct, *selectAct;
		QAction *deselectAct, *undoAct, *redoAct, *searchAct, *replaceAct;
		QAction *changeFontAct, *toggleCaseAct;
		QMenu contextMenu;
		QClipboard *clipboard;
		QString prevSearch, prevReplace;

		QString mFilePath;
		QBasicTimer *autoSaveTimer;

	protected:
		void timerEvent( QTimerEvent * );

	Q_SIGNALS:
		void autoSaved();

		void undoAvailable( bool );
		void redoAvailable( bool );
};

/*

		void autoFindAction( QString text, bool regex, bool wcase, bool whole );
		void findAction( QString text, bool regex, bool wcase, bool whole, bool forward );
		void replaceAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex, bool forward );
		void replaceAndFindAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex, bool forward );
		void replaceInDocumentAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex );
		void replaceInSelectionAction( QString ftext, QString rtext, bool wcase, bool whole, bool regex );

		void updateTextActions();
		void updateSelectionActions();
		void updateClipboardActions();
		void showCustomMenu( const QPoint& );

*/
