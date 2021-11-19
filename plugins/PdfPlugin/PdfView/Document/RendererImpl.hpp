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

#include "Renderer.hpp"
#include "RenderOptions.hpp"

namespace DesQDocs {
    class Page;
}

class RenderTask : public QObject, public QRunnable {
    Q_OBJECT;

    public:
        RenderTask( DesQDocs::Page *pg, QSize imgSz, DesQDocs::RenderOptions opts, qint64 id );

        int pageNumber();
        qint64 requestId();
        QSize imageSize();

        void invalidate();

        void run();

    private:
        DesQDocs::Page *mPage;
        QSize mImgSize;
        DesQDocs::RenderOptions mOpts;
        qint64 mId;

    Q_SIGNALS:
        void imageReady( int pageNo, QImage image, qint64 id );
};
