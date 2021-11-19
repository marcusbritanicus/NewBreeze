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
#include "Navigation.hpp"

#include <QGuiApplication>
#include <QScreen>
#include <QScrollBar>
#include <QScroller>

DesQDocs::ViewPrivate::ViewPrivate(): QAbstractScrollAreaPrivate() {

    m_document = nullptr;
    m_pageNavigation = nullptr;
    m_pageRenderer = nullptr;
    m_pageMode = DesQDocs::View::SinglePage;
    m_zoomMode = DesQDocs::View::CustomZoom;
    m_zoomFactor = 1.0;
    m_pageSpacing = 3;
    m_documentMargins = QMargins( 6, 6, 6, 6 );
    m_blockPageScrolling = false;
    m_screenResolution = QGuiApplication::primaryScreen()->logicalDotsPerInch() / 72.0;
};

void DesQDocs::ViewPrivate::init() {

    Q_Q( View );

    m_pageNavigation = new DesQDocs::Navigation( q );
    m_pageRenderer = new DesQDocs::Renderer( q );
};

void DesQDocs::ViewPrivate::documentStatusChanged() {

    Q_Q( View );

    updateDocumentLayout();
    q->viewport()->update();
};

void DesQDocs::ViewPrivate::currentPageChanged( int currentPage ) {

    Q_Q( View );

    if ( m_blockPageScrolling )
        return;

    q->verticalScrollBar()->setValue( yPositionForPage( currentPage ) );

    if ( m_pageMode == DesQDocs::View::SinglePage )
        invalidateDocumentLayout();
};

void DesQDocs::ViewPrivate::calculateViewport() {

    Q_Q( View );

    if ( not m_document )
        return;

    const int x = q->horizontalScrollBar()->value();
    const int y = q->verticalScrollBar()->value();
    const int width = q->viewport()->width();
    const int height = q->viewport()->height();

    setViewport( QRect( x, y, width, height ) );
};

void DesQDocs::ViewPrivate::setViewport( QRect viewport ) {

    Q_Q( View );

    if ( m_viewport == viewport )
        return;

    const QSize oldSize = m_viewport.size();

    m_viewport = viewport;

    if ( oldSize != m_viewport.size() ) {
        updateDocumentLayout();

        if ( m_zoomMode != DesQDocs::View::CustomZoom ) {
            q->viewport()->update();
        }
    }

    if ( m_pageMode == DesQDocs::View::MultiPage ) {
        // An imaginary, 2px height line at the upper half of the viewport, which is used to
        // determine which page is currently located there -> we propagate that as 'current' page
        // to the DesQDocs::Navigation object
        const QRect currentPageLine( m_viewport.x(), m_viewport.y() + m_viewport.height() * 0.4, m_viewport.width(), 2 );

        int currentPage = 0;
        for ( auto it = m_documentLayout.pageGeometries.cbegin(); it != m_documentLayout.pageGeometries.cend(); ++it ) {
            const QRect pageGeometry = it.value();
            if ( pageGeometry.intersects( currentPageLine ) ) {
                currentPage = it.key();
                break;
            }
        }

        if ( currentPage != m_pageNavigation->currentPage() ) {
            m_blockPageScrolling = true;
            m_pageNavigation->setCurrentPage( currentPage );
            m_blockPageScrolling = false;
        }
    }
};

void DesQDocs::ViewPrivate::updateScrollBars() {

    Q_Q( View );

    const QSize p = q->viewport()->size();
    const QSize v = m_documentLayout.documentSize;

    q->horizontalScrollBar()->setRange( 0, v.width() - p.width() );
    q->horizontalScrollBar()->setPageStep( p.width() );
    q->verticalScrollBar()->setRange( 0, v.height() - p.height() );
    q->verticalScrollBar()->setPageStep( p.height() );
};

void DesQDocs::ViewPrivate::invalidateDocumentLayout() {

    Q_Q( View );

    updateDocumentLayout();
    q->viewport()->update();
};

DesQDocs::ViewPrivate::DocumentLayout DesQDocs::ViewPrivate::calculateDocumentLayout() const {
    // The DocumentLayout describes a virtual layout where all pages are positioned inside
    //    - For SinglePage mode, this is just an area as large as the current page surrounded
    //      by the m_documentMargins.
    //    - For MultiPage mode, this is the area that is covered by all pages which are placed
    //      below each other, with m_pageSpacing inbetween and surrounded by m_documentMargins

    DocumentLayout documentLayout;

    if ( !m_document || m_document->status() != DesQDocs::Document::Ready )
        return documentLayout;

    QHash<int, QRect> pageGeometries;

    const int pageCount = m_document->pageCount();

    int totalWidth = 0;

    const int startPage = ( m_pageMode == DesQDocs::View::SinglePage ? m_pageNavigation->currentPage() : 0 );
    const int endPage = ( m_pageMode == DesQDocs::View::SinglePage ? m_pageNavigation->currentPage() + 1 : pageCount );

    // calculate page sizes
    for ( int page = startPage; page < endPage; ++page ) {
        QSize pageSize;
        if ( m_zoomMode == DesQDocs::View::CustomZoom ) {
            pageSize = QSizeF( m_document->pageSize( page ) * m_screenResolution * m_zoomFactor ).toSize();
        }

        else if ( m_zoomMode == DesQDocs::View::FitToWidth ) {
            pageSize = QSizeF( m_document->pageSize( page ) * m_screenResolution ).toSize();
            const qreal factor = ( qreal( m_viewport.width() - m_documentMargins.left() - m_documentMargins.right() ) / qreal( pageSize.width() ) );
            pageSize *= factor;
        }

        else if ( m_zoomMode == DesQDocs::View::FitInView ) {
            const QSize viewportSize( m_viewport.size() + QSize( -m_documentMargins.left() - m_documentMargins.right(), -m_pageSpacing ) );

            pageSize = QSizeF( m_document->pageSize( page ) * m_screenResolution ).toSize();
            pageSize = pageSize.scaled( viewportSize, Qt::KeepAspectRatio );
        }

        switch( m_renderOpts.rotation() ) {
            /* Normal */
            case DesQDocs::RenderOptions::Rotate0:
            case DesQDocs::RenderOptions::Rotate180: {
                totalWidth = qMax( totalWidth, pageSize.width() );
                pageGeometries[page] = QRect( QPoint( 0, 0 ), pageSize );
                break;
            }

            /* 90 degree rotated */
            case DesQDocs::RenderOptions::Rotate90:
            case DesQDocs::RenderOptions::Rotate270: {
                totalWidth = qMax( totalWidth, pageSize.height() );
                pageGeometries[page] = QRect( 0, 0, pageSize.height(), pageSize.width() );
                break;
            }
        }
    }

    totalWidth += m_documentMargins.left() + m_documentMargins.right();

    int pageY = m_documentMargins.top();

    // calculate page positions
    for ( int page = startPage; page < endPage; ++page ) {
        const QSize pageSize = pageGeometries[page].size();

        // center horizontal inside the viewport
        const int pageX = ( qMax( totalWidth, m_viewport.width() ) - pageSize.width() ) / 2;
        pageGeometries[page].moveTopLeft( QPoint( pageX, pageY ) );
        pageY += pageSize.height() + m_pageSpacing;
    }

    pageY += m_documentMargins.bottom();

    documentLayout.pageGeometries = pageGeometries;

    // calculate overall document size
    documentLayout.documentSize = QSize( totalWidth, pageY );

    return documentLayout;
};

qreal DesQDocs::ViewPrivate::yPositionForPage( int pageNumber ) const {

    const auto it = m_documentLayout.pageGeometries.constFind( pageNumber );
    if ( it == m_documentLayout.pageGeometries.cend() )
        return 0.0;

    return ( *it ).y();
};

void DesQDocs::ViewPrivate::updateDocumentLayout() {

    m_documentLayout = calculateDocumentLayout();

    updateScrollBars();
};

qreal DesQDocs::ViewPrivate::zoomFactor() const {

    int page = m_pageNavigation->currentPage();

    QSize pageSize;
    if ( m_zoomMode == DesQDocs::View::CustomZoom ) {
        return m_zoomFactor;
    }

    else if ( m_zoomMode == DesQDocs::View::FitToWidth ) {
        pageSize = QSizeF( m_document->pageSize( page ) * m_screenResolution ).toSize();
        const qreal factor = ( qreal( m_viewport.width() - m_documentMargins.left() - m_documentMargins.right() ) / qreal( pageSize.width() ) );
        return factor;
    }

    else if ( m_zoomMode == DesQDocs::View::FitInView ) {
        const QSize viewportSize( m_viewport.size() + QSize( -m_documentMargins.left() - m_documentMargins.right(), -m_pageSpacing ) );

        pageSize = QSizeF( m_document->pageSize( page ) * m_screenResolution ).toSize();
        pageSize = pageSize.scaled( viewportSize, Qt::KeepAspectRatio );

        return pageSize.width() / m_document->pageSize( page ).width();
    }

    return 1.0;
};
