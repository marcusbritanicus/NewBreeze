/*
	*
	* PdfView - Pdf Document Viewer for PdfWidget
	*
*/

#include "PdfView.hpp"

PdfView::PdfView( QString path, QWidget *parent ) : QScrollArea( parent ) {

	setWidgetResizable( true );
	setAlignment( Qt::AlignCenter );
	setWidget( new QWidget() );

	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
	setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );

	setMinimumSize( QSize( 640, 480 ) );

	currentPage = -1;
	mZoom = 1.0;

	PdfDoc = new MuPdfDocument( path );
	PdfDoc->loadDocument();

	if ( PdfDoc->pageCount() > 0 )
		currentPage = 0;

	setWindowTitle( "PdfView - " + PdfDoc->pdfName() );

	reshapeView();
};

void PdfView::getCurrentPage() {

	/* Fetch and store the rendering of the current page */
	if ( PdfDoc->pageCount() <= 0 )
		return;

	QRectF viewRect = QRectF( QPointF( 0, verticalScrollBar()->value() ), viewport()->size() );

	currentPage = 0;

	/* Get the current page: maximum area => current page */
	qreal area = 0;
	for( int pg = 0; pg < PdfDoc->pageCount(); pg++ ) {
		QRectF xRect = pageRects[ pg ].intersected( viewRect );
		qreal newArea = abs( xRect.width() *  xRect.height() );
		if ( newArea > area ) {
			currentPage = pg;
			area = newArea;
		}
	}

	qreal width = viewport()->width() - style()->pixelMetric( QStyle::PM_ScrollBarExtent ) - 25;

	/* Render the current page if not rendered already */
	if ( renderedImages.value( currentPage ).isNull() )
		renderedImages[ currentPage ] = PdfDoc->renderPageForWidth( currentPage, width );

	/* Render the next page if not rendered */
	if ( currentPage + 1 < PdfDoc->pageCount() ) {
		if ( renderedImages.value( currentPage + 1 ).isNull() )
			renderedImages[ currentPage + 1 ] = PdfDoc->renderPageForWidth( currentPage + 1, width );
	}

	/* Render the previous page if not rendered */
	if ( currentPage - 1 >= 0 ) {
		if ( renderedImages.value( currentPage - 1 ).isNull() )
			renderedImages[ currentPage - 1 ] = PdfDoc->renderPageForWidth( currentPage - 1, width );
	}

	/* Render all other pages which are visible in the viewport */
	for( int i = 0; i < PdfDoc->pageCount(); i++ ) {
		if ( isPageVisible( i ) and renderedImages.value( i ).isNull() )
			renderedImages[ i ] = PdfDoc->renderPageForWidth( i, width );
	}
};

void PdfView::reshapeView() {

	pageRects.clear();
	renderedImages.clear();

	int minHeight = 10;
	int maxWidth = viewport()->width() - style()->pixelMetric( QStyle::PM_ScrollBarExtent ) - 25;

	verticalScrollBar()->setPageStep( height() / 4 * 3 );
	verticalScrollBar()->setSingleStep( 30 );

	for( int i = 0; i < PdfDoc->pageCount(); i++ ) {
		QSizeF pageSize = PdfDoc->pageSize( i );

		qreal height = 1.0 * pageSize.height() * maxWidth / pageSize.width();
		pageRects[ i ] = QRectF( QPointF( 0, minHeight ), QSizeF( maxWidth, height ) );

		minHeight += ( int )( height );
		minHeight += 5;
	}

	/* 20px border */
	widget()->setFixedSize( maxWidth + 20, minHeight );
	viewport()->update();

	horizontalScrollBar()->hide();
};

bool PdfView::isPageVisible( int pgNo ) {

	QRectF pageRect = pageRects[ pgNo ];
	QRectF viewRect = QRectF( QPointF( 0, verticalScrollBar()->value() ), viewport()->size() );

	return viewRect.intersects( pageRect );
};

void PdfView::paintEvent( QPaintEvent *pEvent ) {

	/* If the document is not loaded, return */
	if ( currentPage == -1 )
		return;

	/* Init the painter */
	QPainter painter( viewport() );

	/* Draw the background */
	painter.fillRect( rect(), Qt::lightGray );

	/* Get the current page */
	getCurrentPage();

	/* ScrollBar Positions */
	int h = verticalScrollBar()->value();

	/* Start drawing the current page from where it starts */
	painter.translate( widget()->x() + 10, -h );
	painter.drawImage( pageRects[ currentPage ], renderedImages[ currentPage ] );

	for( int pg = 0; pg < PdfDoc->pageCount(); pg++ ) {
		if ( isPageVisible( pg ) )
			painter.drawImage( pageRects[ pg ], renderedImages[ pg ] );
	}

	/* Draw the current page rect */
	painter.setPen( Qt::black );
	painter.drawRect( pageRects[ currentPage ] );

	painter.end();

	pEvent->accept();
};

void PdfView::resizeEvent( QResizeEvent *rEvent ) {

	rEvent->accept();
	reshapeView();
};

void PdfView::wheelEvent( QWheelEvent *wEvent ) {

	QScrollArea::wheelEvent( wEvent );

	 int cPos = verticalScrollBar()->value();

	 int numDegrees = -wEvent->delta() / 8;
	 verticalScrollBar()->setValue( cPos + numDegrees * verticalScrollBar()->singleStep() );

	 wEvent->accept();

	 viewport()->repaint();
};

void PdfView::keyPressEvent( QKeyEvent *kEvent ) {

	switch ( kEvent->key() ) {
		case Qt::Key_Right: {

			if ( currentPage + 1 < PdfDoc->pageCount() )
				verticalScrollBar()->setValue( pageRects.value( currentPage + 1 ).top() - 5 );

				break;
		};

		case Qt::Key_Left: {

			if ( currentPage - 1 >= 0 )
				verticalScrollBar()->setValue( pageRects.value( currentPage - 1 ).top() - 5 );

			break;
		};

		case Qt::Key_Home: {

			verticalScrollBar()->setValue( 0 );

			break;
		};

		case Qt::Key_End: {

			verticalScrollBar()->setValue( widget()->height() );

			break;
		};

		default: {

			QScrollArea::keyPressEvent( kEvent );
			break;
		};
	}

	viewport()->repaint();
	kEvent->accept();
};
