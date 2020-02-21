/*
	*
	* NBPdfPeep.hpp - NewBreeze PdfPeep Class Header
	*
*/

#pragma once

#include "Global.hpp"

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#include "qpdfview.h"

class NBPdfPeep : public QDialog {
	Q_OBJECT

	public :
		NBPdfPeep( QString path = QDir::homePath() );
		QPdfView *peekWidgetBase;

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
