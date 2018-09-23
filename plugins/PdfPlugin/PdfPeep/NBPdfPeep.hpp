/*
	*
	* NBPdfPeep.hpp - NewBreeze PdfPeep Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "PdfView.hpp"

class NBPdfPeep : public QDialog {
	Q_OBJECT

	public :
		NBPdfPeep( QString path = QDir::homePath() );
		PdfView *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

		QVBoxLayout *pdfLyt;
		QLabel *lbl;

		QString path;

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};
