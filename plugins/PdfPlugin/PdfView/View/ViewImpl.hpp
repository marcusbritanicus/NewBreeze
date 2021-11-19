/*
    *
    * This file is a part of DesQEye.
    * DesQEye is an Image Viewer for the DesQ Suite
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

#pragma once

#include "View.hpp"

#include <QPointer>
#include <QtWidgets/private/qabstractscrollarea_p.h>

namespace DesQDocs {
    class Renderer;
    class Document;

    class ViewPrivate : public QAbstractScrollAreaPrivate {

        Q_DECLARE_PUBLIC( DesQDocs::View );

        public:
            ViewPrivate();
            void init();

            void documentStatusChanged();
            void currentPageChanged( int currentPage );
            void calculateViewport();
            void setViewport( QRect viewport );
            void updateScrollBars();

            void invalidateDocumentLayout();

            qreal yPositionForPage( int page ) const;

            qreal zoomFactor() const;

            struct DocumentLayout {
                QSize documentSize;
                QHash<int, QRect> pageGeometries;
            };

            DocumentLayout calculateDocumentLayout() const;
            void updateDocumentLayout();

            QPointer<DesQDocs::Document> m_document = nullptr;
            DesQDocs::Navigation* m_pageNavigation = nullptr;
            DesQDocs::Renderer *m_pageRenderer = nullptr;

            View::PageMode m_pageMode;
            View::ZoomMode m_zoomMode;
            qreal m_zoomFactor;
            DesQDocs::RenderOptions m_renderOpts;

            int m_pageSpacing;
            QMargins m_documentMargins;

            bool m_blockPageScrolling;

            QMetaObject::Connection m_documentStatusChangedConnection;
            QMetaObject::Connection m_reloadDocumentConnection;

            QRect m_viewport;

            DocumentLayout m_documentLayout;

            qreal m_screenResolution; // pixels per point
            bool pendingResize = false;
    };
}

Q_DECLARE_TYPEINFO( DesQDocs::ViewPrivate::DocumentLayout, Q_MOVABLE_TYPE );
