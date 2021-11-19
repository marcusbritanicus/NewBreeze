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

#pragma once

#include <QtWidgets/qabstractscrollarea.h>
#include "RenderOptions.hpp"

class Zoom;
class PageWidget;
class QProgressBar;

namespace DesQDocs {
    class Document;
    class Navigation;
    class Renderer;
    class ViewPrivate;

    class View : public QAbstractScrollArea {
        Q_OBJECT;

        Q_PROPERTY( DesQDocs::Document* document READ document WRITE setDocument NOTIFY documentChanged );

        Q_PROPERTY( PageMode pageMode READ pageMode WRITE setPageMode NOTIFY pageModeChanged );
        Q_PROPERTY( ZoomMode zoomMode READ zoomMode WRITE setZoomMode NOTIFY zoomModeChanged );
        Q_PROPERTY( qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged );
        Q_PROPERTY( DesQDocs::RenderOptions renderOptions READ renderOptions WRITE setRenderOptions NOTIFY renderOptionsChanged );

        Q_PROPERTY( int pageSpacing READ pageSpacing WRITE setPageSpacing NOTIFY pageSpacingChanged );
        Q_PROPERTY( QMargins documentMargins READ documentMargins WRITE setDocumentMargins NOTIFY documentMarginsChanged );

        public:
            enum PageMode {
                SinglePage,
                MultiPage
            };
            Q_ENUM( PageMode );

            enum ZoomMode {
                CustomZoom,
                FitToWidth,
                FitInView
            };
            Q_ENUM( ZoomMode );

            explicit View( QWidget *parent = nullptr );
            ~View();

            void loadDocument( QString path );
            void setDocument( DesQDocs::Document *document );
            DesQDocs::Document *document() const;

            DesQDocs::Navigation *pageNavigation() const;

            PageMode pageMode() const;
            ZoomMode zoomMode() const;
            qreal zoomFactor() const;
            DesQDocs::RenderOptions renderOptions() const;

            int pageSpacing() const;
            void setPageSpacing( int spacing );

            QMargins documentMargins() const;
            void setDocumentMargins( QMargins margins );

        public Q_SLOTS:
            void setPageMode( PageMode mode );
            void setZoomMode( ZoomMode mode );
            void setZoomFactor( qreal factor );
            void setRenderOptions( DesQDocs::RenderOptions opts );

        Q_SIGNALS:
            void documentChanged( DesQDocs::Document *document );
            void pageModeChanged( PageMode pageMode );
            void zoomModeChanged( ZoomMode zoomMode );
            void zoomFactorChanged( qreal zoomFactor );
            void renderOptionsChanged( DesQDocs::RenderOptions opts );
            void pageSpacingChanged( int pageSpacing );
            void documentMarginsChanged( QMargins documentMargins );

            void documentLoadingFailed();

        protected:
            explicit View( ViewPrivate &, QWidget * );

            void paintEvent( QPaintEvent *event ) override;
            void resizeEvent( QResizeEvent *event ) override;
            void scrollContentsBy( int dx, int dy ) override;

            void keyPressEvent( QKeyEvent *kEvent );

            void wheelEvent( QWheelEvent *wEvent );

        private:
            Q_DECLARE_PRIVATE( View );

            Zoom *mZoomBtn;
            PageWidget *mPagesBtn;
            QProgressBar *progress;
    };
}
