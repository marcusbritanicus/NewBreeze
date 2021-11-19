    /*
    *
    * This file is a part of DesQDocs.
    * DesQDocs is the default document viewer for the DesQ Suite
    * Copyright 2019-2021 Britanicus <marcusbritanicus@gmail.com>
    *

    *
    * This program is free software; you can redistribute it and/or modify
    * it under the terms of the GNU General Public License as published by
    * the Free Software Foundation; either version 2 of the License, or
    * at your option, any later version.
    *

    *
    * This program is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    * GNU General Public License for more details.
    *

    *
    * You should have received a copy of the GNU General Public License
    * along with this program; if not, write to the Free Software
    * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    * MA 02110-1301, USA.
    *
*/

#include "View.hpp"
#include "ViewImpl.hpp"
#include "Renderer.hpp"
#include "Document.hpp"
// #include "MuPdfDocument.hpp"
#include "PopplerDocument.hpp"
#include "Navigation.hpp"
#include "ViewWidgets.hpp"

#include <QGuiApplication>
#include <QScreen>
#include <QScrollBar>
#include <QScroller>

DesQDocs::View::View( QWidget *parent ) : QAbstractScrollArea( *new ViewPrivate(), parent ) {

    Q_D( View );

    d->init();

    /* Setup Page Navigation */
    connect( d->m_pageNavigation, &DesQDocs::Navigation::currentPageChanged, this, [d]( int page ){ d->currentPageChanged( page ); } );

    /* Setup Page Renderer */
    connect(
        d->m_pageRenderer, &DesQDocs::Renderer::pageRendered, [=]( int ) {

            viewport()->update();
        }
    );

    /* Zoom buttons */
    mZoomBtn = new Zoom( this );
    if ( d->m_zoomMode == CustomZoom ) {
        mZoomBtn->show();
        mZoomBtn->setEnlargeEnabled( d->m_zoomFactor < 4.0 ? true : false );
        mZoomBtn->setDwindleEnabled( d->m_zoomFactor > 0.1 ? true : false );
    }

    else {
        mZoomBtn->hide();
    }

    connect(
        mZoomBtn, &Zoom::clicked, [=]( QString action ) {
            if ( action == "enlarge" )
                setZoomFactor( d->m_zoomFactor * 1.10 );

            else
                setZoomFactor( d->m_zoomFactor / 1.10 );

            mZoomBtn->setEnlargeEnabled( d->m_zoomFactor < 4.0 ? true : false );
            mZoomBtn->setDwindleEnabled( d->m_zoomFactor > 0.1 ? true : false );
        }
    );

    /* Page buttons */
	mPagesBtn = new PageWidget( this );
	connect( d->m_pageNavigation, &DesQDocs::Navigation::currentPageChanged, mPagesBtn, &PageWidget::setCurrentPage );
	connect( mPagesBtn, &PageWidget::loadPage, d->m_pageNavigation, &DesQDocs::Navigation::setCurrentPage );

    /* ProgressBar */
    progress = new QProgressBar( this );
    progress->move( 5, 5 );
    progress->setFixedSize( 50, 10 );
    progress->setRange( 0, 100 );
    progress->setStyle( QStyleFactory::create( "fusion" ) );
    progress->setFormat( "" );

    /* Does this work? */
    verticalScrollBar()->setSingleStep( 20 );
    horizontalScrollBar()->setSingleStep( 20 );

    /* First load */
    d->calculateViewport();

    /* Don't draw a frame around the QScrollArea */
	setFrameStyle( QFrame::NoFrame );

    /* 100% Zoom */
    QShortcut *shortcut = new QShortcut( this );
    shortcut->setKey( QKeySequence( Qt::CTRL + Qt::Key_0 ) );
    connect(
        shortcut, &QShortcut::activated, [=]() {
            setZoomFactor( 1.0 );
        }
    );
};

DesQDocs::View::View( ViewPrivate &dd, QWidget *parent ) : QAbstractScrollArea( dd, parent ) {
};

DesQDocs::View::~View() {
};

void DesQDocs::View::loadDocument( QString path ) {

	DesQDocs::Document *doc = new PopplerDocument( path );

    progress->show();
    connect(
        doc, &Document::loading, [=]( int pc ) {
            progress->setValue( pc );

            if ( pc == 100 )
                progress->hide();

            qApp->processEvents();
        }
    );

	doc->load();

    if ( doc->passwordNeeded() ) {
        bool ok = true;
        int count = 0;
        do {
            QString passwd = QInputDialog::getText(
                this,
                "DesQDocs | Encrypted Document",
                QString( "%1Please enter the document password:" ).arg( count ? "You may have entered the wrong password.<br>" : "" ),
                QLineEdit::Password,
                QString(),
                &ok,
                Qt::WindowFlags(),
                Qt::ImhSensitiveData
            );

            doc->setPassword( passwd );
            count++;
        } while ( ok == true and doc->passwordNeeded() );

        /* User cancelled loading the document */
        if ( not ok )
            return;
    }

    /* Password has been supplied (if needed), document ready to be loaded */
    setDocument( doc );
};

void DesQDocs::View::setDocument( DesQDocs::Document *document ) {

    Q_D( View );

    if ( d->m_document == document )
        return;

    if ( d->m_document ) {
        disconnect( d->m_documentStatusChangedConnection );
        disconnect( d->m_reloadDocumentConnection );
	}

    d->m_document = document;
    emit documentChanged( d->m_document );

    if ( d->m_document ) {
        d->m_documentStatusChangedConnection = connect(
            d->m_document, &DesQDocs::Document::statusChanged, this, [=]( DesQDocs::Document::Status sts ) {
                d->documentStatusChanged();

                if ( sts == DesQDocs::Document::Loading ) {
                    progress->show();
                }

                else {
                    progress->hide();
                }
            }
        );

        d->m_reloadDocumentConnection = connect(
            d->m_document, &DesQDocs::Document::reloadDocument, this, [ d ]() {
                d->m_pageRenderer->reload();
            }
        );
    }

    d->m_pageNavigation->setDocument( d->m_document );
    d->m_pageRenderer->setDocument( d->m_document );

    d->documentStatusChanged();

	mPagesBtn->setMaximumPages( document->pageCount() );
	mPagesBtn->setCurrentPage( d->m_pageNavigation->currentPage() );
};

DesQDocs::Document *DesQDocs::View::document() const {

    Q_D( const View );

    return d->m_document;
};

DesQDocs::Navigation *DesQDocs::View::pageNavigation() const {

    Q_D( const View );

    return d->m_pageNavigation;
};

DesQDocs::View::PageMode DesQDocs::View::pageMode() const {

    Q_D( const View );

    return d->m_pageMode;
};

void DesQDocs::View::setPageMode( PageMode mode ) {

    Q_D( View );

    if ( d->m_pageMode == mode )
        return;

    d->m_pageMode = mode;
    d->invalidateDocumentLayout();

    emit pageModeChanged( d->m_pageMode );
};

DesQDocs::View::ZoomMode DesQDocs::View::zoomMode() const {

    Q_D( const View );

    return d->m_zoomMode;
};

void DesQDocs::View::setZoomMode( ZoomMode mode ) {

    Q_D( View );

    if ( d->m_zoomMode == mode )
        return;

    d->m_zoomMode = mode;
    d->invalidateDocumentLayout();

    if ( d->m_zoomMode == CustomZoom )
        mZoomBtn->show();

    else
        mZoomBtn->hide();

    emit zoomModeChanged( d->m_zoomMode );
};

qreal DesQDocs::View::zoomFactor() const {

    Q_D( const View );

    return d->m_zoomFactor;
};

void DesQDocs::View::setRenderOptions( DesQDocs::RenderOptions opts ) {

    Q_D( View );

    if ( d->m_renderOpts == opts )
        return;

    d->m_renderOpts = opts;
    d->invalidateDocumentLayout();

    emit renderOptionsChanged( d->m_renderOpts );
};

DesQDocs::RenderOptions DesQDocs::View::renderOptions() const {

    Q_D( const View );
    return d->m_renderOpts;
};

void DesQDocs::View::setZoomFactor( qreal factor ) {

    Q_D( View );

    if ( d->m_zoomFactor == factor )
        return;

    if ( factor > 4.0 )
        factor = 4.0;

    if ( factor < 0.1 )
        factor = 0.1;

    d->m_zoomFactor = factor;
    d->invalidateDocumentLayout();

    emit zoomFactorChanged( d->m_zoomFactor );
};

int DesQDocs::View::pageSpacing() const {

    Q_D( const View );

    return d->m_pageSpacing;
};

void DesQDocs::View::setPageSpacing( int spacing ) {

    Q_D( View );

    if ( d->m_pageSpacing == spacing )
        return;

    d->m_pageSpacing = spacing;
    d->invalidateDocumentLayout();

    emit pageSpacingChanged( d->m_pageSpacing );
};

QMargins DesQDocs::View::documentMargins() const {

    Q_D( const View );

    return d->m_documentMargins;
};

void DesQDocs::View::setDocumentMargins( QMargins margins ) {

    Q_D( View );

    if ( d->m_documentMargins == margins )
        return;

    d->m_documentMargins = margins;
    d->invalidateDocumentLayout();

    emit documentMarginsChanged( d->m_documentMargins );
};

void DesQDocs::View::paintEvent( QPaintEvent *event ) {

    Q_D( View );

    QPainter painter( viewport() );
    painter.fillRect( event->rect(), palette().brush( QPalette::Dark ) );
    painter.translate( -d->m_viewport.x(), -d->m_viewport.y() );

    for ( auto it = d->m_documentLayout.pageGeometries.cbegin(); it != d->m_documentLayout.pageGeometries.cend(); ++it ) {
        const QRect pageGeometry = it.value();
        if ( pageGeometry.intersects( d->m_viewport ) ) {
            painter.fillRect( pageGeometry, Qt::white );

            const int page = it.key();
            QImage img = d->m_pageRenderer->requestPage( page, pageGeometry.size(), d->m_renderOpts );

            if ( img.width() and img.height() )
                painter.drawImage( pageGeometry.topLeft(), img );
        }
    }
};

void DesQDocs::View::resizeEvent( QResizeEvent *event ) {

    Q_D( View );

    QAbstractScrollArea::resizeEvent( event );
    qApp->processEvents();

    if ( mZoomBtn and mZoomBtn->isVisible() )
		mZoomBtn->move( 5, viewport()->height() - mZoomBtn->height() - 5 );


    if ( mPagesBtn )
		mPagesBtn->move( viewport()->width() - mPagesBtn->width() - 5, viewport()->height() - mPagesBtn->height() - 5 );


    d->updateScrollBars();

    qApp->processEvents();

    if ( d->pendingResize )
        return;

    d->pendingResize = true;
    QTimer::singleShot(
        250, [d, this]() {
            d->calculateViewport();
            d->pendingResize = false;
            viewport()->update();
        }
    );
};

void DesQDocs::View::scrollContentsBy( int dx, int dy ) {

    Q_D( View );

    QAbstractScrollArea::scrollContentsBy( dx, dy );

    d->calculateViewport();
};

void DesQDocs::View::keyPressEvent( QKeyEvent *kEvent ) {

    Q_D( View );

    switch( kEvent->key() ) {
        case Qt::Key_Right: {
            /* Go to next page */
            d->m_pageNavigation->setCurrentPage( d->m_pageNavigation->currentPage() + 1 );
            break;
        }

        case Qt::Key_Left: {
            /* Go to previous page */
            d->m_pageNavigation->setCurrentPage( d->m_pageNavigation->currentPage() - 1 );
            break;
        }

        case Qt::Key_Space: {
            /* Move by approximately 90% of the viewport height */
            verticalScrollBar()->setValue( verticalScrollBar()->value() + viewport()->height() * 0.9 );
            break;
        }

        case Qt::Key_Home: {
            /* Go to first page */
            verticalScrollBar()->setValue( 0 );
            break;
        }

        case Qt::Key_End: {
            /* Go to end of the document */
            verticalScrollBar()->setValue( verticalScrollBar()->maximum() );
            break;
        }

        case Qt::Key_Plus: {
            /* Zoom In */
            setZoomFactor( d->m_zoomFactor * 1.10 );
            break;
        }

        case Qt::Key_Minus: {
            /* Zoom Out */
            setZoomFactor( d->m_zoomFactor / 1.10 );
            break;
        }

        default: {
            /* Default action */
            QAbstractScrollArea::keyPressEvent( kEvent );
            break;
        }
    }
};

void DesQDocs::View::wheelEvent( QWheelEvent *wEvent ) {

    Q_D( View );

    if ( wEvent->modifiers() & Qt::ControlModifier ) {
        QPoint numDegrees = wEvent->angleDelta() / 8;

        int steps = numDegrees.y() / 15;

        if ( steps > 0 )
            setZoomFactor( d->m_zoomFactor * 1.10 );

        else
            setZoomFactor( d->m_zoomFactor / 1.10 );

        return;
    }

    QAbstractScrollArea::wheelEvent( wEvent );
};
