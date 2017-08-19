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

#ifndef PDFVIEWER_PRINTHANDLER_H
#define PDFVIEWER_PRINTHANDLER_H

#include <QWidget>
#if QT_VERSION >= 0x050000
	#include <poppler-qt5.h>
#else
	#include <poppler-qt4.h>
#endif

class QComboBox;
class QRadioButton;

class PrintHandler : public QObject
{
	Q_OBJECT

public:
	explicit PrintHandler(QWidget *parent = 0);
	~PrintHandler();

	void print(Poppler::Document *popplerDocument, QList<Poppler::Page*> popplerPages, const QString &fileName, int pageNumber);

private:
	QWidget *cupsOptionsPagesWidget();
	QStringList cupsOptions() const;

	QRadioButton *m_onePageRadio;
	QRadioButton *m_twoPagesRadio;
	QRadioButton *m_fourPagesRadio;
	QRadioButton *m_sixPagesRadio;
	QRadioButton *m_ninePagesRadio;
	QRadioButton *m_sixteenPagesRadio;
	QComboBox *m_pagesFlowComboBox;
};

#endif // PDFVIEWER_PRINTHANDLER_H
