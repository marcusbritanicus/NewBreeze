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

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "Document.hpp"
#include "RenderOptions.hpp"

#include <poppler/qt5/poppler-qt5.h>

class PdfPage;

class PopplerDocument : public DesQDocs::Document {
	Q_OBJECT;

	public:
		PopplerDocument( QString pdfPath );

		/* Set a password */
		void setPassword( QString password );

		/* Pdf Info / Metadata */
		QString title() const;
		QString author() const;
		QString creator() const;
		QString producer() const;
		QString created() const;

	public Q_SLOTS:
		void load();
		void close();

	private:
		/* Pointer to our actual poppler document */
		Poppler::Document *mPdfDoc;
};

class PdfPage : public DesQDocs::Page {

	public:
		PdfPage( int );
		~PdfPage();

		/* Way to store Poppler::Page */
		void setPageData( void *data );

		/* Size of the page */
		QSizeF pageSize( qreal zoom = 1.0 ) const;

		/* Thumbnail of the page */
		QImage thumbnail() const;

		/* Render and return a page */
		QImage render( QSize, DesQDocs::RenderOptions ) const;
		QImage render( qreal zoomFactor, DesQDocs::RenderOptions ) const;

		/* Page Text */
		QString pageText() const;

		/* Text of a Selection rectangle */
		QString text( QRectF ) const;

		/* Search for @query in @pageNo or all pages */
		QList<QRectF> search( QString query, DesQDocs::RenderOptions ) const;

	private:
		Poppler::Page *m_page;
};
