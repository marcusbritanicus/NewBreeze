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

#include "selectpageaction.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

SelectPageAction::SelectPageAction(QObject *parent, const QString &name)
	: SelectAction(parent)
{
	if (!name.isEmpty())
		setObjectName(name);

	connect(this, SIGNAL(triggered(QString)), this, SLOT(slotGoToPage(QString)));
}

SelectPageAction::~SelectPageAction()
{
}

void SelectPageAction::setPageLabels(const QStringList &labels)
{
	disconnect(this, SIGNAL(triggered(QString)), this, SLOT(slotGoToPage(QString)));
	clear();

	const int pageCount = labels.size();
	QStringList pageLabelTexts;
	pageLabelTexts.reserve(pageCount);
	for (int i = 0; i < pageCount; ++i)
		pageLabelTexts << labels.at(i) + QLatin1String(" (") + QString::number(i+1) + QLatin1String(" / ") + QString::number(pageCount) + QLatin1Char(')');
	setItems(pageLabelTexts);
	connect(this, SIGNAL(triggered(QString)), this, SLOT(slotGoToPage(QString)));
}

void SelectPageAction::slotGoToPage(const QString &pageLabelText)
{
	const int start = pageLabelText.indexOf(QLatin1Char('(')) + 1; // pageLabelText is of the form "iv (4 / 316)", so we extract the "4"
	const int pageNumber = pageLabelText.mid(start, pageLabelText.indexOf(QLatin1Char('/')) - start).toInt() - 1;
	Q_EMIT pageSelected(pageNumber);
}
