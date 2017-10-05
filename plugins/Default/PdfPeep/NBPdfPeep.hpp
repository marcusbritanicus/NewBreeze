/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "PdfView.hpp"

class NBPdfPeep : public QDialog {
	Q_OBJECT

	public :
		NBPdfPeep( QString path = QDir::homePath() );
		QString path;
		PdfView *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

		QVBoxLayout *pdfLyt;
		QLabel *lbl;

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};
