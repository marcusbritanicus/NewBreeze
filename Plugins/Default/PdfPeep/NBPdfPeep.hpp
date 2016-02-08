/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#ifndef NBPDFPEEP_HPP
#define NBPDFPEEP_HPP

#include <Global.hpp>
#include <poppler-qt4.h>

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

	private slots :
		void openInExternal();
		void loadPdf();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

#endif
