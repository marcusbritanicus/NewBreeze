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

#ifndef PDFVIEWER_PDFVIEW_H
#define PDFVIEWER_PDFVIEW_H

#include <QGraphicsView>
#include <QInputDialog>
#if QT_VERSION >= 0x050000
	#include <poppler-qt5.h>
#else
	#include <poppler-qt4.h>
#endif

class PdfViewPrivate;

class PdfPasswdInput : public QInputDialog {
	Q_OBJECT

	public:
		PdfPasswdInput( QWidget *parent );
		static QString getText( QWidget *parent, QString title, QString label );
};

class PdfView : public QGraphicsView
{
	Q_OBJECT

	Q_PROPERTY(QString fileName READ fileName)
	Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
	Q_PROPERTY(double pageNumber READ pageNumberWithPosition WRITE setPage)
	Q_PROPERTY(QStringList pageLabels READ popplerPageLabels)
	Q_ENUMS(MouseTool PdfViewAction)

public:
	/**
	 * \enum PdfView::MouseTool
	 * This enum describes the possible tools that can be used when
	 * the mouse is dragged.
	 * \value Browsing Dragging the mouse changes the position of the
	 * pages in the view.
	 * \value Magnifying Clicking and dragging the mouse shows a magnifying
	 * window which zooms the area in the immediate neighborhood of the
	 * mouse cursor.
	 * \value Selection Dragging the mouse selects a rectangular area which
	 * can be copied as an image or as text to the clipboard or saved as an
	 * image on disk.
	 * \value TextSelection Dragging the mouse selects continuous text which
	 * can be copied to the clipboard.
	 */
	enum MouseTool {
		Browsing = 0,
		Magnifying = 1,
		Selection = 2,
		TextSelection = 3
	};
	/**
	 * \enum PdfView::FindFlag
	 * This enum describes the options available to PdfView's find function.
	 * \value FindBackward Search backwards instead of forwards.
	 * \value FindCaseSensitively By default find works case insensitive.
	 * Specifying this option changes the behavior to a case sensitive
	 * find operation.
	 */
	enum FindFlag {
		FindBackward = 0x00001,
		FindCaseSensitively = 0x00002
	};
	Q_DECLARE_FLAGS(FindFlags, FindFlag)
	enum PositionHandling {
		DontKeepPosition = 0,
		DontUpdatePosition = 1,
		KeepPosition = 2,
		ReloadPage = 3
	};
	/**
	 * \enum PdfView::PdfViewAction
	 * This enum describes the different actions which can be performed on
	 * the PDF view.
	 * \value NoPdfViewAction No action is triggered
	 * \value ZoomIn Zoom the current document in by one step.
	 * \value ZoomOut Zoom the current document in by one step.
	 * \value Zoom This is a QWidgetAction which allows to select the zoom factor.
	 * \value GoToStartOfDocument Go to the start of the document
	 * \value GoToEndOfDocument Go to the end of the document
	 * \value GoToPreviousPage Go to the previous page
	 * \value GoToNextPage Go to the next page
	 * \value GoToPage This is a QWidgetAction which allows to select the page number to go to.
	 * \value Bookmarks This is an action which has a submenu containing the bookmark related actions.
	 * \value SetBookmark Set/unset the current position as a bookmark.
	 * \value PreviousBookmark Go the the previous bookmark.
	 * \value NextBookmark Go the the next bookmark.
	 */
	enum PdfViewAction {
		NoPdfViewAction = -1,
		ZoomIn,
		ZoomOut,
		Zoom,
		GoToStartOfDocument,
		GoToEndOfDocument,
		GoToPreviousPage,
		GoToNextPage,
		GoToPage,
		Bookmarks,
		SetBookmark,
		PreviousBookmark,
		NextBookmark,
		Print,
		MouseToolBrowse,
		MouseToolMagnify,
		MouseToolSelection,
		MouseToolTextSelection,
		ShowForms
	};

	explicit PdfView(QWidget *parent = 0);
	~PdfView();

	void setMouseTool(MouseTool mouseTool);
	void setMaximumFileSettingsCacheSize(int size);

	/**
	 * Returns a QAction for the specified PdfViewAction <em>action</em>.
	 *
	 * The action is owned by PdfView but you can customize the look by
	 * changing its properties.
	 *
	 * PdfView also takes care of implementing the action, so that upon
	 * triggering the corresponding action is performed on the view.
	 */
	QAction *action(PdfViewAction action);

	/**
	 * This function maps a point in page coordinates to scene coordinates.
	 * If <em>pageNumber</em> is not a valid page number, then a null point
	 * is returned, see QPointF::isNull().
	 * \param pageNumber the number of the page to which the coordinates refer
	 * \param pagePos the point in page coordinates
	 * \return the point in scene coordinates
	 */
	QPointF mapFromPage(int pageNumber, const QPointF &pagePos) const;
	/**
	 * This function maps a rectangle in page coordinates to scene coordinates.
	 * If <em>pageNumber</em> is not a valid page number, then a null rectangle
	 * is returned, see QRectF::isNull().
	 * \param pageNumber the number of the page to which the coordinates refer
	 * \param pageRect the rectangle in page coordinates
	 * \return the rectangle in scene coordinates
	 */
	QRectF mapFromPage(int pageNumber, const QRectF &pageRect) const;
	/**
	 * This function maps a point in scene coordinates to page coordinates.
	 * If <em>pageNumber</em> is not a valid page number, then a null point
	 * is returned, see QPointF::isNull().
	 * \param pageNumber the number of the page to which the coordinates refer
	 * \param scenePos the point in scene coordinates
	 * \return the point in page coordinates
	 */
	QPointF mapToPage(int pageNumber, const QPointF &scenePos) const;
	/**
	 * This function maps a rectangle in scene coordinates to page coordinates.
	 * If <em>pageNumber</em> is not a valid page number, then a null rectangle
	 * is returned, see QRectF::isNull().
	 * \param pageNumber the number of the page to which the coordinates refer
	 * \param sceneRect the rectangle in scene coordinates
	 * \return the rectangle in page coordinates
	 */
	QRectF mapToPage(int pageNumber, const QRectF &sceneRect) const;
	/**
	 * This function maps a point in scene coordinates to the current page's
	 * coordinates, the current page being the page that is visible at the
	 * <em>scenePos</em>'s coordinates.
	 * If the calculated current page's number is not a valid page number,
	 * then a null point is returned, see QPointF::isNull().
	 * \param scenePos the point in scene coordinates
	 * \return the point in page coordinates
	 */
	QPointF mapToCurrentPage(const QPointF &scenePos) const;

	void setRenderBackend(Poppler::Document::RenderBackend backend);
	Poppler::Document::RenderBackend renderBackend() const;
	void setRenderHint(Poppler::Document::RenderHint hint, bool on = true);
	Poppler::Document::RenderHints renderHints() const;

	bool load(const QString &fileName);
	QString fileName() const;
	Poppler::Document *document() const;
//	void loadFileSpecificSettings();
	void close();
	QList<Poppler::Page*> popplerPages();
	QStringList popplerPageLabels();

	/*virtual*/ void setPage(double pageNumber, PositionHandling keepPosition = DontKeepPosition);
	/**
	 * This function returns a value containing the number of the current
	 * page together with the fraction of this page on which the top of the
	 * currently visible area is located.
	 */
	double pageNumberWithPosition() const;

	/**
	 * This function searches for <em>text</em> in the view, starting in
	 * the currently visible area of the text, taking into consideration
	 * the search flags <em>flags</em>.
	 * \param text the text to be searched in the document
	 * \param flags the search flags determining the type of the search
	 */
	void search(const QString &text, const FindFlags &flags);
	/**
	 * This function returns the string for a given page
	 * \param text the text to be searched in the document
	 * \param flags the search flags determining the type of the search
	 */
	QString pageText(int pageno);
	/**
	 * This function sets the zoom factor in the view to <em>value</em>.
	 * The value of <em>value</em> should be between 0.1 and 6. For example,
	 * a value of 1.0 corresponds to a zoom factor of 100%.
	 * \param value the new zoom factor for the view
	 */
	void setZoomFactor(qreal value);
	/**
	 * This function returns the current zoom factor in the view.
	 * \return the zoom factor in the view
	 */
	qreal zoomFactor() const;
	/**
	 * This function sets the zoom factor in the view such that maximum page
	 * width will be <em>value</em>. Max zoom factor is fixed at 6.
	 * The value of <em>value</em> should be between 0.1 and 6. For example,
	 * a value of 1.0 corresponds to a zoom factor of 100%.
	 * \param value the new page width for the view
	 */
	void setMaximumPageWidth(int value);

	/**
	 * This function adds <em>action</em> to the list of actions that
	 * are available in the context menu of the view.
	 * \param action the action to be added to the view's context menu
	 */
	void addContextMenuAction(QAction *action);
	/**
	 * This function removes <em>action</em> from the list of actions that
	 * are available in the context menu of the view.
	 * \param action the action to be removed from the view's context menu
	 */
	void removeContextMenuAction(QAction *action);

public Q_SLOTS:
	/**
	 * Toggle the visibility of the forms.
	 * \param visible whether the forms will become visible
	 */
	void slotToggleShowForms(bool visible);
	/**
	 * This function zooms the contents in the view in with one step.
	 */
	void slotZoomIn();
	/**
	 * This function zooms the contents in the view out with one step.
	 */
	void slotZoomOut();
	/**
	 * Move to the start of the document.
	 */
	void slotGoToStartOfDocument();
	/**
	 * Move to the end of the document.
	 */
	void slotGoToEndOfDocument();
	/**
	 * Move to the previous page in the document.
	 */
	void slotGoToPreviousPage();
	/**
	 * Move to the next page in the document.
	 */
	void slotGoToNextPage();
	/**
	 * Focus the page selection action (if it is added to a toolbar or
	 * any other visible place in the main application).
	 */
	void slotGoToPage();
	/**
	 * Show a print dialog and do the actual printing if the user accepts
	 * the print dialog.
	 */
	void slotPrint();

Q_SIGNALS:
	/**
	 * This signal is emitted when the view's scroll bar's value has changed.
	 * \param fraction the fraction (between 0 and 1) of the page height where the top visible position is located
	 * \param pageNumber the current page's number
	 */
	void scrollPositionChanged(qreal fraction, int pageNumber);
	/**
	 * This signal is emitted whenever the search terminates and the Find
	 * widget in the main window should be closed.
	 */
	void closeFindWidget();
	/**
	 * This signal is emitted when the zoom factor in the view has changed.
	 * \param value the current zoom factor (after the change)
	 */
	void zoomFactorChanged(qreal value);
	/**
	 * This signal is emitted when the mouse tool has changed.
	 * \param tool the ID of the current mouse tool
	 */
	void mouseToolChanged(PdfView::MouseTool tool);

protected:
#ifndef QT_NO_CONTEXTMENU
	virtual void contextMenuEvent(QContextMenuEvent *event);
#endif // QT_NO_CONTEXTMENU
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
#ifndef QT_NO_WHEELEVENT
	virtual void wheelEvent(QWheelEvent *event);
#endif // QT_NO_WHEELEVENT

private:
//	QSharedDataPointer<PdfViewPrivate> d;
	PdfViewPrivate *d;
	friend class PdfViewPrivate;
};

Q_DECLARE_METATYPE(PdfView::MouseTool)

#endif // PDFVIEWER_PDFVIEW_H
