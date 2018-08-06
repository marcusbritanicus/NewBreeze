/*
	*
	* PdfView - Pdf Document Viewer for PdfWidget
	*
*/

#include "PdfView.hpp"

PdfView::PdfView( QWidget *parent ) : QScrollArea( parent ) {

	setWidgetResizable( true );
	setAlignment( Qt::AlignCenter );
	setWidget( new QWidget() );

	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
	setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );

	setMinimumSize( QSize( 640, 480 ) );

	currentPage = -1;
	mZoom = 1.0;

	QAction *zoomInAct = new QAction( this );
	zoomInAct->setShortcut( QKeySequence( "Ctrl++" ) );
	connect( zoomInAct, SIGNAL( triggered() ), this, SLOT( slotZoomIn() ) );
	addAction( zoomInAct );

	QAction *zoomOutAct = new QAction( this );
	zoomOutAct->setShortcut( QKeySequence( "Ctrl+-" ) );
	connect( zoomOutAct, SIGNAL( triggered() ), this, SLOT( slotZoomOut() ) );
	addAction( zoomOutAct );
};

void PdfView::setPdfDocument( MuPdfDocument *Pdf ) {

	PdfDoc = Pdf;
	if ( Pdf->pageCount() > 0 )
		currentPage = 0;

	setWindowTitle( "PdfView - " + PdfDoc->pdfName() );

	reshapeView();
};

qreal PdfView::zoom() {

	return mZoom;
};

void PdfView::setZoom( qreal zoom ) {

	mZoom = zoom;
	PdfDoc->setZoom( zoom );

	renderedImages.clear();
	pageRects.clear();

	reshapeView();
	getCurrentPage();

	repaint();
};

void PdfView::getCurrentPage() {

	/* Fetch and store the rendering of the current page */
	if ( PdfDoc->pageCount() <= 0 )
		return;

	QRectF viewRect = QRectF( QPointF( 0, verticalScrollBar()->value() ), viewport()->size() );

	currentPage = 0;
	qreal area = 0;
	for( int pg = 0; pg < PdfDoc->pageCount(); pg++ ) {
		QRectF xRect = pageRects[ pg ].intersected( viewRect );
		qreal newArea = abs( xRect.width() *  xRect.height() );
		if ( newArea > area ) {
			currentPage = pg;
			area = newArea;
		}
	}

	/* Render the current page if not rendered already */
	if ( not renderedImages.keys().contains( currentPage ) )
		renderedImages[ currentPage ] = PdfDoc->renderPage( currentPage );

	for( int i = 1; i < 6; i++ ) {
		if ( currentPage + i < PdfDoc->pageCount() ) {
			if ( renderedImages.value( currentPage + i ).isNull() )
				renderedImages[ currentPage + i ] = PdfDoc->renderPage( currentPage + i );
		}

		if ( currentPage - i >= 0 ) {
			if ( renderedImages.value( currentPage - i ).isNull() )
				renderedImages[ currentPage - i ] = PdfDoc->renderPage( currentPage - i );
		}
	}
};

void PdfView::reshapeView() {

	pageRects.clear();
	renderedImages.clear();

	int minHeight = 10;
	int maxWidth = 0;

	verticalScrollBar()->setPageStep( height() / 4 * 3 );
	verticalScrollBar()->setSingleStep( 30 );

	for( int i = 0; i < PdfDoc->pageCount(); i++ ) {
		QSizeF pageSize = PdfDoc->pageSize( i );
		pageRects[ i ] = QRectF( QPointF( 0, minHeight ), pageSize );

		maxWidth = ( pageSize.width() > maxWidth ? pageSize.width() : maxWidth );

		minHeight += ( int )( pageSize.height() );
		minHeight += 5;
	}

	/* 20px border */
	widget()->setFixedSize( maxWidth + 20, minHeight );
	viewport()->update();

	if ( ( maxWidth + 20 ) < viewport()->width() )
		horizontalScrollBar()->hide();

	else
		horizontalScrollBar()->show();
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

	for( int pg = 0; pg < PdfDoc->pageCount(); pg++ )
		painter.drawImage( pageRects[ pg ], renderedImages[ pg ] );

	/* Draw the current page rect */
	painter.setPen( Qt::black );
	if ( pageRects[ currentPage ].y() + pageRects[ currentPage ].height() - h < height() / 2 )
		painter.drawRect( pageRects[ currentPage + 1 ] );

	else
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
