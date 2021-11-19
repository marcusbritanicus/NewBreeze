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

#include "Global.hpp"
#include "RenderOptions.hpp"

class RenderTask;

namespace DesQDocs {
    class Document;

    class Renderer: public QObject {
        Q_OBJECT;

        public:
            Renderer( QObject *parent = nullptr );

            void setDocument( Document * );
            QImage requestPage( int pg, QSize imgSz, RenderOptions opts );

            void reload();

        private:
            Document *mDoc;
            qint64 validFrom = -1;

            void validateImage( int pg, QImage img, qint64 id );

            QHash<int, QImage> pageCache;
            QVector<int> pages;
            int pageCacheLimit = 20;

            QHash<int, RenderTask*> requestCache;
            QVector<int> requests;
            int requestLimit = 5;

            QHash<int, RenderTask *> queuedRequests;
            QVector<int> queue;

        Q_SIGNALS:
            void pageRendered( int );
    };
};
