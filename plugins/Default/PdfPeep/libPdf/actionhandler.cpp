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

#include "actionhandler.h"

#include "utils/selectpageaction.h"
#include "utils/zoomaction.h"

#include <QAction>

ActionHandler::ActionHandler(QWidget *parent)
	: QObject(parent)
	, m_zoomInAction(0)
	, m_zoomOutAction(0)
	, m_zoomAction(0)
	, m_goToStartAction(0)
	, m_goToEndAction(0)
	, m_goToPreviousPageAction(0)
	, m_goToNextPageAction(0)
	, m_goToPageAction(0)
	, m_printAction(0)
	, m_mouseToolsActionGroup(0)
	, m_mouseBrowseAction(0)
	, m_mouseMagnifyAction(0)
	, m_mouseSelectionAction(0)
	, m_mouseTextSelectionAction(0)
	, m_showFormsAction(0)
{
}

ActionHandler::~ActionHandler()
{
}

/*******************************************************************/

QAction *ActionHandler::action(PdfView::PdfViewAction action, QObject *receiver, const char *slot)
{
	switch (action)
	{
		case PdfView::ZoomIn:
			if (!m_zoomInAction)
			{
				m_zoomInAction = new QAction(tr("Zoom &In", "Action"), this);
#ifndef QT_NO_SHORTCUT
				m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
#endif
				m_zoomInAction->setObjectName(QLatin1String("pdfview_zoom_in"));
				connect(m_zoomInAction, SIGNAL(triggered()), receiver, slot);
			}
			return m_zoomInAction;
			break;

		case PdfView::ZoomOut:
			if (!m_zoomOutAction)
			{
				m_zoomOutAction = new QAction(tr("Zoom &Out", "Action"), this);
#ifndef QT_NO_SHORTCUT
				m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
#endif
				m_zoomOutAction->setObjectName(QLatin1String("pdfview_zoom_out"));
				connect(m_zoomOutAction, SIGNAL(triggered()), receiver, slot);
			}
			return m_zoomOutAction;
			break;

		case PdfView::Zoom:
			if (!m_zoomAction)
			{
				m_zoomAction = new ZoomAction(tr("&Zoom", "Action"), this);
				connect(m_zoomAction, SIGNAL(zoomFactorAdded(qreal)), receiver, slot);
				m_zoomReceiver = receiver;
				m_zoomSlot = new char[strlen(slot) + 1];
				strcpy(m_zoomSlot, slot);
//				m_zoomSlot = (char *)slot; // XXX this seems to work, if an unknown crash occurs, replacing this by the above might be a better idea
			}
			return m_zoomAction;
			break;

		case PdfView::GoToStartOfDocument:
			if (!m_goToStartAction)
			{
				m_goToStartAction = new QAction(tr("&Beginning of the Document", "Action"), this);
				m_goToStartAction->setIconText(tr("Start", "Action icon text: go to start of document"));
#ifndef QT_NO_SHORTCUT
				m_goToStartAction->setShortcut(QKeySequence::MoveToStartOfDocument);
#endif
				m_goToStartAction->setObjectName(QLatin1String("pdfview_go_start_document"));
				connect(m_goToStartAction, SIGNAL(triggered()), receiver, slot);
			}
			return m_goToStartAction;
			break;

		case PdfView::GoToEndOfDocument:
			if (!m_goToEndAction)
			{
				m_goToEndAction = new QAction(tr("&End of the Document", "Action"), this);
				m_goToEndAction->setIconText(tr("End", "Action icon text: go to end of document"));
#ifndef QT_NO_SHORTCUT
				m_goToEndAction->setShortcut(QKeySequence::MoveToEndOfDocument);
#endif
				m_goToEndAction->setObjectName(QLatin1String("pdfview_go_end_document"));
				connect(m_goToEndAction, SIGNAL(triggered()), receiver, slot);
			}
			return m_goToEndAction;
			break;

		case PdfView::GoToPreviousPage:
			if (!m_goToPreviousPageAction)
			{
				m_goToPreviousPageAction = new QAction(tr("&Previous Page", "Action"), this);
				m_goToPreviousPageAction->setIconText(tr("Previous", "Action icon text: go to previous page"));
				m_goToPreviousPageAction->setObjectName(QLatin1String("pdfview_go_previous_page"));
				connect(m_goToPreviousPageAction, SIGNAL(triggered()), receiver, slot);
			}
			return m_goToPreviousPageAction;
			break;

		case PdfView::GoToNextPage:
			if (!m_goToNextPageAction)
			{
				m_goToNextPageAction = new QAction(tr("&Next Page", "Action"), this);
				m_goToNextPageAction->setIconText(tr("Next", "Action icon text: go to next page"));
				m_goToNextPageAction->setObjectName(QLatin1String("pdfview_go_next_page"));
				connect(m_goToNextPageAction, SIGNAL(triggered()), receiver, slot);
			}
			return m_goToNextPageAction;
			break;

		case PdfView::GoToPage:
			if (!m_goToPageAction)
			{
				m_goToPageAction = new SelectPageAction(this);
//				connect(m_goToPageAction, SIGNAL(pageSelected(int)), receiver, slot);
				m_goToPageReceiver = receiver;
				m_goToPageSlot = new char[strlen(slot) + 1];
				strcpy(m_goToPageSlot, slot);
//				m_goToPageSlot = (char *)slot; // XXX this seems to work, if an unknown crash occurs, replacing this by the above might be a better idea
			}
			return m_goToPageAction;
			break;

		case PdfView::Print:
			if (!m_printAction)
			{
				m_printAction = new QAction(tr("&Print...", "Action"), this);
#ifndef QT_NO_SHORTCUT
				m_printAction->setShortcut(QKeySequence::Print);
#endif
				m_printAction->setObjectName(QLatin1String("pdfview_print"));
				connect(m_printAction, SIGNAL(triggered()), receiver, slot);
			}
			return m_printAction;
			break;

		case PdfView::MouseToolBrowse:
			if (!m_mouseToolsActionGroup)
				m_mouseToolsActionGroup = new QActionGroup(this);
			if (!m_mouseBrowseAction)
			{
				m_mouseBrowseAction = new QAction(tr("&Browse Tool", "Action: choose mouse tool"), this);
				m_mouseBrowseAction->setCheckable(true);
				m_mouseBrowseAction->setData(qVariantFromValue(PdfView::Browsing));
				m_mouseBrowseAction->setObjectName(QLatin1String("pdfview_mouse_browse"));
				connect(m_mouseBrowseAction, SIGNAL(triggered()), receiver, slot);
				m_mouseToolsActionGroup->addAction(m_mouseBrowseAction);
			}
			return m_mouseBrowseAction;
			break;

		case PdfView::MouseToolMagnify:
			if (!m_mouseToolsActionGroup)
				m_mouseToolsActionGroup = new QActionGroup(this);
			if (!m_mouseMagnifyAction)
			{
				m_mouseMagnifyAction = new QAction(tr("&Magnify Tool", "Action: choose mouse tool"), this);
				m_mouseMagnifyAction->setCheckable(true);
				m_mouseMagnifyAction->setData(qVariantFromValue(PdfView::Magnifying));
				m_mouseMagnifyAction->setObjectName(QLatin1String("pdfview_mouse_magnify"));
				connect(m_mouseMagnifyAction, SIGNAL(triggered()), receiver, slot);
				m_mouseToolsActionGroup->addAction(m_mouseMagnifyAction);
			}
			return m_mouseMagnifyAction;
			break;

		case PdfView::MouseToolSelection:
			if (!m_mouseToolsActionGroup)
				m_mouseToolsActionGroup = new QActionGroup(this);
			if (!m_mouseSelectionAction)
			{
				m_mouseSelectionAction = new QAction(tr("&Selection Tool", "Action: choose mouse tool"), this);
				m_mouseSelectionAction->setCheckable(true);
				m_mouseSelectionAction->setData(qVariantFromValue(PdfView::Selection));
				m_mouseSelectionAction->setObjectName(QLatin1String("pdfview_mouse_selection"));
				connect(m_mouseSelectionAction, SIGNAL(triggered()), receiver, slot);
				m_mouseToolsActionGroup->addAction(m_mouseSelectionAction);
			}
			return m_mouseSelectionAction;
			break;

		case PdfView::MouseToolTextSelection:
			if (!m_mouseToolsActionGroup)
				m_mouseToolsActionGroup = new QActionGroup(this);
			if (!m_mouseTextSelectionAction)
			{
				m_mouseTextSelectionAction = new QAction(tr("&Text Selection Tool", "Action: choose mouse tool"), this);
				m_mouseTextSelectionAction->setCheckable(true);
				m_mouseTextSelectionAction->setData(qVariantFromValue(PdfView::TextSelection));
				m_mouseTextSelectionAction->setObjectName(QLatin1String("pdfview_mouse_text_selection"));
				connect(m_mouseTextSelectionAction, SIGNAL(triggered()), receiver, slot);
				m_mouseToolsActionGroup->addAction(m_mouseTextSelectionAction);
			}
			return m_mouseTextSelectionAction;
			break;

		case PdfView::ShowForms:
			if (!m_showFormsAction)
			{
				m_showFormsAction = new QAction(tr("S&how Forms", "Action"), this);
				m_showFormsAction->setObjectName(QLatin1String("pdfview_show_forms"));
				m_showFormsAction->setCheckable(true);
				connect(m_showFormsAction, SIGNAL(toggled(bool)), receiver, slot);
			}
			return m_showFormsAction;
			break;

		case PdfView::NoPdfViewAction:
		default:
			return 0;
	}
	return 0;
}

QAction *ActionHandler::action(PdfView::PdfViewAction action)
{
	switch (action)
	{
		case PdfView::ZoomIn: return m_zoomInAction; break;
		case PdfView::ZoomOut: return m_zoomOutAction; break;
		case PdfView::Zoom: return m_zoomAction; break;
		case PdfView::GoToStartOfDocument: return m_goToStartAction; break;
		case PdfView::GoToEndOfDocument: return m_goToEndAction; break;
		case PdfView::GoToPreviousPage: return m_goToPreviousPageAction; break;
		case PdfView::GoToNextPage: return m_goToNextPageAction; break;
		case PdfView::GoToPage: return m_goToPageAction; break;
		case PdfView::Print: return m_printAction; break;
		case PdfView::MouseToolBrowse: return m_mouseBrowseAction; break;
		case PdfView::MouseToolMagnify: return m_mouseMagnifyAction; break;
		case PdfView::MouseToolSelection: return m_mouseSelectionAction; break;
		case PdfView::MouseToolTextSelection: return m_mouseTextSelectionAction; break;
		case PdfView::ShowForms: return m_showFormsAction; break;
		case PdfView::NoPdfViewAction:
		default:
			return 0;
	}
	return 0;
}

/*******************************************************************/

void ActionHandler::updateZoomActions(qreal zoomFactor, qreal minZoomFactor, qreal maxZoomFactor)
{
	if (m_zoomInAction)
		m_zoomInAction->setEnabled(zoomFactor < maxZoomFactor);
	if (m_zoomOutAction)
		m_zoomOutAction->setEnabled(zoomFactor > minZoomFactor);
	if (m_zoomAction)
	{
		disconnect(m_zoomAction, SIGNAL(zoomFactorAdded(qreal)), m_zoomReceiver, m_zoomSlot);
		m_zoomAction->setZoomFactor(zoomFactor);
		connect(m_zoomAction, SIGNAL(zoomFactorAdded(qreal)), m_zoomReceiver, m_zoomSlot);
	}
}

void ActionHandler::setPageLabels(QStringList popplerPageLabels)
{
	if (m_goToPageAction)
		m_goToPageAction->setPageLabels(popplerPageLabels);
}

void ActionHandler::toggleFileDependentActionsEnabled(bool enabled)
{
// XXX AFAICS the following must not be done here since it already happens in toggleGoToActionsEnabled() which is also called when a file is loaded
//	if (m_goToPageAction)
//	{
//		m_goToPageAction->setEnabled(enabled);
//		disconnect(m_goToPageAction, SIGNAL(pageSelected(int)), m_goToPageReceiver, m_goToPageSlot);
//		m_goToPageAction->setCurrentIndex(pageNumber);
//		connect(m_goToPageAction, SIGNAL(pageSelected(int)), m_goToPageReceiver, m_goToPageSlot);
//	}
	if (m_printAction)
		m_printAction->setEnabled(enabled);
	if (m_showFormsAction)
		m_showFormsAction->setEnabled(enabled);
}

void ActionHandler::toggleGoToActionsEnabled(bool enabled, int pageNumber, int minPageNumber, int maxPageNumber, int scrollPosition, int minScrollPosition, int maxScrollPosition)
{
	if (m_goToPreviousPageAction)
		m_goToPreviousPageAction->setEnabled(enabled && pageNumber > minPageNumber);
	if (m_goToNextPageAction)
		m_goToNextPageAction->setEnabled(enabled && pageNumber < maxPageNumber);
	if (m_goToStartAction)
		m_goToStartAction->setEnabled(enabled && scrollPosition > minScrollPosition);
	if (m_goToEndAction)
		m_goToEndAction->setEnabled(enabled && scrollPosition < maxScrollPosition);
	if (m_goToPageAction)
	{
		disconnect(m_goToPageAction, SIGNAL(pageSelected(int)), m_goToPageReceiver, m_goToPageSlot);
		if (enabled)
			m_goToPageAction->setCurrentIndex(pageNumber);
		else
			m_goToPageAction->clear();
		connect(m_goToPageAction, SIGNAL(pageSelected(int)), m_goToPageReceiver, m_goToPageSlot);
		m_goToPageAction->setEnabled(enabled);
	}
}
