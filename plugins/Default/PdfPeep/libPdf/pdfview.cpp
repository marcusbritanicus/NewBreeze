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

#include <QDebug>
#include "newbreeze.hpp"
#include "pdfview.h"
#include "pdfview_p.h"

#include "pageitem.h"
#include "actionhandler.h"
#include "printhandler.h"
#include "utils/bookmarkshandler.h"
#include "utils/filesettings.h"
//#include "utils/icon.h"
#include "utils/selectpageaction.h"
#include "utils/zoomaction.h"

#if QT_VERSION >= 0x050000
	#include <poppler-qt5.h>
#else
	#include <poppler-qt4.h>
#endif
#include <poppler-form.h>

#include <QFileInfo>
#include <QPointer>
#include <QProcess>
#include <QTimer>
#include <QUrl>
#include <qmath.h>
#include <QClipboard>
#include <QDesktopServices>
#include <QImage>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QStyle>
#include <QTextEdit>
#include <QInputDialog>
#ifndef QT_NO_TOOLTIP
#include <QToolTip>
#endif // QT_NO_TOOLTIP

// add copyright notice to the *.ts files; this string is not used anywhere else
static struct { const char *source; const char *comment; } copyrightString
	= QT_TRANSLATE_NOOP3("__Copyright__",
	"The original English text is copyrighted by the authors of the source "
	"files where the strings come from. This file is distributed under the "
	"same license as the PdfViewLib package. The translations in this file "
	"are copyrighted as follows.",
	"Translators: do not translate this, but put in the \"translation\" "
	"a copyright notice of the form \"This file was translated by <NAME>. "
	"Copyright (C) <YEAR> <NAME>.\" in which you fill in the year(s) of "
	"translation and your name.");

static const int s_interPageSpace = 10;
static const qreal s_minZoomFactor = 0.1;
static const qreal s_maxZoomFactor = 6;

PdfPasswdInput::PdfPasswdInput( QWidget *parent ) : QInputDialog( parent ) {

	setInputMode( QInputDialog::TextInput );
	setTextEchoMode( QLineEdit::Password );
	setFixedSize( 200, 75 );

	QDesktopWidget dw;
	int hpos = (int)( ( dw.width() - 200 ) / 2 );
	int vpos = (int)( ( dw.height() - 75 ) / 2 );

	setGeometry( hpos, vpos, 200, 75 );
	setWindowFlags( Qt::Popup );
};

QString PdfPasswdInput::getText( QWidget *parent, QString title, QString label ) {

	PdfPasswdInput *dlg = new PdfPasswdInput( parent );
	dlg->setWindowTitle( title );
	dlg->setLabelText( label );

	dlg->exec();

	return dlg->textValue();
}

PdfViewPrivate::PdfViewPrivate(PdfView *pdfView)
	: q(pdfView)
	, m_actionHandler(0)
	, m_zoomInAction(0)
	, m_zoomOutAction(0)
	, m_zoomAction(0)
	, m_maxFileSettingsCacheSize(0)
	, m_bookmarksHandler(0)
	, m_popplerDocument(0)
    , m_zoomFactor(1.0)
    , m_maximumPageWidth(0)
    , m_dpiX(QApplication::desktop()->physicalDpiX())
    , m_dpiY(QApplication::desktop()->physicalDpiY())
	, m_pageNumber(-1)
	, m_realPageNumber(-1)
	, m_findHighlightRect(0)
	, m_mouseTool(PdfView::Magnifying)
	, m_isDragging(false)
	, m_magnifiedPageRect(0)
	, m_magnifiedPageItem(0)
	, m_selectionRect(0)
	, m_showForms(false)
	, m_verticalPositionTimer(0) {
	Q_UNUSED(copyrightString);
}

PdfViewPrivate::~PdfViewPrivate() {
}

void PdfViewPrivate::init() {
//	Q_Q(PdfView);
	m_pageScene = new QGraphicsScene(q);
	q->setScene(m_pageScene);
	q->setMouseTracking(true);
	const QColor backgroundColor = QApplication::style()->standardPalette().color(QPalette::Normal, QPalette::Window);
	m_pageScene->setBackgroundBrush(backgroundColor); // use window color for the background behind the pages

	m_printHandler = new PrintHandler(q);
}

/*******************************************************************/
// BookmarksWidget functions

QWidget *PdfViewPrivate::widget() {
	return q;
}

double PdfViewPrivate::position() const {
	return q->pageNumberWithPosition();
}

/*******************************************************************/

PdfView::PdfView(QWidget *parent)
	: QGraphicsView(parent)
	, d(new PdfViewPrivate(this)) {
//	Q_D(PdfView);
	d->init();
	setRenderHint(Poppler::Document::TextAntialiasing, true);
	setRenderHint(Poppler::Document::Antialiasing, true);
	setRenderBackend(Poppler::Document::RenderBackend(0));
	connect(d, SIGNAL(scrollPositionChanged(qreal,int)), this, SIGNAL(scrollPositionChanged(qreal,int)));
	connect(d, SIGNAL(mouseToolChanged(PdfView::MouseTool)), this, SIGNAL(mouseToolChanged(PdfView::MouseTool)));
}

PdfView::~PdfView() {
	delete d;
}

void PdfView::setMouseTool(MouseTool mouseTool) {
//	Q_D(PdfView);
	d->m_mouseTool = mouseTool;
}

void PdfViewPrivate::slotSelectMouseTool() {
	QAction *action = qobject_cast<QAction*>(sender());
	if (action)
	{
		PdfView::MouseTool which = action->data().value<PdfView::MouseTool>();
		q->setMouseTool(which);
		Q_EMIT mouseToolChanged(which);
	}
}

void PdfView::setMaximumFileSettingsCacheSize(int size) {
	d->m_maxFileSettingsCacheSize = size;
}

/*******************************************************************/
// Actions

QAction *PdfView::action(PdfViewAction action) {
	if (!d->m_actionHandler)
		d->m_actionHandler = new ActionHandler(this);

	switch (action)
	{
		case ZoomIn:
			d->m_zoomInAction = d->m_actionHandler->action(action, this, SLOT(slotZoomIn()));
			return d->m_zoomInAction;
			break;

		case ZoomOut:
			d->m_zoomOutAction = d->m_actionHandler->action(action, this, SLOT(slotZoomOut()));
			return d->m_zoomOutAction;
			break;

		case Zoom:
			d->m_zoomAction = qobject_cast<ZoomAction*>(d->m_actionHandler->action(action, d, SLOT(slotSetZoomFactor(qreal))));
			d->m_zoomAction->setMinZoomFactor(s_minZoomFactor);
			d->m_zoomAction->setMaxZoomFactor(s_maxZoomFactor);
			return d->m_zoomAction;
			break;

		case GoToStartOfDocument:
		{
			QAction *goToStartAction = d->m_actionHandler->action(action, this, SLOT(slotGoToStartOfDocument()));
			if (!d->m_popplerDocument)
				goToStartAction->setEnabled(false);
			return goToStartAction;
			break;
		}

		case GoToEndOfDocument:
		{
			QAction *goToEndAction = d->m_actionHandler->action(action, this, SLOT(slotGoToEndOfDocument()));
			if (!d->m_popplerDocument)
				goToEndAction->setEnabled(false);
			return goToEndAction;
			break;
		}

		case GoToPreviousPage:
		{
			QAction *goToPreviousPageAction = d->m_actionHandler->action(action, this, SLOT(slotGoToPreviousPage()));
			if (!d->m_popplerDocument)
				goToPreviousPageAction->setEnabled(false);
			return goToPreviousPageAction;
			break;
		}

		case GoToNextPage:
		{
			QAction *goToNextPageAction = d->m_actionHandler->action(action, this, SLOT(slotGoToNextPage()));
			if (!d->m_popplerDocument)
				goToNextPageAction->setEnabled(false);
			return goToNextPageAction;
			break;
		}

		case GoToPage:
		{
			QAction *goToPageAction = qobject_cast<SelectPageAction*>(d->m_actionHandler->action(action, d, SLOT(slotSetPage(int))));
			if (!d->m_popplerDocument)
				goToPageAction->setEnabled(false);
			return goToPageAction;
			break;
		}

		case Bookmarks:
		case SetBookmark:
		case PreviousBookmark:
		case NextBookmark:
			if (!d->m_bookmarksHandler)
			{
				d->m_bookmarksHandler = new BookmarksHandler(d);
				connect(d->m_bookmarksHandler, SIGNAL(goToPosition(double)), d, SLOT(slotSetPage(double)));
				if (!d->m_popplerDocument)
					d->m_bookmarksHandler->action(0)->setEnabled(false);
			}
			if (action == Bookmarks)
				return d->m_bookmarksHandler->menuAction();
			else if (action == SetBookmark)
				return d->m_bookmarksHandler->action(0);
			else if (action == PreviousBookmark)
				return d->m_bookmarksHandler->action(1);
			else if (action == NextBookmark)
				return d->m_bookmarksHandler->action(2);
			break;

		case Print:
		{
			QAction *printAction = d->m_actionHandler->action(action, this, SLOT(slotPrint()));
			if (!d->m_popplerDocument)
				printAction->setEnabled(false);
			return printAction;
			break;
		}

		case MouseToolBrowse:
			return d->m_actionHandler->action(action, d, SLOT(slotSelectMouseTool()));
			break;

		case MouseToolMagnify:
			return d->m_actionHandler->action(action, d, SLOT(slotSelectMouseTool()));
			break;

		case MouseToolSelection:
			return d->m_actionHandler->action(action, d, SLOT(slotSelectMouseTool()));
			break;

		case MouseToolTextSelection:
			return d->m_actionHandler->action(action, d, SLOT(slotSelectMouseTool()));
			break;

		case ShowForms:
		{
			QAction *showFormsAction = d->m_actionHandler->action(action, this, SLOT(slotToggleShowForms(bool)));
			if (!d->m_popplerDocument)
				showFormsAction->setEnabled(false);
			return showFormsAction;
			break;
		}

		case NoPdfViewAction:
		default:
			return 0;
	}
	return 0;
}

/*******************************************************************/

double PdfViewPrivate::scaleFactorX() const {
	return m_zoomFactor * m_dpiX / 72.0;
}

double PdfViewPrivate::scaleFactorY() const {
	return m_zoomFactor * m_dpiY / 72.0;
}

QPointF PdfView::mapFromPage(int pageNumber, const QPointF &pagePos) const {
//	Q_D(const PdfView);
	if (pageNumber < 0 || pageNumber >= d->m_popplerDocument->numPages())
		return QPointF();
	const double scenePosX = (pagePos.x() + s_interPageSpace / 2) * d->scaleFactorX();
	const double scenePosY = (pagePos.y() + d->m_popplerPageTopPositions.at(pageNumber)) * d->scaleFactorY();
	return QPointF(scenePosX, scenePosY);
}

QRectF PdfView::mapFromPage(int pageNumber, const QRectF &pageRect) const {
//	Q_D(const PdfView);
	if (pageNumber < 0 || pageNumber >= d->m_popplerDocument->numPages())
		return QRectF();
	const double sceneRectX = (pageRect.x() + s_interPageSpace / 2) * d->scaleFactorX();
	const double sceneRectY = (pageRect.y() + d->m_popplerPageTopPositions.at(pageNumber)) * d->scaleFactorY();
	const double sceneRectWidth = pageRect.width() * d->scaleFactorX();
	const double sceneRectHeight = pageRect.height() * d->scaleFactorY();
	return QRectF(sceneRectX, sceneRectY, sceneRectWidth, sceneRectHeight);
}

QPointF PdfView::mapToPage(int pageNumber, const QPointF &scenePos) const {
//	Q_D(const PdfView);
	if (pageNumber < 0 || pageNumber >= d->m_popplerDocument->numPages())
		return QPointF();
	const double pagePosX = scenePos.x() / d->scaleFactorX() - s_interPageSpace / 2;
	const double pagePosY = scenePos.y() / d->scaleFactorY() - d->m_popplerPageTopPositions.at(pageNumber);
	return QPointF(pagePosX, pagePosY);
}

QRectF PdfView::mapToPage(int pageNumber, const QRectF &sceneRect) const {
//	Q_D(const PdfView);
	if (pageNumber < 0 || pageNumber >= d->m_popplerDocument->numPages())
		return QRectF();
	const double pageRectX = sceneRect.x() / d->scaleFactorX() - s_interPageSpace / 2;
	const double pageRectY = sceneRect.y() / d->scaleFactorY() - d->m_popplerPageTopPositions.at(pageNumber);
	const double pageRectWidth = sceneRect.width() / d->scaleFactorX();
	const double pageRectHeight = sceneRect.height() / d->scaleFactorY();
	return QRectF(pageRectX, pageRectY, pageRectWidth, pageRectHeight);
}

QPointF PdfView::mapToCurrentPage(const QPointF &scenePos) const {
	const int pageNumber = d->pageNumberAtPosition(scenePos);
	if (pageNumber < 0 || pageNumber >= d->m_popplerDocument->numPages())
		return QPointF();
	return mapToPage(pageNumber, scenePos);
}

/*******************************************************************/
// Render hints and backend

void PdfView::setRenderBackend(Poppler::Document::RenderBackend backend) {
	d->m_renderBackend = backend;

	if (d->m_popplerDocument)
	{
		d->clearPages();
		d->m_popplerDocument->setRenderBackend(backend);
		setPage(pageNumberWithPosition());
	}
}

Poppler::Document::RenderBackend PdfView::renderBackend() const {
	return d->m_renderBackend;
}

void PdfView::setRenderHint(Poppler::Document::RenderHint hint, bool on) {
	if (on)
		d->m_renderHints |= hint;
	else
		d->m_renderHints &= ~(int)hint;

	if (d->m_popplerDocument)
	{
//		d->clearPages();
//		d->m_popplerDocument->setRenderHint(hint, on);
//		setPage(pageNumberWithPosition());
		// reloading single pages doesn't work, so we must completely reload the file
		d->m_popplerDocument->setRenderHint(hint, on);
		const double pageNumber = pageNumberWithPosition();
		const QString file = fileName();
		close();
		load(file);
		setPage(pageNumber);
	}
}

Poppler::Document::RenderHints PdfView::renderHints() const {
	return Poppler::Document::RenderHints(d->m_renderHints);
}

/*******************************************************************/
// Toggle show forms

void PdfViewPrivate::showForms(PageItem *pageItem, int pageNumber) {
	if (!m_showForms)
		return;

	pageItem->generateFormFields();
	QList<FormField> formFields = pageItem->formFields();
	QList<QWidget*> formWidgets = pageItem->formWidgets();
	for (int i = 0; i < formFields.size(); ++i)
	{
		const QRectF formFieldRect = q->mapFromPage(pageNumber, formFields.at(i).rect);
		QGraphicsProxyWidget *formFieldWidget = m_pageScene->addWidget(formWidgets.at(i));
		// make sure that multiline text fields are not too large
		formFieldWidget->setMinimumSize(QSizeF(10, 10));
		QTextEdit *textEdit = qobject_cast<QTextEdit*>(formWidgets.at(i));
		if (textEdit && formFieldRect.width() <= textEdit->verticalScrollBar()->width() + 10) // ensure small text fields are editable
		{
			textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		}
		// resize widget to its allocated size
		formFieldWidget->setGeometry(formFieldRect);
//		formFieldWidget->setData(0, i);
		formFieldWidget->setZValue(1);
	}
}

void PdfView::slotToggleShowForms(bool visible) {
	d->m_showForms = visible;
	if (d->m_popplerDocument)
	{
		const double pageNumber = pageNumberWithPosition();
		const QString file = fileName();
		close();
		load(file);
		setPage(pageNumber);
	}
}

/*******************************************************************/
// Load and close document

bool PdfView::load(const QString &fileName) {
	Poppler::Document *popplerDocument = Poppler::Document::load(fileName);
	if (not popplerDocument)
		return false;

	else if ( popplerDocument->isEncrypted() ) {
		QString passwd = PdfPasswdInput::getText(
			this,																									// Parent
			"NewBreeze | Password",																					// Title
			"Enter the password for the encrypted file " + baseName( fileName ) + ":"								// Prompt
		);

		if ( passwd.isEmpty() )
			return false;

		popplerDocument = Poppler::Document::load( fileName, passwd.toLocal8Bit(), QByteArray() );
	}

	else if ( popplerDocument->isLocked() ) {
		QString passwd = PdfPasswdInput::getText(
			this,																								// Parent
			"NewBreeze | Password",																				// Title
			"Enter the password for the locked file " + baseName( fileName ) + ":"								// Prompt
		);

		if ( passwd.isEmpty() )
			return false;

		popplerDocument = Poppler::Document::load( fileName, QByteArray(), passwd.toLocal8Bit() );
	}

	d->m_fileName = fileName;

	// set the zoom factor before actually loading the document (otherwise changing the zoom factor causes the file to be reloaded)
	FileSettings fileSettings(d->m_fileName);
	setZoomFactor(fileSettings.value(QLatin1String("ZoomFactor"), d->m_zoomFactor).toDouble());

	// set show forms hints before loading the document and before setting d->m_popplerDocument
	d->m_showForms = fileSettings.value(QLatin1String("ShowForms"), false).toBool();
	if (d->m_actionHandler && d->m_actionHandler->action(ShowForms))
		d->m_actionHandler->action(ShowForms)->setChecked(d->m_showForms);

	// set d->m_popplerDocument here so that setZoomFactor() above doesn't cause a crash
	d->m_popplerDocument = popplerDocument;

	// set render hints before loading the document
	d->m_popplerDocument->setRenderHint(Poppler::Document::TextAntialiasing, (d->m_renderHints & Poppler::Document::TextAntialiasing));
	d->m_popplerDocument->setRenderHint(Poppler::Document::Antialiasing, (d->m_renderHints & Poppler::Document::Antialiasing));
	d->m_popplerDocument->setRenderBackend(d->m_renderBackend);

	d->loadDocument();

	// set this after loading the document (otherwise there is a crash)
	horizontalScrollBar()->setValue(fileSettings.value(QLatin1String("HorizontalScrollBarValue"), 0).toInt());
	verticalScrollBar()->setValue(fileSettings.value(QLatin1String("VerticalScrollBarValue"), 0).toInt());
	setPage(fileSettings.value(QLatin1String("Page"), 0).toDouble());

	// enable actions
	if (d->m_actionHandler)
	{
		d->m_actionHandler->setPageLabels(d->m_popplerPageLabels);
		d->m_actionHandler->toggleFileDependentActionsEnabled(true);
	}

	// load bookmarks
	if (d->m_bookmarksHandler)
	{
		d->m_bookmarksHandler->setPageLabels(d->m_popplerPageLabels); // must be done before loadBookmarks()
		d->m_bookmarksHandler->loadBookmarks(d->m_fileName);
		action(PdfView::SetBookmark)->setEnabled(true);
	}

	return true;
}

QString PdfView::fileName() const {
	return d->m_fileName;
}

Poppler::Document *PdfView::document() const {
	return d->m_popplerDocument;
}

//void PdfView::loadFileSpecificSettings()
//{
//	// load file specific settings
//	FileSettings fileSettings(d->m_fileName);
//	horizontalScrollBar()->setValue(fileSettings.value("HorizontalScrollBarValue", 0).toInt());
//	verticalScrollBar()->setValue(fileSettings.value("VerticalScrollBarValue", 0).toInt());
//}

void PdfViewPrivate::loadDocument() {
//QTime t = QTime::currentTime();
	// load all pages in the PDF file
	const int pageCount = m_popplerDocument->numPages();
	m_pageLoaded.reserve(pageCount);
	qreal maximumPageWidth = 0;
	for (int i = 0; i < pageCount; ++i)
	{
		m_popplerPages << m_popplerDocument->page(i);
		m_pageItems << new PageItem(m_popplerPages.at(i), q);
		m_popplerPageLabels << m_pageItems.at(i)->label();
		if (i == 0)
			m_popplerPageTopPositions << s_interPageSpace / 2;
		else
		{
			const QSizeF oldPopplerPageSizeF = m_pageItems.at(i-1)->pageSizeF();
			m_popplerPageTopPositions << m_popplerPageTopPositions.at(i-1) + oldPopplerPageSizeF.height() + s_interPageSpace;
			maximumPageWidth = qMax(oldPopplerPageSizeF.width(), maximumPageWidth);
		}

		// draw and fill page rectangles
		const QRectF pageRect = q->mapFromPage(i, QRectF(QPointF(0, 0), m_pageItems.at(i)->pageSizeF()));
		QGraphicsRectItem *rect = m_pageScene->addRect(pageRect, QPen(QBrush(Qt::black), 1)); // black border
		m_pageScene->addRect(pageRect, QPen(), QBrush(Qt::white)); // white background
		rect->setZValue(1);
		rect->setData(0, i); // since rect has a higher z-value than the actual pageItem containing the pixmap, we set the data of rect to the page number here instead of setting the data of pageItem in loadPage()
		m_pageLoaded << false;
	}

	const QSizeF lastPopplerPageSizeF = m_pageItems.at(pageCount-1)->pageSizeF();
	maximumPageWidth = qMax(lastPopplerPageSizeF.width(), maximumPageWidth);
	// add the total height of the PDF (+ s_interPageSpace) to the list
	m_popplerPageTopPositions << m_popplerPageTopPositions.at(pageCount-1) + m_pageItems.at(pageCount-1)->pageSizeF().height() + s_interPageSpace / 2;
	m_pageScene->setSceneRect(0, 0, (maximumPageWidth + s_interPageSpace) * scaleFactorX() + 2, m_popplerPageTopPositions.at(pageCount) * scaleFactorY() + 2);

	m_maximumPageWidth = maximumPageWidth;

	// avoid calling setPage() when closing a document, so we connect the following signals here instead of in the constructor and disconnect them in documentClosed()
	connect(q->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slotVerticalPositionChanged(int)));
}

void PdfView::close() {
	if (!d->m_popplerDocument)
		return;
	d->closeDocument();
	delete d->m_popplerDocument;
	d->m_popplerDocument = 0;
	d->m_fileName.clear();

	// disable actions
	if (d->m_actionHandler)
	{
		d->m_actionHandler->toggleGoToActionsEnabled(false);
		d->m_actionHandler->toggleFileDependentActionsEnabled(false);
	}

	// save bookmarks
	if (d->m_bookmarksHandler)
	{
		d->m_bookmarksHandler->saveBookmarks();
		d->m_bookmarksHandler->clear();
		action(PdfView::SetBookmark)->setEnabled(false);
	}
}

void PdfViewPrivate::closeDocument() {
	// avoid calling setPage() when closing a document, so we disconnect the following signals and reconnect them in documentLoaded()
	disconnect(q->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slotVerticalPositionChanged(int)));

	// store settings
	FileSettings fileSettings(m_fileName);
	fileSettings.setValue(QLatin1String("HorizontalScrollBarValue"), q->horizontalScrollBar()->value());
	fileSettings.setValue(QLatin1String("VerticalScrollBarValue"), q->verticalScrollBar()->value());
	fileSettings.setValue(QLatin1String("ZoomFactor"), q->zoomFactor());
	fileSettings.setValue(QLatin1String("Page"), q->pageNumberWithPosition());
	fileSettings.setValue(QLatin1String("ShowForms"), m_showForms);
	FileSettings::constrainCacheToMaxSize(m_maxFileSettingsCacheSize);

	// clean up
	m_pageNumber = -1;
	m_realPageNumber = -1;
	while (!m_popplerPages.isEmpty())
		delete m_popplerPages.takeLast();
	m_popplerPageLabels.clear();
	m_popplerPageTopPositions.clear();

	while (!m_pageItems.isEmpty())
		delete m_pageItems.takeLast();
	m_pageLoaded.clear();

	removeFindHighlight();
	removeTextSelection(); // this must also happen here, otherwise the sequence "sync from source + rebuild tex file + sync from source" causes a crash
	m_pageScene->clear();
}

QList<Poppler::Page*> PdfView::popplerPages() {
	return d->m_popplerPages;
}

QStringList PdfView::popplerPageLabels() {
	return d->m_popplerPageLabels;
}

/*******************************************************************/
// Set and get page number

void PdfViewPrivate::loadPage(int pageNumber) {
//QTime t = QTime::currentTime();
	if (pageNumber < 0 || pageNumber >= m_popplerDocument->numPages())
		return;

    const double resX = m_dpiX * m_zoomFactor;
    const double resY = m_dpiY * m_zoomFactor;
	const QImage image = m_pageItems.at(pageNumber)->renderToImage(resX, resY); // this is slow

	if (!image.isNull())
	{
		QGraphicsPixmapItem *pageItem = m_pageScene->addPixmap(QPixmap::fromImage(image));
		if (!m_pageLoaded.at(pageNumber))
		{
			m_pageItems.at(pageNumber)->generateLinks(m_popplerPageLabels); // this is very slow if there are many links
			showForms(m_pageItems.at(pageNumber), pageNumber);
		}
		pageItem->setOffset(q->mapFromPage(pageNumber, QPointF(0, 0)));
		pageItem->setData(1, pageNumber);
		m_pageLoaded[pageNumber] = true;
	}
//qCritical() << "loadPage" << pageNumber << t.msecsTo(QTime::currentTime());
}

void PdfViewPrivate::loadVisiblePages(int pageNumberStart, int pageNumberEnd) {
	const int pageCount = m_popplerDocument->numPages();
	for (int i = qMax(0, pageNumberStart); i <= pageNumberEnd && i < pageCount; ++i)
		if (!m_pageLoaded.at(i))
			loadPage(i);
}

void PdfViewPrivate::clearPages() {
	const int pageCount = m_popplerDocument->numPages();
	QList<QGraphicsItem*> items = m_pageScene->items();
	for (int i = 0; i < items.size(); ++i)
	{
		const int pageNumber = items.at(i)->data(1).toInt();
		if (pageNumber >= 0 && pageNumber < pageCount)
		{
			m_pageScene->removeItem(items.at(i));
			delete items.at(i);
			m_pageLoaded[pageNumber] = false;
		}
	}
}

void PdfViewPrivate::clearNonVisiblePages(int pageNumberStart, int pageNumberEnd) {
	// clear non-visible pages outside a certain range around the visible area to save memory
	const int pageCount = m_popplerDocument->numPages();
	const int buffer = 25;
	QList<QGraphicsItem*> items = m_pageScene->items();
	for (int i = 0; i < items.size(); ++i)
	{
		const int pageNumber = items.at(i)->data(1).toInt();
		if ((pageNumber >= 1 && pageNumber < pageNumberStart - buffer)
		    || (pageNumber > pageNumberEnd + buffer && pageNumber < pageCount))
		{
			m_pageScene->removeItem(items.at(i));
			delete items.at(i);
			m_pageLoaded[pageNumber] = false;
		}
	}
}

void PdfView::setPage(double pageNumber, PositionHandling keepPosition) {
//QTime t = QTime::currentTime();
	if (!d->m_popplerDocument)
		return;

	// reset some values
	if (d->m_realPageNumber != pageNumber)
	{
		d->m_findPositionTop = d->m_findPositionBottom = -1;
		d->m_findPositionLeft = d->m_findPositionRight = -1;
	}
	d->m_realPageNumber = pageNumber;

	// calculate which are the visible pages and load them
	int pageNumberStart = pageNumber;
	int pageNumberEnd = pageNumberStart;
	const double maxTopPosition = d->m_popplerPageTopPositions.at(pageNumberStart) + viewport()->height() / d->scaleFactorY(); // minimize calculations by scaling this instead of d->m_popplerPageTopPositions.at(pageNumberEnd) in the following for-loop
	const int pageCount = d->m_popplerDocument->numPages();
	for (; pageNumberEnd <= pageCount; ++pageNumberEnd)
		if (d->m_popplerPageTopPositions.at(pageNumberEnd) > maxTopPosition)
			break;
	if (pageNumberEnd > pageCount || d->m_popplerPageTopPositions.at(pageNumberEnd) <= maxTopPosition) // this can only happen when we reached the last page
	{
		pageNumberEnd = qMin(pageNumberEnd, pageCount);
		const double minTopPosition = d->m_popplerPageTopPositions.at(pageNumberEnd) - viewport()->height() / d->scaleFactorY();
		for (; pageNumberStart >= 0; --pageNumberStart)
			if (d->m_popplerPageTopPositions.at(pageNumberStart) < minTopPosition)
				break;
		--pageNumberEnd;
	}
	if (pageNumberStart < 0)
		pageNumberStart = 0;
	d->loadVisiblePages(pageNumberStart, pageNumberEnd); // this is slow
	d->clearNonVisiblePages(pageNumberStart, pageNumberEnd); // this is fast

	const int pageNumberOld = d->m_pageNumber;
	d->m_pageNumber = pageNumberStart;

	// update scrollbar
	if (keepPosition == DontUpdatePosition)
		return;
	QScrollBar *vbar = verticalScrollBar();
	disconnect(vbar, SIGNAL(valueChanged(int)), d, SLOT(slotVerticalPositionChanged(int)));
	const int newValue = keepPosition == KeepPosition // we must use qRound() here, otherwise vbar->value() is incorrectly set which leads to sometimes not being able to go to the next bookmark (because here the current position is being set as less than the bookmark position)
		? qRound(d->m_popplerPageTopPositions.at(d->m_pageNumber) * d->scaleFactorY() + vbar->value() - d->m_popplerPageTopPositions.at(pageNumberOld) * d->scaleFactorY())
		: qRound((d->m_popplerPageTopPositions.at(d->m_pageNumber) - s_interPageSpace / 2) * d->scaleFactorY() // we subtract s_interPageSpace / 2 here in order to show some space above the page when pageNumber is an integer
		         + (d->m_popplerPageTopPositions.at(d->m_pageNumber+1) - d->m_popplerPageTopPositions.at(d->m_pageNumber) - s_interPageSpace) * d->scaleFactorY() * (pageNumber - int(pageNumber))); // the converse of this is used in pageNumberWithPosition()
	vbar->setValue(newValue);
	d->scrollPositionChanged();
	connect(vbar, SIGNAL(valueChanged(int)), d, SLOT(slotVerticalPositionChanged(int)));
//qCritical() << "setPage" << t.msecsTo(QTime::currentTime());
}

void PdfViewPrivate::slotSetPage(int pageNumber) {
	q->setPage(pageNumber);
}

void PdfViewPrivate::slotSetPage(double pageNumber) {
	q->setPage(pageNumber);
}

double PdfView::pageNumberWithPosition() const {
	// we return the inverse of the calculation done in setPage() for const int newValue in the case that keepPosition == DocumentObserver::DontKeepPosition
	if (d->m_pageNumber < 0)
		return d->m_pageNumber;
	return d->m_pageNumber
	    + (verticalScrollBar()->value() - (d->m_popplerPageTopPositions.at(d->m_pageNumber) - s_interPageSpace / 2) * d->scaleFactorY())
	      / ((d->m_popplerPageTopPositions.at(d->m_pageNumber+1) - d->m_popplerPageTopPositions.at(d->m_pageNumber) - s_interPageSpace) * d->scaleFactorY());
}

void PdfView::slotGoToPage() {
	qobject_cast<SelectPageAction*>(action(GoToPage))->setFocus();
}

/*******************************************************************/
// Search

void PdfViewPrivate::removeFindHighlight() {
	if (m_findHighlightRect)
	{
		m_pageScene->removeItem(m_findHighlightRect);
		delete m_findHighlightRect;
		m_findHighlightRect = 0;
	}
}

void PdfView::search(const QString &text, const FindFlags &flags) {
	if (d->m_realPageNumber < 0) // this is the case at startup when no document is loaded
		return;

	if (d->m_findHighlightRect) // only show current highlight, so remove old highlight
		d->removeFindHighlight();

	const Poppler::Page::SearchDirection searchDirection = (flags & FindBackward) ? Poppler::Page::PreviousResult : Poppler::Page::NextResult;
	const Poppler::Page::SearchMode searchMode = (flags & FindCaseSensitively) ? Poppler::Page::CaseSensitive : Poppler::Page::CaseInsensitive;
	if (d->m_findPositionTop < 0 && searchDirection == Poppler::Page::PreviousResult)
	{
		// set start positions to their maximal values
		QSizeF popplerPageSize = d->m_pageItems.at(d->m_realPageNumber)->pageSizeF();
		d->m_findPositionTop = d->m_findPositionBottom = popplerPageSize.height();
		d->m_findPositionLeft = d->m_findPositionRight = popplerPageSize.width();
	}
	if (d->m_pageItems.at(d->m_realPageNumber)->search(text, d->m_findPositionLeft, d->m_findPositionTop, d->m_findPositionRight, d->m_findPositionBottom, searchDirection, searchMode))
	{
		// scroll to the found word (the page number is updated automatically)
		const int newValue = qMax(qreal(0.0), mapFromPage(d->m_realPageNumber, QPointF(0, d->m_findPositionTop)).y() - 3);
		verticalScrollBar()->setValue(newValue);

		// draw highlight
		d->m_findHighlightRect = d->m_pageScene->addRect(mapFromPage(d->m_realPageNumber, QRectF(d->m_findPositionLeft, d->m_findPositionTop, d->m_findPositionRight - d->m_findPositionLeft, d->m_findPositionBottom - d->m_findPositionTop)).adjusted(-1, -1, 1, 1), QPen(QBrush(), 0), QBrush(QColor(100, 160, 255, 100))); // blue background
		d->m_findHighlightRect->setZValue(2); // make sure that the rectangle is visible

		// scroll horizontally so the found word is visible
		ensureVisible(d->m_findHighlightRect->boundingRect(), 3, 3);
	}
	else // text is not found anymore on the current page
	{
		// reset start positions for find
		d->m_findPositionTop = d->m_findPositionBottom = -1;
		d->m_findPositionLeft = d->m_findPositionRight = -1;
		if (searchDirection == Poppler::Page::NextResult && d->m_realPageNumber < document()->numPages() - 1)
		{
			++d->m_realPageNumber; // we don't physically change the page until the search string is actually found
			search(text, flags);
		}
		else if (searchDirection == Poppler::Page::PreviousResult && d->m_realPageNumber > 0)
		{
			--d->m_realPageNumber; // we don't physically change the page until the search string is actually found
			search(text, flags);
		}
		else
		{
			const QString msg = !(flags & FindBackward) ?
			    tr("End of document reached.\n\nContinue from the beginning?")
			    : tr("Beginning of document reached.\n\nContinue from the end?");
			QPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Question, tr("Find", "MessageBox title"), msg, QMessageBox::NoButton, this);
			QPushButton *yesButton = msgBox->addButton(tr("Continue", "Button label"), QMessageBox::YesRole);
			msgBox->addButton(QMessageBox::No);
			msgBox->setDefaultButton(yesButton);
			msgBox->exec();
			if (msgBox->clickedButton() == yesButton)
			{
				d->m_realPageNumber = (flags & FindBackward) ? d->m_popplerDocument->numPages() - 1 : 0;
				search(text, flags);
			}
			else
				Q_EMIT closeFindWidget();
			delete msgBox;
		}
	}
}

/*******************************************************************/
// PageText

QString PdfView::pageText( int pageno ) {

	if (!d->m_popplerDocument)
		return QString();

	QRectF pageRect = QRectF( QPoint( 0, 0 ), d->m_pageItems.at( pageno )->pageSizeF() );
	if ( pageRect.isValid() )
		return d->m_pageItems.at( pageno )->text( pageRect );

	return QString();
};

/*******************************************************************/
// Zoom

void PdfView::setZoomFactor(qreal value) {
	// keep m_zoomFactor between boundaries
	if (d->m_zoomFactor == s_maxZoomFactor && value > s_maxZoomFactor)
        return;
	d->m_zoomFactor = qBound(s_minZoomFactor, value, s_maxZoomFactor);

	// enable/disable and update zoom actions
	if (d->m_actionHandler)
		d->m_actionHandler->updateZoomActions(d->m_zoomFactor, s_minZoomFactor, s_maxZoomFactor);

	Q_EMIT zoomFactorChanged(d->m_zoomFactor);

	// if no document is loaded, then we are done
	if (!d->m_popplerDocument)
		return;

	// get old values and maxima
	QScrollBar *hbar = horizontalScrollBar();
	QScrollBar *vbar = verticalScrollBar();
	const int oldHValue = hbar->value();
	const int oldVValue = vbar->value();
	const int oldHMaximum = hbar->maximum();
	const int oldVMaximum = vbar->maximum();

	// clean up pages, so that reloadPage() will load them again at the correct size
	d->m_pageNumber = -1; // must set this to -1 so that the current page is actually reloaded
	d->removeFindHighlight(); // we must remove the find highlight, otherwise a crash may occur when zooming fast and pressing F3 immediately after that
	d->m_pageScene->clear();
	const int pageCount = d->m_popplerDocument->numPages();
	qreal maximumPageWidth = 0;
	for (int i = 0; i < pageCount; ++i)
	{
		const QSizeF popplerPageSizeF = d->m_pageItems.at(i)->pageSizeF();
		maximumPageWidth = qMax(popplerPageSizeF.width(), maximumPageWidth);
		const QRectF pageRect = mapFromPage(i, QRectF(QPointF(0, 0), popplerPageSizeF));
		QGraphicsRectItem *rect = d->m_pageScene->addRect(pageRect, QPen(QBrush(Qt::black), 1)); // black border
		d->m_pageScene->addRect(pageRect, QPen(), QBrush(Qt::white)); // white background
		rect->setZValue(1);
		d->m_pageLoaded[i] = false;
	}
	d->m_pageScene->setSceneRect(0, 0, (maximumPageWidth + s_interPageSpace) * d->scaleFactorX() + 2, d->m_popplerPageTopPositions.at(pageCount) * d->scaleFactorY() + 2);

	// set new values (which are rescaled versions of the old values); this will also load the correct pages
	if (oldHMaximum > 0)
		hbar->setValue(int(qreal(hbar->maximum()) / oldHMaximum * oldHValue)); // idem as below
	vbar->setValue(int(qreal(vbar->maximum()) / oldVMaximum * oldVValue)); // cannot use oldVValue * vbar->maximum() / oldVMaximum because the numerator overflows int

	// in the following cases, the value of the vertical scrollbar doesn't change, so we must reload the pages manually; this must be done after the setValue() commands on the previous lines to avoid flicker
	if (oldVValue == 0 || oldVValue == oldVMaximum)
	{
		d->m_pageNumber = oldVValue == 0 ? 0 : pageCount - 1; // if d->m_pageNumber = -1, then all pages are reloaded instead of only the visible pages
		setPage(d->m_pageNumber);
		vbar->setValue(int(qreal(vbar->maximum()) / oldVMaximum * oldVValue)); // this must be done when oldVValue == oldVMaximum in order to stay at the bottom
	}
}

void PdfViewPrivate::slotSetZoomFactor(qreal value) {
	q->setZoomFactor(value);
}

qreal PdfView::zoomFactor() const {
	return d->m_zoomFactor;
}

void PdfView::slotZoomIn() {
	setZoomFactor(d->m_zoomFactor + (d->m_zoomFactor > 0.99 ?
	    (d->m_zoomFactor > 1.99 ? 0.5 : 0.2) : 0.1));
}

void PdfView::slotZoomOut() {
	setZoomFactor(d->m_zoomFactor - (d->m_zoomFactor > 1.01 ?
	    (d->m_zoomFactor > 2.01 ? 0.5 : 0.2) : 0.1));
}

void PdfView::setMaximumPageWidth( int value ) {

	if (!d->m_popplerDocument)
		return;

	value = value * 72.0 / d->m_dpiX;
	qreal z = qreal( value ) / d->m_maximumPageWidth;

	if ( z < 0.1 )
		z = 0.1;

	else if ( z > 6 )
		z = 6;

	setZoomFactor( z );
};

/*******************************************************************/
// Change vertical scrollbar position

void PdfViewPrivate::slotVerticalPositionChanged(int value) {
	// we use a timer to make sure that when the user drags the vertical scrollbar fast, then the switch to the new page happens instantaneously (i.e. unnecessary intermediate page switches are skipped)
	Q_UNUSED(value);
	if (m_verticalPositionTimer)
		m_verticalPositionTimer->stop();
	else
	{
		m_verticalPositionTimer = new QTimer(this);
		m_verticalPositionTimer->setSingleShot(true);
		m_verticalPositionTimer->setInterval(0);
		connect(m_verticalPositionTimer, SIGNAL(timeout()), this, SLOT(slotVerticalPositionChanged()));
	}
	m_verticalPositionTimer->start();
}

void PdfViewPrivate::slotVerticalPositionChanged() {
	// find current page number
	QScrollBar *vbar = q->verticalScrollBar();
    const double reverseScaledVBarValue = (vbar->value() + s_interPageSpace / 2) / scaleFactorY(); // minimize calculations by scaling this instead of m_popplerPageTopPositions.at(pageNumber) in the following for-loop
	int pageNumber = 0;
	const int pageCount = m_popplerDocument->numPages();
	for( ; pageNumber < pageCount && m_popplerPageTopPositions.at(pageNumber) <= reverseScaledVBarValue; ++pageNumber);
	--pageNumber;
	if (pageNumber < 0)
		pageNumber = 0;

	// scroll page to the correct position
	if (pageNumber != m_pageNumber)
	{
		q->setPage(pageNumber, PdfView::DontUpdatePosition); // load pages that become visible
		m_pageNumber = pageNumber;
	}
	scrollPositionChanged();
}

void PdfViewPrivate::scrollPositionChanged() {
	QScrollBar *vbar = q->verticalScrollBar();
	// enable/disable page switching actions
	if (m_actionHandler)
		m_actionHandler->toggleGoToActionsEnabled(true, m_pageNumber, 0, m_popplerDocument->numPages() - 1, vbar->value(), 0, vbar->maximum());
	// enable/disable bookmark switching actions
	if (m_bookmarksHandler)
		m_bookmarksHandler->updateActions();
	Q_EMIT scrollPositionChanged(qreal(vbar->value()) / vbar->maximum(), m_pageNumber);
}

/*******************************************************************/
// Magnify tool

void PdfViewPrivate::magnify(const QPointF &scenePos) {
	// search number of the page we are in
	const int pageNumber = pageNumberAtPosition(scenePos);
	if (pageNumber < 0)
		return;

	// create magnification image
	const qreal magnifyZoom = 2;
	const double resX = m_dpiX * m_zoomFactor * magnifyZoom;
	const double resY = m_dpiY * m_zoomFactor * magnifyZoom;
	const int magnifyWidth = 200;
	const int magnifyHeight = 100;
	const QPointF pageTopLeft = q->mapFromPage(pageNumber, QPointF(0, 0));
	const QImage image = m_pageItems.at(pageNumber)->renderToImage(resX, resY,
	    magnifyZoom * (scenePos.x() - pageTopLeft.x() - magnifyWidth / 2),
	    magnifyZoom * (scenePos.y() - pageTopLeft.y() - magnifyHeight / 2),
	    magnifyZoom * magnifyWidth, magnifyZoom * magnifyHeight);

	// create magnification window
	if (!m_magnifiedPageItem && !image.isNull())
	{
		m_magnifiedPageRect = m_pageScene->addRect(0, 0, magnifyZoom * magnifyWidth, magnifyZoom * magnifyHeight, QPen(QBrush(Qt::black), 1)); // black border
		m_magnifiedPageItem = m_pageScene->addPixmap(QPixmap::fromImage(image));
	}
	else if (!image.isNull())
		m_magnifiedPageItem->setPixmap(QPixmap::fromImage(image));

	// move magnification window to the correct place
	m_magnifiedPageRect->setPos(scenePos.x() - magnifyZoom * magnifyWidth / 2, scenePos.y() - magnifyZoom * magnifyHeight / 2);
	m_magnifiedPageRect->setZValue(3);
	m_magnifiedPageItem->setOffset(scenePos.x() - magnifyZoom * magnifyWidth / 2, scenePos.y() - magnifyZoom * magnifyHeight / 2);
	m_magnifiedPageItem->setZValue(2);
}

void PdfViewPrivate::endMagnify() {
	delete m_magnifiedPageRect;
	m_magnifiedPageRect = 0;
	delete m_magnifiedPageItem;
	m_magnifiedPageItem = 0;
}

/*******************************************************************/
// Links

void PdfViewPrivate::findLinkAtPosition(const QPointF &scenePos) {
	// search number of the page we are in
	const int pageNumber = pageNumberAtPosition(scenePos);

	// at startup and below the bottom of the last page the following conditions are true
	if (pageNumber < 0 || pageNumber >= m_popplerPageTopPositions.size() - 1)
		return;

	// translate scenePos to page and rescale scenePos to [0,1]
	QPointF pagePos = q->mapToPage(pageNumber, scenePos);
	m_pageItems.at(pageNumber)->findLinkAtPosition(pagePos);
}

/*******************************************************************/
// Select rectangular area

int PdfViewPrivate::pageNumberAtPosition(const QPointF &scenePos) {
	const double mouseTopScaled = scenePos.y() / scaleFactorY();
	int pageNumber;
	for (pageNumber = 0; pageNumber < m_popplerPageTopPositions.size(); ++pageNumber)
		if (m_popplerPageTopPositions.at(pageNumber) > mouseTopScaled)
			break;
	--pageNumber;
	return pageNumber;
}

void PdfViewPrivate::handleSelection(const QPoint &popupMenuPos) {
	QRectF selectionRect = m_selectionRect->rect();
	if (selectionRect.width() < 1 || selectionRect.height() < 1)
	{
		removeSelectionRect();
		return;
	}

	// translate selected rectangle to page coordinates
	const int pageNumber = pageNumberAtPosition(m_selectionRect->rect().topLeft());
	if (pageNumber < 0)
		return;
	selectionRect = q->mapToPage(pageNumber, selectionRect);

	// show popup menu with copy and save options
	QMenu menu(q);
//	QAction *copyImageAction = menu.addAction(Icon("image-x-generic"), tr("Copy &Image to Clipboard"));
//	QAction *saveImageAction = menu.addAction(Icon("document-save"), tr("&Save Image to File..."));
//	QAction *copyTextAction = menu.addAction(Icon("edit-copy"), tr("Copy &Text to Clipboard"));
	QAction *copyImageAction = menu.addAction(tr("Copy &Image to Clipboard"));
	QAction *saveImageAction = menu.addAction(tr("&Save Image to File..."));
	QAction *copyTextAction = menu.addAction(tr("Copy &Text to Clipboard"));
	menu.addSeparator();
//	menu.addAction(Icon("dialog-cancel"), tr("&Cancel"));
	menu.addAction(tr("&Cancel"));
	QAction *choice = menu.exec(popupMenuPos);

	if (choice)
	{
		if (choice == copyImageAction || choice == saveImageAction)
		{
			const double resX = m_dpiX * m_zoomFactor;
			const double resY = m_dpiY * m_zoomFactor;
			const QImage image = m_pageItems.at(pageNumber)->renderToImage(resX, resY, selectionRect.left() * scaleFactorX(), selectionRect.top() * scaleFactorY(), selectionRect.width() * scaleFactorX(), selectionRect.height() * scaleFactorY());
			if (choice == copyImageAction)
			{
				QClipboard *clipboard = QApplication::clipboard();
				clipboard->setImage(image, QClipboard::Clipboard);
				if (clipboard->supportsSelection())
					clipboard->setImage(image, QClipboard::Selection);
			}
#ifndef QT_NO_FILEDIALOG
			else // choice == saveImageAction
			{
				const QFileInfo fi(m_fileName);
				QString imageFileName = fi.absolutePath() + QLatin1Char('/') + fi.baseName() + QLatin1String(".png");
				imageFileName = QFileDialog::getSaveFileName(q,
				    tr("Save image as", "Save file dialog title"),
				    imageFileName,
				    QString(QLatin1String("%1 (*.png *.bmp *.gif *.jpg *.jpeg *.tiff *.xpm)")).arg(tr("Supported image files", "Mimetype")));
				if (!imageFileName.isEmpty())
				{
					if (!image.save(imageFileName))
					{
						QPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Critical, tr("File save error", "Message box title"),
						    tr("Cannot save image to:\n") + imageFileName, QMessageBox::Ok, q);
						msgBox->exec();
						delete msgBox;
					}
				}
			}
#endif // QT_NO_FILEDIALOG
		}
		else if (choice == copyTextAction)
		{
			const QString text = m_pageItems.at(pageNumber)->text(selectionRect);
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(text, QClipboard::Clipboard);
			if (clipboard->supportsSelection())
				clipboard->setText(text, QClipboard::Selection);
		}
	}
}

void PdfViewPrivate::addSelectionRect(const QPointF &scenePos) {
	m_selectionStart = scenePos;
	m_selectionRect = m_pageScene->addRect(m_selectionStart.x(), m_selectionStart.y(), 0, 0, QPen(QBrush(QColor(100, 160, 255)), 1), QBrush(QColor(100, 160, 255, 100))); // blue background
	m_selectionRect->setZValue(2);
}

void PdfViewPrivate::removeSelectionRect() {
	if (m_selectionRect)
	{
		m_pageScene->removeItem(m_selectionRect);
		delete m_selectionRect;
		m_selectionRect = 0;
	}
}

/*******************************************************************/
// Text selection

void PdfViewPrivate::getTextSelection(const QPointF &scenePos) {
	// get rectangle with top-left point and bottom-right point given by the start of the selection and the end of the selection
	const double selectionRectLeft = qMin(scenePos.x(), m_selectionStart.x());
	const double selectionRectTop = qMin(scenePos.y(), m_selectionStart.y());
	const double selectionRectWidth = qAbs(scenePos.x() - m_selectionStart.x());
	const double selectionRectHeight = qAbs(scenePos.y() - m_selectionStart.y());

	// translate selected rectangle to page coordinates
	const int pageNumber = pageNumberAtPosition(QPointF(selectionRectLeft, selectionRectTop));
	if (pageNumber < 0)
		return;
	const QRectF selectionRect = q->mapToPage(pageNumber, QRectF(selectionRectLeft, selectionRectTop, selectionRectWidth, selectionRectHeight));

	// remove old selection
	removeTextSelection();

	// get text boxes which are inside the selection and highlight them
	QList<Poppler::TextBox*> popplerTextList = m_pageItems.at(pageNumber)->textList();
	bool isSelected = false;
	for (int i = 0; i < popplerTextList.size(); ++i)
	{
		const QRectF popplerTextBoxRect = popplerTextList.at(i)->boundingBox();
		// get first selected word
		if (popplerTextBoxRect.right() > selectionRect.left() && popplerTextBoxRect.bottom() > selectionRect.top())
			isSelected = true;
		// get last selected word
		if (isSelected && ((popplerTextBoxRect.left() > selectionRect.right() && popplerTextBoxRect.bottom() > selectionRect.bottom()) || popplerTextBoxRect.top() > selectionRect.bottom()))
			break;
		// get all selected words
		if (isSelected)
		{
			const QRectF textBox = q->mapFromPage(pageNumber, popplerTextBoxRect);
			m_textSelectionRects << m_pageScene->addRect(textBox, QPen(QBrush(), 0), QBrush(QColor(100, 160, 255, 100))); // blue background
			m_textSelectionRects.last()->setZValue(2);
			m_textSelectionWords << popplerTextList.at(i)->text();
		}
	}
	qDeleteAll(popplerTextList);
}

void PdfViewPrivate::handleTextSelection(const QPoint &popupMenuPos) {
	if (m_textSelectionWords.isEmpty())
		return;

	// show popup menu with copy and save options
	QMenu menu(q);
//	QAction *copyTextAction = menu.addAction(Icon("edit-copy"), tr("Copy &Text to Clipboard"));
	QAction *copyTextAction = menu.addAction(tr("Copy &Text to Clipboard"));
	menu.addSeparator();
//	menu.addAction(Icon("dialog-cancel"), tr("&Cancel"));
	menu.addAction(tr("&Cancel"));
	QAction *choice = menu.exec(popupMenuPos);

	if (choice && choice == copyTextAction)
	{
		const QString text = m_textSelectionWords.join(QLatin1String(" "));
		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText(text, QClipboard::Clipboard);
		if (clipboard->supportsSelection())
			clipboard->setText(text, QClipboard::Selection);
	}
}

void PdfViewPrivate::removeTextSelection() {
	while (!m_textSelectionRects.isEmpty())
	{
		m_pageScene->removeItem(m_textSelectionRects.last());
		delete m_textSelectionRects.takeLast();
	}
	m_textSelectionWords.clear();
}

/*******************************************************************/
// Page switching

void PdfViewPrivate::scroll(int delta) {
	const int numDegrees = delta / 8;
	const int numSteps = numDegrees / 15;

	QScrollBar *vbar = q->verticalScrollBar();
	vbar->setValue(vbar->value() - 3 * numSteps * vbar->singleStep());
}

void PdfView::slotGoToStartOfDocument() {
	verticalScrollBar()->setValue(0);
}

void PdfView::slotGoToEndOfDocument() {
	QScrollBar *vbar = verticalScrollBar();
	vbar->setValue(vbar->maximum());
}

void PdfView::slotGoToPreviousPage() {
	if (d->m_pageNumber > 0)
		setPage(d->m_pageNumber - 1);
}

void PdfView::slotGoToNextPage() {
	if (d->m_popplerDocument && d->m_pageNumber < d->m_popplerDocument->numPages() - 1)
		setPage(d->m_pageNumber + 1);
}

/*******************************************************************/
// Print

void PdfView::slotPrint() {
	if (!d->m_popplerDocument)
		return;
	d->m_printHandler->print(d->m_popplerDocument, d->m_popplerPages, d->m_fileName, d->m_pageNumber);
}

/*******************************************************************/
// Events

#ifndef QT_NO_CONTEXTMENU
void PdfView::addContextMenuAction(QAction *action) {
	d->m_contextMenuActions << action;
}

void PdfView::removeContextMenuAction(QAction *action) {
	for (int i = 0; i < d->m_contextMenuActions.size(); ++i)
		if (d->m_contextMenuActions.at(i) == action)
			d->m_contextMenuActions.takeAt(i);
}

void PdfView::contextMenuEvent(QContextMenuEvent *event) {
}
#endif // QT_NO_CONTEXTMENU

void PdfView::keyPressEvent(QKeyEvent *event) {
	// make sure Space and Backspace don't scroll the page when a text field or checkbox has the focus
	if (d->m_pageScene->focusItem() && d->m_pageScene->focusItem()->isWidget())
	{
		QGraphicsView::keyPressEvent(event);
		return;
	}

	if (event->key() == Qt::Key_Space)
		verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepAdd);
	else if (event->key() == Qt::Key_Backspace)
		verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepSub);
	else if (event->key() == Qt::Key_Escape)
	{
		d->removeFindHighlight();
		d->removeTextSelection(); // dirty hack: use m_textSelectionRects in syncFromSource()
	}

	QGraphicsView::keyPressEvent(event);
}

void PdfView::mousePressEvent(QMouseEvent *event) {
	if (event->button() != Qt::LeftButton || PageItem::isLinkHovered())
	{
		QGraphicsView::mousePressEvent(event);
		return;
	}
	// remove focus from form fields (useful for letting Space and Backspace work again for page scrolling)
	d->m_pageScene->setFocusItem(0);
	// click in forms
	if (itemAt(event->pos()) && itemAt(event->pos())->isWidget())
	{
		static_cast<QGraphicsProxyWidget*>(itemAt(event->pos()))->widget()->setFocus();
		QGraphicsView::mousePressEvent(event);
		return;
	}
	// start magnifying
	if (d->m_mouseTool == Magnifying)
	{
		d->m_isDragging = true;
		d->magnify(mapToScene(event->pos()));
	}
	else if (d->m_mouseTool == Browsing)
	{
		d->m_isDragging = true;
#ifndef QT_NO_CURSOR
//		QApplication::setOverrideCursor(Qt::SizeAllCursor);
		QApplication::setOverrideCursor(Qt::ClosedHandCursor);
#endif // QT_NO_CURSOR
		d->m_mouseOldPos = event->pos();
	}
	else if (d->m_mouseTool == Selection)
	{
		d->m_isDragging = true;
#ifndef QT_NO_CURSOR
		QApplication::setOverrideCursor(Qt::CrossCursor);
#endif // QT_NO_CURSOR
		d->addSelectionRect(mapToScene(event->pos()));
	}
	else if (d->m_mouseTool == TextSelection)
	{
		d->m_isDragging = true;
#ifndef QT_NO_CURSOR
		QApplication::setOverrideCursor(Qt::IBeamCursor);
#endif // QT_NO_CURSOR
		d->m_selectionStart = mapToScene(event->pos());
	}
}

void PdfView::mouseMoveEvent(QMouseEvent *event) {
	if (d->m_isDragging)
	{
		if (d->m_mouseTool == Magnifying)
			d->magnify(mapToScene(event->pos()));
		if (d->m_mouseTool == Browsing)
		{
			// calculate mouse position change
			const QPoint delta = d->m_mouseOldPos - event->pos();
			d->m_mouseOldPos = event->pos();
			// if the mouse cursor moves out of the view, then let it wrap around the edges of the view
			const QPoint globalPos = event->globalPos();
			const QPoint upperLeftPos = mapToGlobal(QPoint(0,0));
			const QPoint lowerRightPos = mapToGlobal(QPoint(viewport()->width(), viewport()->height()));
#ifndef QT_NO_CURSOR
			if (globalPos.x() > lowerRightPos.x())
			{
				QCursor::setPos(upperLeftPos.x() + globalPos.x() - lowerRightPos.x(), globalPos.y());
				d->m_mouseOldPos += QPoint(upperLeftPos.x() - lowerRightPos.x(), 0);
			}
			if (globalPos.x() < upperLeftPos.x())
			{
				QCursor::setPos(lowerRightPos.x() + globalPos.x() - upperLeftPos.x(), globalPos.y());
				d->m_mouseOldPos += QPoint(lowerRightPos.x() - upperLeftPos.x(), 0);
			}
			if (globalPos.y() > lowerRightPos.y())
			{
				QCursor::setPos(globalPos.x(), upperLeftPos.y() + globalPos.y() - lowerRightPos.y());
				d->m_mouseOldPos += QPoint(0, upperLeftPos.y() - lowerRightPos.y());
			}
			if (globalPos.y() < upperLeftPos.y())
			{
				QCursor::setPos(globalPos.x(), lowerRightPos.y() + globalPos.y() - upperLeftPos.y());
				d->m_mouseOldPos += QPoint(0, lowerRightPos.y() - upperLeftPos.y());
			}
#endif // QT_NO_CURSOR
			// move the contents of the view
			horizontalScrollBar()->setValue(qMax(0, horizontalScrollBar()->value() + delta.x()));
			verticalScrollBar()->setValue(qMax(0, verticalScrollBar()->value() + delta.y()));
		}
		else if (d->m_mouseTool == Selection)
		{
			const QPointF pos = mapToScene(event->pos());
			const double selectionRectLeft = qMin(pos.x(), d->m_selectionStart.x());
			const double selectionRectTop = qMin(pos.y(), d->m_selectionStart.y());
			const double selectionRectWidth = qAbs(pos.x() - d->m_selectionStart.x());
			const double selectionRectHeight = qAbs(pos.y() - d->m_selectionStart.y());
			d->m_selectionRect->setRect(selectionRectLeft, selectionRectTop, selectionRectWidth, selectionRectHeight);
		}
		else if (d->m_mouseTool == TextSelection)
		{
			d->getTextSelection(mapToScene(event->pos()));
		}
		return;
	}

#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif // QT_NO_CURSOR
	d->findLinkAtPosition(mapToScene(event->pos()));
	// show tooltip
	if (PageItem::isLinkHovered())
	{
#ifndef QT_NO_CURSOR
		QApplication::setOverrideCursor(Qt::PointingHandCursor);
#endif // QT_NO_CURSOR
#ifndef QT_NO_TOOLTIP
		QToolTip::showText(mapToGlobal(event->pos()), PageItem::toolTipText(), this);
#endif // QT_NO_TOOLTIP
	}
#ifndef QT_NO_TOOLTIP
	else
		QToolTip::hideText();
#endif // QT_NO_TOOLTIP
}

void PdfView::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() != Qt::LeftButton)
	{
		QGraphicsView::mouseReleaseEvent(event);
		return;
	}
	// click in forms
	if (itemAt(event->pos()) && itemAt(event->pos())->isWidget())
	{
		QGraphicsView::mouseReleaseEvent(event);
		return;
	}
	if (PageItem::isLinkHovered()) // click on link
	{
		Link hoveredLink = PageItem::hoveredLink();
		if (!hoveredLink.url.isEmpty()) // we have a Browse link
			QDesktopServices::openUrl(QUrl(hoveredLink.url));
		else if (hoveredLink.pageNumber >= 0) // we have a Goto link
			setPage(hoveredLink.pageNumber);
	}
	else if (d->m_mouseTool == Magnifying && d->m_isDragging) // stop magnifying
	{
		d->m_isDragging = false;
		d->endMagnify();
	}
	else if (d->m_mouseTool == Browsing && d->m_isDragging)
	{
		d->m_isDragging = false;
#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
#endif // QT_NO_CURSOR
	}
	else if (d->m_mouseTool == Selection && d->m_isDragging)
	{
		d->m_isDragging = false;
#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
#endif // QT_NO_CURSOR
		d->handleSelection(event->globalPos());
		d->removeSelectionRect();
	}
	else if (d->m_mouseTool == TextSelection && d->m_isDragging)
	{
		d->m_isDragging = false;
#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
#endif // QT_NO_CURSOR
		d->handleTextSelection(event->globalPos());
		d->removeTextSelection();
	}
}

#ifndef QT_NO_WHEELEVENT
void PdfView::wheelEvent(QWheelEvent *event) {
	if (event->modifiers() == Qt::ControlModifier)
	{
		if (event->delta() > 0)
			slotZoomIn();
		else
			slotZoomOut();
	}
	else
		d->scroll(event->delta());
}
#endif // QT_NO_WHEELEVENT
