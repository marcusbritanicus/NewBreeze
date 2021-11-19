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

#include "Global.hpp"
#include "ViewWidgets.hpp"

Zoom::Zoom( QWidget *parent ) : QWidget( parent ) {

	btnState = { false, false, false, false, false };

	dwindleRect = QRectF( 4, 4, 24, 24 );
	enlargeRect = QRectF( 37, 4, 24, 24 );

	isDwindleEnabled = false;
	isEnlargeEnabled = false;

	setFixedHeight( 32 );
	setFixedWidth( 65 );

	setMouseTracking( true );

	dwindleIcon = QIcon::fromTheme( "zoom-out" ).pixmap( QSize( 24, 24 ), QIcon::Normal );
	enlargeIcon = QIcon::fromTheme( "zoom-in" ).pixmap( QSize( 24, 24 ), QIcon::Normal );
};

void Zoom::setDwindleEnabled( bool yes ) {

	isDwindleEnabled = yes;
	dwindleIcon = QIcon::fromTheme( "zoom-out" ).pixmap( QSize( 24, 24 ), ( yes ? QIcon::Normal : QIcon::Disabled ) );

	repaint();
};

void Zoom::setEnlargeEnabled( bool yes ) {

	isEnlargeEnabled = yes;
	enlargeIcon = QIcon::fromTheme( "zoom-in" ).pixmap( QSize( 24, 24 ), ( yes ? QIcon::Normal : QIcon::Disabled ) );

	repaint();
};

void Zoom::enterEvent( QEvent *event ) {

	btnState.mouseIn = true;

	repaint();
	event->accept();
};

void Zoom::leaveEvent( QEvent *event ) {

	btnState.mouseIn = false;

	repaint();
	event->accept();
};

void Zoom::mousePressEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		// Left half = dwindle button pressed
		if ( dwindleRect.adjusted( -2, -2, 2, 2 ).contains( mEvent->localPos() ) )
			btnState = { true, isDwindleEnabled, false, isDwindleEnabled, false };

		else if ( enlargeRect.adjusted( -2, -2, 2, 2 ).contains( mEvent->localPos() ) )
			btnState = { true, false, isEnlargeEnabled, false, isEnlargeEnabled };

		else
			btnState = { true, false, false, false, false };
	}

	repaint();
	mEvent->accept();
};

void Zoom::mouseMoveEvent( QMouseEvent *mEvent ) {

	if ( btnState.dwindlePressed or btnState.enlargePressed ) {
		repaint();
		QWidget::mouseMoveEvent( mEvent );

		return;
	}

	if ( dwindleRect.adjusted( -2, -2, 2, 2 ).contains( mEvent->localPos() ) )
		btnState = { true, isDwindleEnabled, false, false, false };

	else if ( enlargeRect.adjusted( -2, -2, 2, 2 ).contains( mEvent->localPos() ) )
		btnState = { true, false, isEnlargeEnabled, false, false };

	else
		btnState = { true, false, false, false, false };

	repaint();
	QWidget::mouseMoveEvent( mEvent );
};

void Zoom::mouseReleaseEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		// dwindle-button was pressed and release
		if ( dwindleRect.adjusted( -2, -2, 2, 2 ).contains( mEvent->localPos() ) ) {
			if ( btnState.dwindlePressed and isDwindleEnabled )
				emit clicked( "dwindle" );
		}

		else if ( enlargeRect.adjusted( -2, -2, 2, 2 ).contains( mEvent->localPos() ) ) {
			if ( btnState.enlargePressed and isEnlargeEnabled )
				emit clicked( "enlarge" );
		}
	}

	btnState = { true, false, false, false, false };

	repaint();
	mEvent->accept();
};

void Zoom::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing );

	painter.fillRect( rect().adjusted( 1, 1, -1, -1), Qt::transparent );

	QColor borderClr = palette().color( QPalette::Shadow );
	QColor bgClr = palette().color( QPalette::Window );

	if ( btnState.mouseIn ) {
		/* Background + Border */
		painter.save();
		painter.setPen( Qt::NoPen );
		painter.setBrush( bgClr );
		painter.drawRoundedRect( QRectF( rect() ), 3, 3 );
		painter.restore();

		/* Separator */
		painter.save();
		painter.setRenderHint( QPainter::Antialiasing, false );
		painter.setPen( borderClr );
		painter.drawLine( QPoint( width() / 2, 5 ), QPoint( width() / 2, height() - 6 ) );
		painter.restore();

		/* Highlight */
		painter.save();
		painter.setOpacity( 0.25 );
		if ( btnState.dwindlePressed or btnState.enlargePressed )
			painter.setOpacity( 0.35 );

		if ( btnState.inDwindle ) {
			painter.setPen( Qt::transparent );
			painter.setBrush( palette().color( QPalette::Highlight ) );
			painter.drawRoundedRect( dwindleRect.adjusted( -2, -2, 2, 2 ), 3, 3 );
		}

		if ( btnState.inEnlarge ) {
			painter.setPen( Qt::transparent );
			painter.setBrush( palette().color( QPalette::Highlight ) );
			painter.drawRoundedRect( enlargeRect.adjusted( -2, -2, 2, 2 ), 3, 3 );
		}

		else {
			// Nothing
		}
		painter.restore();

		/* Dwindle and Enlarge icons */
		painter.save();
		painter.drawPixmap( dwindleRect.topLeft(), dwindleIcon );
		painter.drawPixmap( enlargeRect.topLeft(), enlargeIcon );
		painter.restore();
	}

	else {
		// Draw at 21% opacity
		borderClr.setAlphaF( 0.21 );
		bgClr.setAlphaF( 0.21 );

		/* Background + Border */
		painter.save();
		painter.setPen( Qt::NoPen );
		painter.setBrush( bgClr );
		painter.drawRoundedRect( QRectF( rect() ).adjusted( 1, 1, -1, -1 ), 3, 3 );
		painter.restore();

		/* Separator */
		painter.save();
		painter.setRenderHint( QPainter::Antialiasing, false );
		painter.setPen( borderClr );
		painter.drawLine( QPoint( width() / 2, 5 ), QPoint( width() / 2, height() - 6 ) );
		painter.restore();

		/* Dwindle and Enlarge icons */
		painter.save();
		painter.setOpacity( 0.21 );
		painter.drawPixmap( dwindleRect.topLeft(), dwindleIcon );
		painter.drawPixmap( enlargeRect.topLeft(), enlargeIcon );
		painter.restore();
	}

	painter.end();

	QWidget::paintEvent( pEvent );
};

PageWidget::PageWidget( QWidget *parent ) : QWidget( parent ) {

	setFixedSize( QSize( 72, 48 ) );

	btnState = { false, false, false, false, false, false, false };

	previousRect = QRectF( 5, 2, 28, 24 );
	nextRect     = QRectF( 38, 2, 28, 24 );
	textRect     = QRectF( 2, 26, 68, 20 );

	isPreviousEnabled = false;
	isNextEnabled     = false;

	setMouseTracking( true );

	previousIcon = QIcon::fromTheme( "arrow-up" ).pixmap( QSize( 24, 24 ), QIcon::Normal );
	nextIcon     = QIcon::fromTheme( "arrow-down" ).pixmap( QSize( 24, 24 ), QIcon::Normal );
};

void PageWidget::setCurrentPage( int page ) {

	curPage = page;
	isPreviousEnabled = ( curPage != 0 );
	isNextEnabled = ( curPage < maxPages - 1 );

	repaint();
};

void PageWidget::setMaximumPages( int pages ) {

	maxPages = pages;
	isPreviousEnabled = ( curPage != 0 );
	isNextEnabled = ( curPage < maxPages - 1 );

	repaint();
};

void PageWidget::enterEvent( QEvent *event ) {

	btnState.mouseIn = true;

	repaint();
	event->accept();
};

void PageWidget::leaveEvent( QEvent *event ) {

	btnState.mouseIn = false;

	repaint();
	event->accept();
};

void PageWidget::mousePressEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		// Left half = previous button pressed
		if ( previousRect.contains( mEvent->localPos() ) )
			btnState = { true, isPreviousEnabled, false, false, isPreviousEnabled, false, false };

		else if ( nextRect.contains( mEvent->localPos() ) )
			btnState = { true, false, isNextEnabled, false, false, isNextEnabled, false };

		else if ( textRect.contains( mEvent->localPos() ) )
			btnState = { true, false, false, true, false, false, true };

		else
			btnState = { true, false, false, false, false, false, false };
	}

	repaint();
	mEvent->accept();
};

void PageWidget::mouseMoveEvent( QMouseEvent *mEvent ) {

	if ( btnState.previousPressed or btnState.nextPressed ) {
		repaint();
		QWidget::mouseMoveEvent( mEvent );

		return;
	}

	if ( previousRect.contains( mEvent->localPos() ) )
		btnState = { true, isPreviousEnabled, false, false, false, false, false };

	else if ( nextRect.contains( mEvent->localPos() ) )
		btnState = { true, false, isNextEnabled, false, false, false, false };

	else if ( textRect.contains( mEvent->localPos() ) )
		btnState = { true, false, false, true, false, false, true };

	else
		btnState = { true, false, false, false, false, false, false };

	repaint();
	QWidget::mouseMoveEvent( mEvent );
};

void PageWidget::mouseReleaseEvent( QMouseEvent *mEvent ) {

	if ( mEvent->button() == Qt::LeftButton ) {
		// previous-button was pressed and release
		if ( previousRect.contains( mEvent->localPos() ) ) {
			if ( btnState.previousPressed and isPreviousEnabled ) {
				curPage--;
				if ( curPage < 0 )
					curPage = 0;

				isPreviousEnabled = ( curPage != 0 );
				emit loadPage( curPage );
			}
		}

		else if ( nextRect.contains( mEvent->localPos() ) ) {
			if ( btnState.nextPressed and isNextEnabled ) {
				curPage++;
				if ( curPage >= maxPages )
					curPage = maxPages - 1;

				isNextEnabled = ( curPage < maxPages - 1 );
				emit loadPage( curPage );
			}
		}

		else if ( textRect.contains( mEvent->localPos() ) ) {
			QInputDialog *inDlg = new QInputDialog( this );
			inDlg->setWindowTitle( "DesQDocs | Goto Page" );
			inDlg->setLabelText( "Enter the page to which you want to jump:" );
			inDlg->setInputMode( QInputDialog::IntInput );
			inDlg->setIntRange( 1, maxPages );
			inDlg->setIntValue( curPage + 1 );
			inDlg->setIntStep( 1 );

			QSpinBox *sb = inDlg->findChild<QSpinBox *>();
			if ( sb )
				sb->setSuffix( QString( " of %1 pages" ).arg( maxPages ) );

			if ( inDlg->exec() )
				curPage = inDlg->intValue() - 1;

			emit loadPage( curPage );
		}
	}

	btnState = { true, false, false, false, false, false, false };

	repaint();
	mEvent->accept();
};

void PageWidget::paintEvent( QPaintEvent *pEvent ) {

	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing );

	painter.fillRect( rect(), Qt::transparent );

	QColor borderClr = palette().color( QPalette::Shadow );
	QColor bgClr = palette().color( QPalette::Window );

	if ( btnState.mouseIn ) {
		/* Background + Border */
		painter.save();
		painter.setPen( Qt::NoPen );
		painter.setBrush( bgClr );
		painter.drawRoundedRect( QRectF( rect() ), 3, 3 );
		painter.restore();

		/* Separator */
		painter.save();
		painter.setRenderHint( QPainter::Antialiasing, false );
		painter.setPen( borderClr );
		painter.drawLine( QPoint( 35, 5 ), QPoint( 35, 22 ) );
		painter.restore();

		/* Highlight */
		painter.save();
		painter.setOpacity( 0.25 );
		if ( btnState.previousPressed or btnState.nextPressed )
			painter.setOpacity( 0.35 );

		if ( btnState.inPrevious ) {
			painter.setPen( Qt::transparent );
			painter.setBrush( palette().color( QPalette::Highlight ) );
			painter.drawRoundedRect( previousRect, 3, 3 );
		}

		if ( btnState.inNext ) {
			painter.setPen( Qt::transparent );
			painter.setBrush( palette().color( QPalette::Highlight ) );
			painter.drawRoundedRect( nextRect, 3, 3 );
		}

		if ( btnState.inText ) {
			painter.setPen( Qt::transparent );
			painter.setBrush( palette().color( QPalette::Highlight ) );
			painter.drawRoundedRect( textRect, 3, 3 );
		}

		painter.restore();

		/* Previous and Next icons */
		painter.save();
		painter.drawPixmap( previousRect.topLeft() + QPointF( 2.0, 0 ), previousIcon );
		painter.drawPixmap( nextRect.topLeft() + QPointF( 2.0, 0 ), nextIcon );
		painter.drawText( textRect, Qt::AlignCenter, QString( "%1 / %2" ).arg( curPage + 1 ).arg( maxPages ) );
		painter.restore();
	}

	else {
		// Draw at 21% opacity
		painter.setOpacity( 0.1 );

		/* Background + Border */
		painter.save();
		painter.setPen( Qt::NoPen );
		painter.setBrush( bgClr );
		painter.drawRoundedRect( QRectF( rect() ), 3, 3 );
		painter.restore();

		/* Separator */
		painter.save();
		painter.setRenderHint( QPainter::Antialiasing, false );
		painter.setPen( borderClr );
		painter.drawLine( QPointF( 35, 5 ), QPointF( 35, 22 ) );
		painter.restore();

		/* Previous and Next icons */
		painter.save();
		painter.drawPixmap( previousRect.topLeft() + QPointF( 2.0, 0 ), previousIcon );
		painter.drawPixmap( nextRect.topLeft() + QPointF( 2.0, 0 ), nextIcon );
		painter.restore();

		/* Page numbers */
		painter.save();
		painter.setOpacity( 0.63 );
		painter.setPen( Qt::NoPen );
		painter.setBrush( bgClr );
		painter.drawRoundedRect( textRect, 3, 3 );
		painter.setPen( borderClr );
		painter.drawText( textRect, Qt::AlignCenter, QString( "%1 / %2" ).arg( curPage + 1 ).arg( maxPages ) );
		painter.restore();
	}

	painter.end();

	QWidget::paintEvent( pEvent );
};
