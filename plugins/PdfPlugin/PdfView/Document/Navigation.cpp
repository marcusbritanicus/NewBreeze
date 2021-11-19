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

#include "Navigation.hpp"
#include "Document.hpp"

#include <QPointer>

DesQDocs::Navigation::Navigation( QObject *parent ) : QObject( *new NavigationPrivate, parent ) {

}

DesQDocs::Navigation::~Navigation() {
}

DesQDocs::Document* DesQDocs::Navigation::document() const {

    Q_D( const DesQDocs::Navigation );

    return d->m_document;
};

void DesQDocs::Navigation::setDocument( Document *document ) {

    Q_D( DesQDocs::Navigation );

    if ( d->m_document == document )
        return;

    if ( d->m_document )
        disconnect( d->m_documentStatusChangedConnection );

    d->m_document = document;
    emit documentChanged( d->m_document );

    if ( d->m_document )
        d->m_documentStatusChangedConnection = connect( d->m_document.data(), &Document::statusChanged, this, [d](){ d->documentStatusChanged(); } );

    d->update();
}

int DesQDocs::Navigation::currentPage() const {

    Q_D( const DesQDocs::Navigation );

    return d->m_currentPage;
}

void DesQDocs::Navigation::setCurrentPage( int newPage ) {

    Q_D( DesQDocs::Navigation );

    if ( newPage < 0 || newPage >= d->m_pageCount )
        return;

    if ( d->m_currentPage == newPage )
        return;

    d->m_currentPage = newPage;
    emit currentPageChanged( d->m_currentPage );

    d->updatePrevNext();
}

int DesQDocs::Navigation::pageCount() const {

    Q_D( const DesQDocs::Navigation );
    return d->m_pageCount;
}

bool DesQDocs::Navigation::canGoToPreviousPage() const {

    Q_D( const DesQDocs::Navigation );
    return d->m_canGoToPreviousPage;
}

bool DesQDocs::Navigation::canGoToNextPage() const {

    Q_D( const DesQDocs::Navigation );
    return d->m_canGoToNextPage;
}

void DesQDocs::Navigation::goToPreviousPage() {

    Q_D( DesQDocs::Navigation );

    if ( d->m_currentPage > 0 )
        setCurrentPage( d->m_currentPage - 1 );
}

void DesQDocs::Navigation::goToNextPage() {

    Q_D( DesQDocs::Navigation );

    if ( d->m_currentPage < d->m_pageCount - 1 )
        setCurrentPage( d->m_currentPage + 1 );
}
