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

#ifndef PDFVIEWER_ACTIONHANDLER_H
#define PDFVIEWER_ACTIONHANDLER_H

#include "pdfview.h"
#include <QObject>

class QAction;
class QActionGroup;
class SelectPageAction;
class ZoomAction;

class ActionHandler : public QObject
{
	Q_OBJECT

public:
	explicit ActionHandler(QWidget *parent = 0);
	~ActionHandler();

	QAction *action(PdfView::PdfViewAction action, QObject *receiver, const char *slot);
	QAction *action(PdfView::PdfViewAction action); // to be used to access an action which is already initialized
	void updateZoomActions(qreal zoomFactor, qreal minZoomFactor, qreal maxZoomFactor);
	void setPageLabels(QStringList popplerPageLabels);
	void toggleFileDependentActionsEnabled(bool enabled);
	void toggleGoToActionsEnabled(bool enabled, int pageNumber = -1, int minPageNumber = -1, int maxPageNumber = -1, int scrollPosition = -1, int minScrollPosition = -1, int maxScrollPosition = -1);

private:
	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;
	ZoomAction *m_zoomAction;
	QAction *m_goToStartAction;
	QAction *m_goToEndAction;
	QAction *m_goToPreviousPageAction;
	QAction *m_goToNextPageAction;
	SelectPageAction *m_goToPageAction;
	QAction *m_printAction;
	QActionGroup *m_mouseToolsActionGroup;
	QAction *m_mouseBrowseAction;
	QAction *m_mouseMagnifyAction;
	QAction *m_mouseSelectionAction;
	QAction *m_mouseTextSelectionAction;
	QAction *m_showFormsAction;

	QObject *m_goToPageReceiver;
	QObject *m_zoomReceiver;
	char *m_goToPageSlot;
	char *m_zoomSlot;
};

#endif // PDFVIEWER_PRINTHANDLER_H
