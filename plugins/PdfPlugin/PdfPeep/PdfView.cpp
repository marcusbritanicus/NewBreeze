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

	PdfDoc = 0;
	currentPage = -1;
	mZoom = 1.0;

	mViewMode = SinglePageView;
	mLytMode = Continuous;
};

void PdfView::loadPdfDocument( QString pdfPath, RenderBackend rbknd ) {

	PdfDoc = qobject_cast<PdfDocument *>( new PopplerDocument( pdfPath ) );
	PdfDoc->loadDocument();

	if ( PdfDoc->pageCount() > 0 )
		currentPage = 0;

	connect( verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( renderPages() ) );

	reshapeView();
};

qreal PdfView::zoom() {

	return mZoom;
};

void PdfView::setZoom( qreal zoom ) {

	if ( not PdfDoc or not PdfDoc->isReady() )
		return;

	mZoom = zoom;
	PdfDoc->setZoom( zoom );

	pages.clear();
	renderedImages.clear();

	reshapeView();

	viewport()->update();
};

void PdfView::renderPages() {

	if ( not PdfDoc or not PdfDoc->isReady() )
		return;

	/* Fetch and store the rendering of the current page */
	if ( PdfDoc->pageCount() <= 0 )
		return;

	visiblePages.clear();

	/* Get the current view rect */
	QRectF viewRect = QRectF( QPoint( 0, verticalScrollBar()->value() ), viewport()->size() );

	/* View Area */
	qreal viewArea = viewRect.width() * viewRect.height();

	/* Page coverage */
	qreal coverage = 0;

	/* Compute visible pages */
	for( int i = 0; i < pages.count(); i++ ) {
		QLabel *lbl = pages[ i ];
		QRectF pgRect = viewRect.intersected( lbl->geometry() );
		if ( pgRect.isValid() ) {
			visiblePages << i;

			/* Page with maximum */
			qreal pageArea = pgRect.width() * pgRect.height();
			if ( ( pageArea / viewArea ) > coverage ) {
				currentPage = i;
				coverage = ( pageArea / viewArea );
			}
		}
	}

	/* Render and set the visible pages */
	Q_FOREACH( int pg, visiblePages ) {
		if ( renderedImages.value( pg ).isNull() )
			renderedImages[ pg ] = PdfDoc->renderPageForWidth( pg, pages[ pg ]->width() );

		/* Draw a rectangle around the current page */
		if ( pg == currentPage ) {
			QImage img = renderedImages[ pg ].copy();
			QPainter painter( &img );
			painter.setPen( Qt::black );
			painter.drawRect( QRectF( QPointF( 0, 0 ), pages[ pg ]->size() - QSize( 1, 1 ) ) );
			painter.end();
			pages.value( pg )->setPixmap( QPixmap::fromImage( img ) );
		}

		else
			pages.value( pg )->setPixmap( QPixmap::fromImage( renderedImages[ pg ] ) );
	}

	/* Obtain the few pages before and after */
	int start = visiblePages.first() - 5;
	int stop = visiblePages.last() + 5;

	while ( start < 0 )
		start++;

	while ( stop >= PdfDoc->pageCount() )
		stop--;

	/* Render the pages before the current page */
	for ( int pg = start; pg < visiblePages.first(); pg++ ) {
		if ( renderedImages.value( pg ).isNull() )
			renderedImages[ pg ] = PdfDoc->renderPageForWidth( pg, pages[ pg ]->width() );
	}

	/* Render the pages after the current page */
	for ( int pg = stop; pg > visiblePages.last(); pg-- ) {
		if ( renderedImages.value( pg ).isNull() )
			renderedImages[ pg ] = PdfDoc->renderPageForWidth( pg, pages[ pg ]->width() );
	}

	/* Clear all other pages and rendered images */
	for( int i = 0; i < pages.count(); i++ ) {
		if ( ( i < start ) or ( i > stop ) ) {
			if ( not renderedImages.value( i ).isNull() )
				renderedImages.remove( i );

			pages.value( i )->clear();
		}
	}
};

void PdfView::reshapeView() {

	if ( not PdfDoc or not PdfDoc->isReady() )
		return;

	pages.clear();
	renderedImages.clear();

	QWidget *w = new QWidget();

	QGridLayout *pgLyt = new QGridLayout();
	pgLyt->setSpacing( 5 );
	pgLyt->setContentsMargins( QMargins( 0, 5, 0, 5 ) );

	if ( mLytMode == Continuous ) {
		switch( mViewMode ) {
			case SinglePageView: {

				for( int i = 0; i < PdfDoc->pageCount(); i++ ) {
					QSizeF pageSize = PdfDoc->pageSize( i );

					QLabel *lbl = new QLabel();
					lbl->setFixedSize( pageSize.toSize() );
					pgLyt->addWidget( lbl, i, 0 );
					pages[ i ] = lbl;
				}

				break;
			}

			case DoublePageView: {

				for( int i = 0; i < PdfDoc->pageCount(); i++ ) {
					QSizeF pageSize = PdfDoc->pageSize( i );

					QLabel *lbl = new QLabel();
					lbl->setFixedSize( pageSize.toSize() );
					pgLyt->addWidget( lbl, i / 2, i % 2 );
					pages[ i ] = lbl;
				}

				break;
			}

			case FitPageToWidth: {

				qreal width = viewport()->width() - style()->pixelMetric( QStyle::PM_ScrollBarExtent ) - 25;

				for( int i = 0; i < PdfDoc->pageCount(); i++ ) {
					QSizeF pageSize = PdfDoc->pageSize( i );

					qreal height = pageSize.height() * width / pageSize.width();

					QLabel *lbl = new QLabel();
					lbl->setFixedSize( QSize( width, height ) );
					pgLyt->addWidget( lbl, i, 0 );
					pages[ i ] = lbl;
				}

				break;
			}

			case FitPageToHeight: {

				qreal height = viewport()->height() - 10;

				for( int i = 0; i < PdfDoc->pageCount(); i++ ) {
					QSizeF pageSize = PdfDoc->pageSize( i );

					qreal width = pageSize.width() * height / pageSize.height();

					QLabel *lbl = new QLabel();
					lbl->setFixedSize( QSize( width, height ) );
					pgLyt->addWidget( lbl, i, 0 );
					pages[ i ] = lbl;
				}

				break;
			}
		}
	}

	else if ( mLytMode == SinglePage ) {

		QLabel *pg1 = new QLabel();
		QLabel *pg2 = new QLabel();

		switch( mViewMode ) {
			case SinglePageView: {

				QSizeF pageSize = PdfDoc->pageSize( currentPage );

				pg1->setFixedSize( pageSize.toSize() );
				pgLyt->addWidget( pg1, 0, 0 );

				break;
			}

			case FitPageToWidth: {

				qreal width = viewport()->width() - style()->pixelMetric( QStyle::PM_ScrollBarExtent ) - 25;
				QSizeF pageSize = PdfDoc->pageSize( currentPage );

				qreal height = pageSize.height() * width / pageSize.width();

				pg1->setFixedSize( QSize( width, height ) );
				pgLyt->addWidget( pg1, 0, 0 );

				break;
			}

			case FitPageToHeight: {

				qreal height = viewport()->height() - 10;

				QSizeF pageSize = PdfDoc->pageSize( currentPage );
				qreal width = pageSize.width() * height / pageSize.height();

				pg1->setFixedSize( QSize( width, height ) );
				pgLyt->addWidget( pg1, 0, 0 );

				break;
			}

			case DoublePageView: {

				/* Size of first page */
				QSizeF pageSize = PdfDoc->pageSize( currentPage );
				pg1->setFixedSize( pageSize.toSize() );
				pgLyt->addWidget( pg1 );

				if ( ( currentPage + 1 ) < PdfDoc->pageCount() ) {
					/* Size of second page */
					pageSize = PdfDoc->pageSize( currentPage + 1 );
					pg2->setFixedSize( pageSize.toSize() );
					pgLyt->addWidget( pg2 );
				}

				break;
			}
		}
	}

	else {
		/*
			*
			* BookView / BookLayout
			*
		*/
	}

	w->setLayout( pgLyt );
	setWidget( w );

	viewport()->update();
	renderPages();
};

bool PdfView::isPageVisible( int pgNo ) {

	QRectF pageRect = pages[ pgNo ]->rect();
	QRectF viewRect = QRectF( QPointF( 0, verticalScrollBar()->value() ), viewport()->size() );

	return viewRect.intersects( pageRect );
};

void PdfView::resizeEvent( QResizeEvent *rEvent ) {

	rEvent->accept();

	int width = rEvent->size().width() - style()->pixelMetric( QStyle::PM_ScrollBarExtent ) - 25;
	int height = rEvent->size().height() - style()->pixelMetric( QStyle::PM_ScrollBarExtent ) - 25;

	if ( rEvent->oldSize() != rEvent->size() ) {
		/* If the width has actually changed */
		if ( rEvent->oldSize().width() != rEvent->size().width() ) {
			/* Book View and Double Page View */
			switch( mViewMode ) {
				case SinglePageView: {
					setZoom( PdfDoc->zoomForWidth( 0, width ) );
					break;
				}

				case DoublePageView: {
					setZoom( PdfDoc->zoomForWidth( 0, ( width - 5.0 ) / 2 ) );
					break;
				}

				case BookView: {
					/* If the width changes, then we have nothing to do */
					break;
				}
			}
		}

		/* If the height has actually changed */
		if ( rEvent->oldSize().height() != rEvent->size().height() ) {
			/* Book View and Double Page View */
			switch( mViewMode ) {
				case SinglePageView:
				case DoublePageView: {
					/* If the height has changed, then we have nothing to do. */
					break;
				}

				case BookView: {
					setZoom( PdfDoc->zoomForHeight( 0, height / 2 ) );
					break;
				}
			}
		}

		/* We reshape the view only if there was a resize */
		reshapeView();
	}

	viewport()->repaint();
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

	if ( mViewMode == DoublePageView ) {
		if ( kEvent->key() == Qt::Key_Right ) {
			if ( currentPage + 2 < PdfDoc->pageCount() )
				verticalScrollBar()->setValue( pages.value( currentPage + 1 )->geometry().top() - 5 );
		}

		else if ( kEvent->key() == Qt::Key_Left ) {
			if ( currentPage - 2 >= 0 )
				verticalScrollBar()->setValue( pages.value( currentPage - 1 )->geometry().top() - 5 );
		}
	}

	else {
		switch ( kEvent->key() ) {
			case Qt::Key_Right: {

				if ( currentPage + 1 < PdfDoc->pageCount() )
					verticalScrollBar()->setValue( pages.value( currentPage + 1 )->geometry().top() - 5 );

				if (mLytMode != Continuous ) {
					currentPage++;
					reshapeView();
				}

				break;
			};

			case Qt::Key_Left: {

				if ( currentPage - 1 >= 0 )
					verticalScrollBar()->setValue( pages.value( currentPage - 1 )->geometry().top() - 5 );

				if (mLytMode != Continuous ) {
					currentPage--;
					reshapeView();
				}

				break;
			};

			case Qt::Key_Home: {

				verticalScrollBar()->setValue( 0 );

				if (mLytMode != Continuous ) {
					currentPage = 0;
					reshapeView();
				}

				break;
			};

			case Qt::Key_End: {

				verticalScrollBar()->setValue( widget()->height() );

				if (mLytMode != Continuous ) {
					currentPage = PdfDoc->pageCount() - 1;
					reshapeView();
				}

				break;
			};

			case Qt::Key_Escape: {

				emit closePreview();

				break;
			};

			default: {

				QScrollArea::keyPressEvent( kEvent );
				break;
			};
		}
	}

	viewport()->update();
	kEvent->accept();
};
