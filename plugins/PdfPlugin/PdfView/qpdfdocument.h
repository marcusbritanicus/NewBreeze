/*
    *
    * This file is a part of CorePDF.
    * A PDF viewer for CuboCore Application Suite
    * Copyright 2019 CuboCore Group
    *

    *
    * This program is free software; you can redistribute it and/or modify
    * it under the terms of the GNU General Public License as published by
    * the Free Software Foundation; either version 3 of the License, or
    * (at your option) any later version.
    *

    *
    * This program is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    * GNU General Public License for more details.
    *

    *
    * You should have received a copy of the GNU General Public License
    * along with this program; if not, vsit http://www.gnu.org/licenses/.
    *
*/

#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include <poppler/qt5/poppler-qt5.h>

#include "qpdfdocumentrenderoptions.h"

typedef Poppler::Page PdfPage;
typedef QList<Poppler::Page*> PdfPages;

class QPdfDocument : public QObject {
	Q_OBJECT

	public:
		enum Status {
			Null,
			Loading,
			Ready,
			Unloading,
			Error
		};
		Q_ENUM(Status)

		enum DocumentError {
			NoError,
			UnknownError,
			FileNotFoundError,
			InvalidFileFormatError,
			IncorrectPasswordError,
			UnsupportedSecuritySchemeError
		};
		Q_ENUM(DocumentError)

		enum MetaDataField {
			Title,
			Subject,
			Author,
			Keywords,
			Producer,
			Creator,
			CreationDate,
			ModificationDate
		};
		Q_ENUM(MetaDataField)

		QPdfDocument( QString pdfPath );

		/* Check if a password is needed */
		bool passwordNeeded() const;

		/* Set a password */
		void setPassword( QString password );

		/* Pdf File Name and File Path */
		QString pdfName() const;
		QString pdfPath() const;

		/* Pdf Info / Metadata */
		QString title() const;
		QString author() const;
		QString creator() const;
		QString producer() const;
		QString created() const;

		/* Number of pages */
		int pageCount() const;

		/* Size of the page */
		QSizeF pageSize( int pageNo ) const;

		/* Reload the current document */
		void reload();

		/* PDF load status */
		QPdfDocument::Status status() const;

		/* Render and return a page */
		QImage renderPage( int, QSize, QPdfDocumentRenderOptions ) const;
		QImage renderPage( int, qreal zoomFactor, QPdfDocumentRenderOptions ) const;

		/* Page Text */
		QString pageText( int pageNo ) const;

		/* Text of a Selection rectangle */
		QString text( int pageNo, QRectF ) const;

		qreal zoomForWidth( int pageNo, qreal width ) const;
		qreal zoomForHeight( int pageNo, qreal width ) const;

		void setZoom( qreal zoom ) {

			mZoom = zoom;
        }

	public Q_SLOTS:
		void load();
		void close();

	private:
		Poppler::Document *mPdfDoc;

		QString mPdfPath;
		PdfPages mPages;

		qreal mZoom;

		Status mStatus;
		bool mPassNeeded;

	Q_SIGNALS:
		void passwordChanged();
		void passwordRequired();
		void statusChanged(QPdfDocument::Status status);
		void pageCountChanged(int pageCount);
};
