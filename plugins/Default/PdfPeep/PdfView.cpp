/*
	*
	* PdfView - Pdf Document Viewer for PdfWidget
	*
*/

#include "PdfView.hpp"

PdfView::PdfView( QWidget *parent ) : QAbstractScrollArea( parent ) {

	vScroll = new QScrollBar( Qt::Vertical, this );
	vScroll->setRange( 0, 0 );
	vScroll->setFixedWidth( 12 );
	vScroll->setPageStep( height() / 4 * 3 );
	vScroll->setSingleStep( 30 );

	setVerticalScrollBar( vScroll );

	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

	connect( vScroll, SIGNAL( valueChanged( int ) ), this, SLOT( repaint() ) );

	setMinimumSize( QSize( 640, 480 ) );

	currentPage = -1;
};

PdfView::PdfView( QString pdfPath, QWidget *parent ) : QAbstractScrollArea( parent ) {

	vScroll = new QScrollBar( Qt::Vertical, this );
	vScroll->setRange( 0, 0 );
	vScroll->setFixedWidth( 12 );

	setVerticalScrollBar( vScroll );

	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

	connect( vScroll, SIGNAL( valueChanged( int ) ), this, SLOT( repaint() ) );

	setMinimumSize( QSize( 640, 480 ) );

	setPdfDocument( new PdfDocument( pdfPath ) );
};

void PdfView::setPdfDocument( PdfDocument *Pdf ) {

	PdfDoc = Pdf;
	if ( Pdf->pages() > 0 )
		currentPage = 0;

	reshapeView();

	lookAround();
};

void PdfView::load( QString pdfPath ) {

	setPdfDocument( new PdfDocument( pdfPath ) );
};

QString PdfView::pageText( int pageNo ) {

	return PdfDoc->page( pageNo )->text( QRectF() );
};

void PdfView::reshapeView() {

	pageRects.clear();
	renderedImages.clear();

	int minHeight = 10;
	int viewWidth = viewport()->width() - vScroll->width();

	vScroll->setPageStep( height() / 4 * 3 );
	vScroll->setSingleStep( 30 );

	for( int i = 0; i < PdfDoc->pages(); i++ ) {
		QSize pageSize = PdfDoc->page( i )->pageSize();
		pageRects[ i ] = QRect( 0, minHeight, viewWidth, 1.0 * viewWidth * pageSize.height() / pageSize.width() );

		minHeight += ( int )( 1.0 * viewWidth * pageSize.height() / pageSize.width() );
		minHeight += 5;
	}

	viewport()->setFixedHeight( minHeight );
	vScroll->setMaximum( minHeight );

	repaint();
};

float PdfView::getResolution( int pageNo ) {

	int viewWidth = viewport()->width() - vScroll->width();
	return viewWidth * 72 / PdfDoc->page( pageNo )->pageSize().width();
};

void PdfView::getCurrentPage() {
	/* Fetch and store the rendering of the current page */

	if ( PdfDoc->pages() <= 0 )
		return;

	int viewWidth = viewport()->width() - vScroll->width();
	int h = vScroll->value();

	/* Set the current page */
	for( int i = 0; i < PdfDoc->pages(); i++ ) {
		if ( pageRects[ i ].contains( QPoint( viewWidth / 2, h ) ) ) {
			currentPage = i;
			break;
		}
	}

	/* Render the current page if not rendered already */
	float pageRes = getResolution( currentPage );
	if ( not renderedImages.keys().contains( currentPage ) )
		renderedImages[ currentPage ] = PdfDoc->page( currentPage )->renderToImage( pageRes, pageRes );

	lookAround();
};

void PdfView::lookAround() {
	/* We will be rendering 5 pages before and after the current page */

	if ( currentPage == -1 )
		return;

	PdfPages renderList;
	int i = 1;
	while ( i <= 5 ) {
		if ( currentPage + i < PdfDoc->pages() ) {
			float pageRes = getResolution( currentPage + i );
			if ( not renderedImages.keys().contains( currentPage + i ) )
				renderedImages[ currentPage + i ] = PdfDoc->page( currentPage + i )->renderToImage( pageRes, pageRes );
		}

		if ( currentPage - i > 0 ) {
			float pageRes = getResolution( currentPage - i );
			if ( not renderedImages.keys().contains( currentPage - i ) )
				renderedImages[ currentPage - i ] = PdfDoc->page( currentPage - i )->renderToImage( pageRes, pageRes );
		}

		i++;
	}
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
	/* Vertical Scroll Bar Position */
	int h = vScroll->value();

	/* Start drawing the current page from where it starts */
	painter.translate( 5, -h );
	painter.drawImage( pageRects[ currentPage ], renderedImages[ currentPage ] );

	/* If the  */
	if ( pageRects[ currentPage ].y() + height() < h + pageRects[ currentPage ].height() )
		painter.drawImage( pageRects[ currentPage + 1 ], renderedImages[ currentPage + 1 ] );

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

	int cPos = vScroll->value();

	int numDegrees = -wEvent->delta() / 8;
	vScroll->setValue( cPos + numDegrees * vScroll->singleStep() );

	wEvent->accept();

	viewport()->repaint();
};
