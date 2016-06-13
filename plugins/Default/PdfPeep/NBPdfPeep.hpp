/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"
#if QT_VERSION >= 0x050000
	#include <poppler-qt5.h>
#else
	#include <poppler-qt4.h>
#endif

class NBPdfPeep : public QDialog {
	Q_OBJECT

	public :
		NBPdfPeep( QString path = QDir::homePath() );
		QString path;
		QScrollArea *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

		QVBoxLayout *pdfLyt;
		QLabel *lbl;

	public slots:
		int exec();

	private slots :
		void openInExternal();
		void loadPdf();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};
