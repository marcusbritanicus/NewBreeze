/*
	*
	* Copyright 2019-2021 Britanicus <marcusbritanicus@gmail.com>
	*
	* This file is a part of DesQ project (https://gitlab.com/desq/)
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

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "RenderOptions.hpp"

namespace DesQDocs {

	class Page;
	typedef QList<DesQDocs::Page*> Pages;

	class Document : public QObject {
		Q_OBJECT;

		public:
			enum Status {
				Null,
				Loading,
				Ready,
				Unloading,
				Error
			};
			Q_ENUM( Status )

			enum DocumentError {
				NoError,
				UnknownError,
				FileNotFoundError,
				InvalidFileFormatError,
				IncorrectPasswordError,
				UnsupportedSecuritySchemeError
			};
			Q_ENUM( DocumentError )

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
			Q_ENUM( MetaDataField )

			Document( QString docPath );

			/* Check if a password is needed */
			virtual bool passwordNeeded() const;

			/* Set a password */
			virtual void setPassword( QString password ) = 0;

			/* Document File Name and File Path */
			virtual QString fileName() const;
			virtual QString filePath() const;

			/* Pdf Info / Metadata */
			virtual QString title() const = 0;
			virtual QString author() const = 0;
			virtual QString creator() const = 0;
			virtual QString producer() const = 0;
			virtual QString created() const = 0;

			/* Number of pages */
			int pageCount() const;

			/* Size of the page */
			QSizeF pageSize( int pageNo ) const;

			/* Reload the current document */
			void reload();

			/* PDF load status */
			Document::Status status() const;

			/* Render and return a page */
			QImage renderPage( int, QSize, RenderOptions ) const;
			QImage renderPage( int, qreal zoomFactor, RenderOptions ) const;

			/* Page Related */
			Pages pages() const;
			Page* page( int pageNo ) const;
			QImage pageThumbnail( int pageNo ) const;

			/* Page Text */
			QString pageText( int pageNo ) const;

			/* Text of a Selection rectangle */
			QString text( int pageNo, QRectF ) const;

			/* Search for @query in @pageNo or all pages */
			QList<QRectF> search( QString query, int pageNo, RenderOptions opts ) const;

			qreal zoomForWidth( int pageNo, qreal width ) const;
			qreal zoomForHeight( int pageNo, qreal width ) const;

			void setZoom( qreal zoom );

		public Q_SLOTS:
			virtual void load() = 0;
			virtual void close() = 0;

		protected:
			QString mDocPath;
			Pages mPages;

			mutable QList<QRectF> searchRects;

			qreal mZoom;

			Status mStatus;
			DocumentError mDocError;
			bool mPassNeeded;

		Q_SIGNALS:
			void passwordRequired();
			void reloadDocument();
			void statusChanged( Document::Status status );
			void pageCountChanged( int pageCount );

			void loading( int );
	};

	class Page {

		public:
			Page( int );
			virtual ~Page();

			virtual void setPageData( void *data ) = 0;

			/* Page number of this page */
			virtual int pageNo();

			/* Render and return a page */
			virtual QImage render( QSize, RenderOptions ) const = 0;
			virtual QImage render( qreal zoomFactor, RenderOptions ) const = 0;

			/* Page Text */
			virtual QString pageText() const = 0;

			/* Text of a Selection rectangle */
			virtual QString text( QRectF ) const = 0;

			/* Search for @query in @pageNo or all pages */
			virtual QList<QRectF> search( QString query, RenderOptions opts ) const = 0;

			/* Size of the page */
			virtual QSizeF pageSize( qreal zoom = 1.0 ) const = 0;

			/* Thumbnail of the page */
			virtual QImage thumbnail() const = 0;

		private:
			int mPageNo = -1;
	};
};
