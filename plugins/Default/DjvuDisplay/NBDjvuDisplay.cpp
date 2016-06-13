/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Classes
	*
*/

#include "NBDjvuDisplay.hpp"

NBDjvuDisplay::NBDjvuDisplay( QString pth ) : QDialog() {

	path = QString( pth );

	createGUI();
	setWindowProperties();

	peekWidgetBase->setFocus();
};

void NBDjvuDisplay::createGUI() {

	QHBoxLayout *lblBtnLyt = new QHBoxLayout();
	QVBoxLayout *widgetLyt = new QVBoxLayout();
	QVBoxLayout *baseLyt = new QVBoxLayout();

	QLabel *lbl = new QLabel( "<tt><b>" + path + "</b></tt>" );

	QToolButton *openBtn = new QToolButton();
	openBtn->setIcon( QIcon( ":/icons/maximize.png" ) );
	openBtn->setAutoRaise( true );
	openBtn->setFocusPolicy( Qt::NoFocus );

	QWidget *baseWidget = new QWidget();
	baseWidget->setObjectName( tr( "guiBase" ) );

	connect( openBtn, SIGNAL( clicked() ), this, SLOT( openInExternal() ) );

	peekWidgetBase = new QScrollArea();
	peekWidgetBase->setAlignment( Qt::AlignCenter );
	peekWidgetBase->setObjectName( tr( "previewBase" ) );

	QWidget *djvuBase = new QWidget();
	djvuLyt = new QVBoxLayout();
	djvuLyt->setAlignment( Qt::AlignCenter );
	djvuLyt->setContentsMargins( QMargins() );
	djvuBase->setLayout( djvuLyt );

	peekWidgetBase->setWidget( djvuBase );
	peekWidgetBase->setWidgetResizable( true );

	lblBtnLyt->addWidget( lbl );
	lblBtnLyt->addStretch( 0 );
	lblBtnLyt->addWidget( openBtn );

	widgetLyt->addLayout( lblBtnLyt );
	widgetLyt->addWidget( peekWidgetBase );

	baseWidget->setLayout( widgetLyt );
	baseLyt->addWidget( baseWidget );
	baseLyt->setContentsMargins( 0, 0, 0, 0 );

	setLayout( baseLyt );
	peekWidgetBase->setFocus();
};

void NBDjvuDisplay::setWindowProperties() {

	setFixedSize( 720, 540 );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 720 ) / 2 );
	int vpos = (int)( ( dw.height() - 540 ) / 2 );

	setGeometry( hpos, vpos, 720, 540 );

	/* We don't want to bother deleting this widget, let it get automatically done. */
	setAttribute( Qt::WA_DeleteOnClose );
};

int NBDjvuDisplay::exec() {

	QTimer::singleShot( 0, this, SLOT( loadDjvu() ) );

	return QDialog::exec();
};

void NBDjvuDisplay::openInExternal() {

	QProcess::startDetached( "xdg-open " + path );
	close();
};

void NBDjvuDisplay::loadDjvu() {

	int pageWidth = 700 - peekWidgetBase->verticalScrollBar()->width() - 4;

	/* DjVu Context */
	ddjvu_context_t *ctx = ddjvu_context_create( "newbreeze3" );

	/* DjVu Document */
	ddjvu_document_t *doc = ddjvu_document_create_by_filename( ctx, path.toLocal8Bit().data(), 1 );

	/* Wait for decoding to be complete */
	ddjvu_job_t *job = ddjvu_document_job( doc );
	ddjvu_message_wait( ctx );
	while ( ddjvu_job_status( job ) != DDJVU_JOB_OK ) {
		if ( ddjvu_job_status( job ) == DDJVU_JOB_FAILED ) {
			QLabel *page = new QLabel( this );
			page->setAlignment( Qt::AlignCenter );
			page->setWordWrap( true );
			page->setText(
				"Unable to process the djvu document. The djvu file may be damaged or you may not have sufficient permissions to view it."
			);
			djvuLyt->addWidget( page );
			return;
		};

		continue;
	}

	/* DjVu page ptr */
	ddjvu_page_t *pg;
	/* DjVu page job ptr */
	ddjvu_job_t *pgjob;

	/* DjVu page info struct */
	ddjvu_pageinfo_t info;

	/* DjVu page render rect */
	ddjvu_rect_t rect;
	rect.w = pageWidth;
	rect.x = 0;
	rect.y = 0;

	QLabel *djvuPage;

	/* DjVu page render format */
	unsigned int masks[ 4 ] = { 0xff0000, 0xff00, 0xff, 0xff000000 };
	ddjvu_format_t *fmt = ddjvu_format_create( DDJVU_FORMAT_RGBMASK32, 4, masks );

	/* Get the number of pages */
	ddjvu_message_wait( ctx );
	int pages = 0;
	pages = ddjvu_document_get_pagenum( doc );

	for ( int i = 0; i < pages; i++ ) {
		/* Create and decode page */
		pg = ddjvu_page_create_by_pageno( doc, i );
		pgjob = ddjvu_page_job( pg );

		while ( ddjvu_job_status( pgjob ) < DDJVU_JOB_OK )
			continue;

		while ( ddjvu_document_get_pageinfo_imp( doc, i, &info, sizeof( ddjvu_pageinfo_t ) ) < DDJVU_JOB_OK )
			continue;

		/* Set rectange height */
		rect.h = pageWidth * info.height / info.width;

		/* Make DjVu decoder follow X11 conventions */
		ddjvu_format_set_row_order( fmt, true );
		ddjvu_format_set_y_direction( fmt, true );

		/* Render the DjVu page to QImage */
		QImage image( rect.w, rect.h, QImage::Format_RGB32 );
		ddjvu_page_render( pg, DDJVU_RENDER_COLOR, &rect, &rect, fmt, image.bytesPerLine(), (char*)image.bits() );

		/* Load the image into a QLabel for display */
		djvuPage = new QLabel();
		djvuPage->setPixmap( QPixmap::fromImage( image ).scaledToWidth( pageWidth, Qt::SmoothTransformation ) );
		djvuLyt->addWidget( djvuPage );

		qApp->processEvents();
	}
};

void NBDjvuDisplay::keyPressEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == Qt::Key_Escape )
		close();

	else
		QWidget::keyPressEvent( keyEvent );
};

void NBDjvuDisplay::changeEvent( QEvent *event ) {

	if ( ( event->type() == QEvent::ActivationChange ) and ( !isActiveWindow() ) ) {
		close();
		event->accept();
	}

	else {
		QWidget::changeEvent( event );
		event->accept();
	}
};

void NBDjvuDisplay::paintEvent( QPaintEvent *pEvent ) {

	QWidget::paintEvent( pEvent );
	QPainter *painter = new QPainter( this );

	painter->setPen( QPen( palette().color( QPalette::Window ).darker(), 2.0 ) );
	painter->drawRect( rect().adjusted( +1, +1, -1, -1 ) );

	painter->end();
	pEvent->accept();
};
