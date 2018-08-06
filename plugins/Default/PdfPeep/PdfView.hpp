/*
	*
	* PdfView - PdfView class header
	*
*/

#pragma once

#include <QtCore>
#include <QtGui>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#include "MuPdfDocument.hpp"

class PdfView : public QScrollArea {
	Q_OBJECT

	public:
		PdfView( QWidget *parent );

		void setPdfDocument( MuPdfDocument *Pdf );

		qreal zoom();
		void setZoom( qreal );

	private:
		MuPdfDocument *PdfDoc;
		QHash<int, QImage> renderedImages;
		QHash<int, QRectF> pageRects;

		int currentPage;
		qreal mZoom;

		void getCurrentPage();

		void reshapeView();

		bool isPageVisible( int pgNo );

	public Q_SLOTS:
		void slotZoomIn() {

			if ( mZoom >= 4.0 )
				return;

			setZoom( mZoom + 0.1 );
		};

		void slotZoomOut() {

			if ( mZoom <= 0.1 )
				return;

			setZoom( mZoom - 0.1 );
		};

	protected:
		void paintEvent( QPaintEvent *pEvent );

		void resizeEvent( QResizeEvent *rEvent );
		void wheelEvent( QWheelEvent *wEvent );
};
