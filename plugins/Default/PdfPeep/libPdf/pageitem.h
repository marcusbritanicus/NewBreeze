/***************************************************************************
 *   Copyright (C) 2012 by Glad Deschrijver                                *
 *     <glad.deschrijver@gmail.com>                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef PDFVIEWER_PAGEITEM_H
#define PDFVIEWER_PAGEITEM_H

#include <QObject>
#if QT_VERSION >= 0x050000
	#include <poppler-qt5.h>
#else
	#include <poppler-qt4.h>
#endif

struct Link
{
	QRectF linkArea;
	double pageNumber;
	QString pageLabel;
	QString url;
	Poppler::LinkAction::ActionType actionType;
};
//Q_DECLARE_TYPEINFO(Link, Q_PRIMITIVE_TYPE);
struct FormField
{
	QRectF rect;
	Poppler::FormField *field;
};

class PageItem : public QObject
{
	Q_OBJECT

public:
	explicit PageItem(Poppler::Page *popplerPage, QObject *parent = 0);
	~PageItem();

	QSizeF pageSizeF() const;
	QString label() const;
	QImage renderToImage(double xres = 72.0, double yres = 72.0, int x = -1, int y = -1, int w = -1, int h = -1, Poppler::Page::Rotation rotate = Poppler::Page::Rotate0) const;
	QString text(const QRectF &rect, Poppler::Page::TextLayout textLayout) const;
	QString text(const QRectF &rect) const;
	QList<Poppler::TextBox*> textList(Poppler::Page::Rotation rotate = Poppler::Page::Rotate0) const;
	bool search(const QString &text, double &rectLeft, double &rectTop, double &rectRight, double &rectBottom, Poppler::Page::SearchDirection direction, Poppler::Page::SearchMode caseSensitive, Poppler::Page::Rotation rotate = Poppler::Page::Rotate0) const;

	void generateFormFields();
	QList<FormField> formFields() const;
	QList<QWidget*> formWidgets() const;

	void generateLinks(const QStringList &popplerPageLabels);
	QList<Link> links() const;
	void findLinkAtPosition(const QPointF &pagePos);
	static bool isLinkHovered();
	static Link hoveredLink();
	static QString toolTipText();

private Q_SLOTS:
	void slotSetFormData();
	void slotSetFormData(const QString &text);
	void slotSetFormData(bool checked);
	void slotSetFormData(int index);

private:
	Poppler::Page *m_popplerPage;

	QList<Link> m_links;
	static Link s_hoveredLink;
	static bool s_isLinkHovered;

	QList<FormField> m_formFields;
	QList<QWidget*> m_formWidgets;
};

#endif // PDFVIEWER_PAGEITEM_H
