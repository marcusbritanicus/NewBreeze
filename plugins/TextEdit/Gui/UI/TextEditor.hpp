/*
	*
	* TextEditor.hpp - TextEditor Header
	*
*/

#pragma once

#include <Global.hpp>

#include <QsciEditor.hpp>
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
		QsciEditor *ed;

		bool preview;

		QToolButton *saveBtn;
		QToolButton *printBtn;
		QToolButton *propsBtn;
		QToolButton *cutBtn;
		QToolButton *copyBtn;
		QToolButton *pasteBtn;
		QToolButton *selectBtn;
		QToolButton *undoBtn;
		QToolButton *redoBtn;
		QToolButton *editBtn;
		QToolButton *openBtn;
		QWidget *toolBar;

		QShortcut *closeSC;

	public slots:
		void setPreviewMode();
		void switchToEditMode();

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

		void openInExternal();

	protected :
		void resizeEvent( QResizeEvent * );
		void paintEvent( QPaintEvent * );
		void closeEvent( QCloseEvent * );
};
