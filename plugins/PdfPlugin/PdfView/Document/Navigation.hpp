/*
	*
	* Copyright 2019-2021 Britanicus <marcusbritanicus@gmail.com>
	*
	* This file is a part of DesQ project ( https://gitlab.com/desq/ )
	*

	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 3 of the License, or
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

#include <QObject>
#include <private/qobject_p.h>

#include "Document.hpp"

namespace DesQDocs {

    class Document;
    class Navigation;
    class NavigationPrivate;

    class Navigation : public QObject {

        Q_OBJECT;

        Q_PROPERTY( Document* document READ document WRITE setDocument NOTIFY documentChanged )

        Q_PROPERTY( int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged )
        Q_PROPERTY( int pageCount READ pageCount NOTIFY pageCountChanged )
        Q_PROPERTY( bool canGoToPreviousPage READ canGoToPreviousPage NOTIFY canGoToPreviousPageChanged )
        Q_PROPERTY( bool canGoToNextPage READ canGoToNextPage NOTIFY canGoToNextPageChanged )

        public:
            explicit Navigation( QObject *parent = nullptr );
            ~Navigation();

            Document* document() const;
            void setDocument( Document *document );

            int currentPage() const;
            void setCurrentPage( int currentPage );

            int pageCount() const;

            bool canGoToPreviousPage() const;
            bool canGoToNextPage() const;

        public Q_SLOTS:
            void goToPreviousPage();
            void goToNextPage();

        Q_SIGNALS:
            void documentChanged( Document *document );
            void currentPageChanged( int currentPage );
            void pageCountChanged( int pageCount );
            void canGoToPreviousPageChanged( bool canGo );
            void canGoToNextPageChanged( bool canGo );

        private:
            Q_DECLARE_PRIVATE( Navigation );
    };

    class NavigationPrivate : public QObjectPrivate {

        public:
            NavigationPrivate() : QObjectPrivate() {
            }

            void update() {

                Q_Q( Navigation );

                const bool documentAvailable = m_document && m_document->status() == Document::Ready;

                if ( documentAvailable ) {
                    const int newPageCount = m_document->pageCount();
                    if ( m_pageCount != newPageCount ) {
                        m_pageCount = newPageCount;
                        emit q->pageCountChanged( m_pageCount );
                    }
                }

                else {
                    if ( m_pageCount != 0 ) {
                        m_pageCount = 0;
                        emit q->pageCountChanged( m_pageCount );
                    }
                }

                if ( m_currentPage != 0 ) {
                    m_currentPage = 0;
                    emit q->currentPageChanged( m_currentPage );
                }

                updatePrevNext();
            }

            void updatePrevNext() {

                Q_Q( Navigation );

                const bool hasPreviousPage = m_currentPage > 0;
                const bool hasNextPage = m_currentPage < ( m_pageCount - 1 );

                if ( m_canGoToPreviousPage != hasPreviousPage ) {

                    m_canGoToPreviousPage = hasPreviousPage;
                    emit q->canGoToPreviousPageChanged( m_canGoToPreviousPage );
                }

                if ( m_canGoToNextPage != hasNextPage ) {

                    m_canGoToNextPage = hasNextPage;
                    emit q->canGoToNextPageChanged( m_canGoToNextPage );
                }
            }

            void documentStatusChanged() {

                update();
            }

            Q_DECLARE_PUBLIC( Navigation );

            QPointer<Document> m_document = nullptr;
            int m_currentPage = 0;
            int m_pageCount = 0;
            bool m_canGoToPreviousPage = false;
            bool m_canGoToNextPage = false;

            QMetaObject::Connection m_documentStatusChangedConnection;
    };
}
