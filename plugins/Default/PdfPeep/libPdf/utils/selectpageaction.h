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

#ifndef SELECTPAGEACTION_H
#define SELECTPAGEACTION_H

#include "selectaction.h"

class SelectPageAction : public SelectAction
{
	Q_OBJECT

public:
	explicit SelectPageAction(QObject *parent, const QString &name = 0);
	~SelectPageAction();

	void setPageLabels(const QStringList &labels);

Q_SIGNALS:
	void pageSelected(int value);

private Q_SLOTS:
	void slotGoToPage(const QString &pageLabelText);
};

#endif // SELECTPAGEACTION_H
