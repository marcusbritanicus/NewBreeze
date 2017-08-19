/*
 * Copyright (C) 2012, Glad Deschrijver <glad.deschrijver@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PDFVIEWER_PDFVIEW_P_H
#define PDFVIEWER_PDFVIEW_P_H

#include "pdfview.h"
#include "pageitem.h"
#include "utils/bookmarkshandler.h"
#if QT_VERSION >= 0x050000
	#include <poppler-qt5.h>
#else
	#include <poppler-qt4.h>
#endif

//class PdfView;
//class PageItem;
//class BookmarksHandler;
class ActionHandler;
class PrintHandler;
class ZoomAction;
class QAction;
class QGraphicsRectItem;
class QTimer;
class QToolBar;
class QVBoxLayout;

class PdfViewPrivate : public QObject, public BookmarksWidget
{
    Q_OBJECT
//	Q_DECLARE_PUBLIC(PdfView)

public:
	PdfViewPrivate(PdfView *pdfView);
	~PdfViewPrivate();

	void init();
	QWidget *widget();
	double position() const;
//	void createActions();
	double scaleFactorX() const;
	double scaleFactorY() const;
	void showForms(PageItem *pageItem, int pageNumber);
	void loadDocument();
	void closeDocument();
	void loadPage(int pageNumber);
	void loadVisiblePages(int pageNumberStart, int pageNumberEnd);
	void clearPages();
	void clearNonVisiblePages(int pageNumberStart, int pageNumberEnd);
	void removeFindHighlight();
	void scrollPositionChanged();
	void magnify(const QPointF &scenePos);
	void endMagnify();
	void findLinkAtPosition(const QPointF &scenePos);
	int pageNumberAtPosition(const QPointF &scenePos);
	void handleSelection(const QPoint &popupMenuPos);
	void addSelectionRect(const QPointF &scenePos);
	void removeSelectionRect();
	void getTextSelection(const QPointF &scenePos);
	void handleTextSelection(const QPoint &popupMenuPos);
	void removeTextSelection();
	void scroll(int delta);

public Q_SLOTS:
	void slotSelectMouseTool();
	void slotSetPage(int pageNumber);
	void slotSetPage(double pageNumber);
	void slotSetZoomFactor(qreal value);
	void slotVerticalPositionChanged(int value);
	void slotVerticalPositionChanged();

Q_SIGNALS:
	void scrollPositionChanged(qreal fraction, int pageNumber);
	void openTexDocument(const QString &fileName, int lineNumber);
	void mouseToolChanged(PdfView::MouseTool tool);

public:
	PdfView *q;
	QGraphicsScene *m_pageScene;
	QList<PageItem*> m_pageItems;

	ActionHandler *m_actionHandler;

	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;
	ZoomAction *m_zoomAction;
	int m_maxFileSettingsCacheSize;
	BookmarksHandler *m_bookmarksHandler;
	QList<QAction*> m_contextMenuActions;

	QString m_fileName;
	Poppler::Document *m_popplerDocument;
	QList<Poppler::Page*> m_popplerPages;
	QStringList m_popplerPageLabels;
	QList<int> m_popplerPageTopPositions;
	QList<bool> m_pageLoaded;
	qreal m_zoomFactor;
	qreal m_maximumPageWidth;
	int m_dpiX;
	int m_dpiY;
	int m_pageNumber;
	Poppler::Document::RenderBackend m_renderBackend;
	int m_renderHints;

	double m_findPositionTop;
	double m_findPositionLeft;
	double m_findPositionRight;
	double m_findPositionBottom;
	int m_realPageNumber;
	QGraphicsRectItem *m_findHighlightRect;

	PdfView::MouseTool m_mouseTool;
	bool m_isDragging;
	QGraphicsRectItem *m_magnifiedPageRect;
	QGraphicsPixmapItem *m_magnifiedPageItem;
	QPoint m_mouseOldPos;
	QGraphicsRectItem *m_selectionRect;
	QPointF m_selectionStart;
	QList<QGraphicsRectItem*> m_textSelectionRects;
	QStringList m_textSelectionWords;

	bool m_showForms;

	QTimer *m_verticalPositionTimer;

	PrintHandler *m_printHandler;
};

#endif // PDFVIEWER_PDFVIEW_P_H
