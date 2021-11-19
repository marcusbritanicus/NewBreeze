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

#include "Global.hpp"

class Zoom : public QWidget {
    Q_OBJECT

    public:
        Zoom( QWidget *parent );

        void setEnlargeEnabled( bool );
        void setDwindleEnabled( bool );

    private:
        typedef struct _state {
            bool mouseIn;
            bool inDwindle;
            bool inEnlarge;
            bool dwindlePressed;
            bool enlargePressed;
        } ZoomBtnState;

        bool isEnlargeEnabled;
        bool isDwindleEnabled;

        ZoomBtnState btnState;

        QRectF enlargeRect;
        QRectF dwindleRect;

        QPixmap enlargeIcon;
        QPixmap dwindleIcon;

    protected:
        void enterEvent( QEvent * ) override;
        void leaveEvent( QEvent * ) override;

        void mousePressEvent( QMouseEvent * ) override;
        void mouseMoveEvent( QMouseEvent * ) override;
        void mouseReleaseEvent( QMouseEvent * ) override;

        void paintEvent( QPaintEvent *pEvent ) override;

    Q_SIGNALS:
        void clicked( QString );
};

class PageWidget : public QWidget {
	Q_OBJECT

	public:
		PageWidget( QWidget *parent );

        void setMaximumPages( int );
        void setCurrentPage( int );

	private:
		typedef struct _state {
			bool mouseIn;
			bool inPrevious;
			bool inNext;
            bool inText;
			bool previousPressed;
			bool nextPressed;
            bool textPressed;
		} ZoomBtnState;

		bool isNextEnabled;
		bool isPreviousEnabled;

		ZoomBtnState btnState;

		QRectF nextRect;
		QRectF previousRect;
		QRectF textRect;

		QPixmap nextIcon;
		QPixmap previousIcon;

        int maxPages = 0;
        int curPage = 0;

	protected:
		void enterEvent( QEvent * ) override;
		void leaveEvent( QEvent * ) override;

		void mousePressEvent( QMouseEvent * ) override;
		void mouseMoveEvent( QMouseEvent * ) override;
		void mouseReleaseEvent( QMouseEvent * ) override;

		void paintEvent( QPaintEvent *pEvent ) override;

	Q_SIGNALS:
		void loadPage( int );
};
