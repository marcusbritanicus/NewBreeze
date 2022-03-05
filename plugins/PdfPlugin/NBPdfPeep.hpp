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

#include <qdocumentview/QDocumentView.hpp>

class NBPdfPeep : public QDialog {
	Q_OBJECT

	public :
		NBPdfPeep( QString path = QDir::homePath() );
		QDocumentView *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

		QLabel *lbl;
		QString mPath;

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};
