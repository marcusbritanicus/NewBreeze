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

#include "View.hpp"

class NBPdfPeep : public QDialog {
	Q_OBJECT

	public :
		NBPdfPeep( QString path = QDir::homePath() );
		DesQDocs::View *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

		QLabel *lbl;
		QString mPath;

	private slots :
		void openInExternal();
		void loadDocument();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};
