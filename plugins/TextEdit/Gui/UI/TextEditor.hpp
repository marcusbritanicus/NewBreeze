/*
	*
	* TextEditor.hpp - TextEditor Header
	*
*/

#pragma once

#include <Global.hpp>

#include <Editor.hpp>
#include <GuiWidgets.hpp>
#include <NBWindowButton.hpp>

class TextEditor : public QMainWindow {
	Q_OBJECT

	public :
		TextEditor( QString filename, QWidget *parent );

	private :
		void setWindowProperties();
		void setupGui();

		void buildStatusBar();
		void buildToolBar();

		void setupConnections();

		QVBoxLayout *BaseLayout;

		QLabel *lineCountLbl;
		QLabel *cursorPosLbl;
		QLabel *charCountLbl;
		QLabel *selectionLbl;

		LedLight *safetyLed;

		QString filename;
		Editor *ed;

		QToolButton *saveBtn;
		QToolButton *printBtn;
		QToolButton *propsBtn;
		QToolButton *cutBtn;
		QToolButton *copyBtn;
		QToolButton *pasteBtn;
		QToolButton *selectBtn;
		QToolButton *undoBtn;
		QToolButton *redoBtn;
		QWidget *toolBar;

	private slots :
		// File Menu Actions
		void loadFile();

		void saveFile();

		void printFile();
		void fileProps();

		void changeFont( QFont );

		void updateModStatus( bool );
		void updateStatusBar();
		void updateToolBar();

		void handleAutoSaved();

		void toggleNormal();

	protected :
		void resizeEvent( QResizeEvent * );
		void closeEvent( QCloseEvent * );
};
