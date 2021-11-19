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

#include <desq/DesQUtils.hpp>

#include "Document.hpp"

/*
	*
	* Generic class to handle document
	*
*/

DesQDocs::Document::Document( QString path ) : QObject() {

	mZoom = 1.0;
	mStatus = Null;
	mDocError = NoError;
	mPassNeeded = false;

	mDocPath = QFileInfo( path ).absoluteFilePath();

	QFileSystemWatcher *fsw = new QFileSystemWatcher();
	fsw->addPath( mDocPath );
	connect(
		fsw, &QFileSystemWatcher::fileChanged, [=]( QString file ) {
			/* File deleted and created again: add it to the watcher */
			if ( not fsw->files().contains( file ) and QFile::exists( file ) )
				fsw->addPath( file );

			reload();
		}
	);
};

QString DesQDocs::Document::fileName() const {

	return DesQ::Utils::baseName( mDocPath );
};

QString DesQDocs::Document::filePath() const {

	return DesQ::Utils::dirName( mDocPath );
};

bool DesQDocs::Document::passwordNeeded() const {

	return mPassNeeded;
};

int DesQDocs::Document::pageCount() const {

	return mPages.count();
};

QSizeF DesQDocs::Document::pageSize( int pageNo ) const {

	return mPages.at( pageNo )->pageSize() * mZoom;
};

void DesQDocs::Document::reload() {

	mStatus = Null;
	mPages.clear();

	load();

	if ( mStatus == Ready ) {
		qDebug() << "Reload your pages..";
		emit reloadDocument();
	}
};

DesQDocs::Document::Status DesQDocs::Document::status() const {

	return mStatus;
};

QImage DesQDocs::Document::renderPage( int pageNo, QSize size, DesQDocs::RenderOptions opts ) const {

	if ( pageNo >= mPages.count() )
		return QImage();

	return mPages.at( pageNo )->render( size, opts );
};

QImage DesQDocs::Document::renderPage( int pageNo, qreal zoomFactor, DesQDocs::RenderOptions opts ) const {

	if ( pageNo >= mPages.count() )
		return QImage();

	return mPages.at( pageNo )->render( zoomFactor, opts );
};

DesQDocs::Pages DesQDocs::Document::pages() const {

	return mPages;
};

DesQDocs::Page* DesQDocs::Document::page( int pageNo ) const {

	if ( pageNo >= mPages.count() )
		return nullptr;

	return mPages.at( pageNo );
};

QImage DesQDocs::Document::pageThumbnail( int pageNo ) const {

	if ( pageNo >= mPages.count() )
		return QImage();

	return mPages.at( pageNo )->thumbnail();
};

QString DesQDocs::Document::pageText( int pageNo ) const {

	return mPages.at( pageNo )->text( QRectF() );
};

QString DesQDocs::Document::text( int pageNo, QRectF rect ) const {

	return mPages.at( pageNo )->text( rect );
};

QList<QRectF> DesQDocs::Document::search( QString query, int pageNo, DesQDocs::RenderOptions opts ) const {

	searchRects.clear();

	if ( pageNo >= 0 or pageNo < mPages.count() ) {
		for( QRectF rect: mPages.at( pageNo )->search( query, opts ) ) {
			searchRects << QRect( rect.x() * mZoom, rect.y() * mZoom, rect.width() * mZoom, rect.height() * mZoom );
		}
	}

	return searchRects;
};

qreal DesQDocs::Document::zoomForWidth( int pageNo, qreal width ) const {

	if ( pageNo >= mPages.count() )
		return 0.0;

	return 1.0 * width / mPages.at( pageNo )->pageSize().width();
};

qreal DesQDocs::Document::zoomForHeight( int pageNo, qreal height ) const {

	if ( pageNo >= mPages.count() )
		return 0.0;

	return 1.0 * height / mPages.at( pageNo )->pageSize().height();
};

void DesQDocs::Document::setZoom( qreal zoom ) {

	mZoom = zoom;
};

/*
	*
	* Generic class to handle document page
	*
*/

DesQDocs::Page::Page( int pgNo ) {

	mPageNo = pgNo;
};

DesQDocs::Page::~Page() {
};

int DesQDocs::Page::pageNo() {

	return mPageNo;
};
