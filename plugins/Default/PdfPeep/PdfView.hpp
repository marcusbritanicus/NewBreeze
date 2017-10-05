/*
	*
	* PdfView - PdfView class header
	*
*/

#pragma once

#include "Global.hpp"
#include "PdfDocument.hpp"

class PdfView : public QAbstractScrollArea {
	Q_OBJECT

	public:
		PdfView( QWidget *parent );
		PdfView( QString, QWidget *parent );

		void setPdfDocument( PdfDocument *Pdf );
		void load( QString pdfPath );
		QString pageText( int );

	private:
		PdfDocument *PdfDoc;
		QHash<int, QImage> renderedImages;
		QHash<int, QRect> pageRects;

		int currentPage;
		QScrollBar *vScroll;

		void getCurrentPage();
		void lookAround();

		void reshapeView();
		float getResolution( int );

		void paintPage( int );
		void paintRect( int );

	protected:
		void paintEvent( QPaintEvent *pEvent );

		void resizeEvent( QResizeEvent *rEvent );
		void wheelEvent( QWheelEvent *wEvent );
};

/*
class PageRenderer : public QThread {
	Q_OBJECT

	public:
		PageRenderer( QWidget *parent );

		void render( PdfPages pages );

	protected:
		void run();

	private:
		PdfPages pageList;
};
*/
