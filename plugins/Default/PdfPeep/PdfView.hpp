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
		PdfView( QString path, QWidget *parent );

	private:
		MuPdfDocument *PdfDoc;
		QHash<int, QImage> renderedImages;
		QHash<int, QRectF> pageRects;

		int currentPage;
		qreal mZoom;

		void getCurrentPage();

		void reshapeView();

		bool isPageVisible( int pgNo );

	protected:
		void paintEvent( QPaintEvent *pEvent );

		void resizeEvent( QResizeEvent *rEvent );
		void wheelEvent( QWheelEvent *wEvent );

		void keyPressEvent( QKeyEvent *kEvent );

	Q_SIGNALS:
		void
};
