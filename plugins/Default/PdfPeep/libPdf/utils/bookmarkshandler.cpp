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

#include "bookmarkshandler.h"

#include <QMenu>
#include "filesettings.h"
//#include "icon.h"
//#include "../shortcuthandler/shortcuthandler.h"

BookmarksHandler::BookmarksHandler(BookmarksWidget *parent)
	: QObject(parent->widget())
	, m_bookmarksMenuAction(0)
{
	m_bookmarksWidget = parent;

	m_bookmarksMenu = new QMenu(tr("&Bookmarks", "Menu title"), parent->widget());

//	QAction *setBookmarkAction = new QAction(Icon("bookmark-new"), tr("Set &Bookmark", "Action"), m_bookmarksMenu);
	QAction *setBookmarkAction = new QAction(tr("Set &Bookmark", "Action"), m_bookmarksMenu);
	setBookmarkAction->setObjectName(QLatin1String("pdfview_bookmarks_set"));
#ifndef QT_NO_SHORTCUT
	setBookmarkAction->setShortcut(tr("Ctrl+B", "Bookmarks|Set"));
#endif // QT_NO_SHORTCUT
#ifndef QT_NO_STATUSTIP
	setBookmarkAction->setStatusTip(tr("Set or unset a bookmark at the current line"));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
	setBookmarkAction->setWhatsThis(tr("<p>Set or unset a bookmark at the current line.</p>"));
#endif // QT_NO_WHATSTHIS
	connect(setBookmarkAction, SIGNAL(triggered()), this, SLOT(toggleBookmark()));
	m_bookmarksMenu->addAction(setBookmarkAction);
//#ifndef QT_NO_SHORTCUT
//	ShortcutHandler::instance()->addAction(setBookmarkAction);
//#endif // QT_NO_SHORTCUT

//	QAction *previousBookmarkAction = new QAction(Icon("go-up"), tr("&Previous Bookmark", "Action"), m_bookmarksMenu);
	QAction *previousBookmarkAction = new QAction(tr("&Previous Bookmark", "Action"), m_bookmarksMenu);
	previousBookmarkAction->setObjectName(QLatin1String("pdfview_bookmarks_prev"));
#ifndef QT_NO_SHORTCUT
	previousBookmarkAction->setShortcut(tr("Alt+Up", "Bookmarks|Previous"));
#endif // QT_NO_SHORTCUT
#ifndef QT_NO_STATUSTIP
	previousBookmarkAction->setStatusTip(tr("Go to the previous bookmark"));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
	previousBookmarkAction->setWhatsThis(tr("<p>Go to the previous bookmark.</p>"));
#endif // QT_NO_WHATSTHIS
	previousBookmarkAction->setEnabled(false);
	connect(previousBookmarkAction, SIGNAL(triggered()), this, SLOT(goToPreviousBookmark()));
	m_bookmarksMenu->addAction(previousBookmarkAction);
//#ifndef QT_NO_SHORTCUT
//	ShortcutHandler::instance()->addAction(previousBookmarkAction);
//#endif // QT_NO_SHORTCUT

//	QAction *nextBookmarkAction = new QAction(Icon("go-down"), tr("&Next Bookmark", "Action"), m_bookmarksMenu);
	QAction *nextBookmarkAction = new QAction(tr("&Next Bookmark", "Action"), m_bookmarksMenu);
	nextBookmarkAction->setObjectName(QLatin1String("pdfview_bookmarks_next"));
#ifndef QT_NO_SHORTCUT
	nextBookmarkAction->setShortcut(tr("Alt+Down", "Bookmarks|Next"));
#endif // QT_NO_SHORTCUT
#ifndef QT_NO_STATUSTIP
	nextBookmarkAction->setStatusTip(tr("Go to the next bookmark"));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
	nextBookmarkAction->setWhatsThis(tr("<p>Go to the next bookmark.</p>"));
#endif // QT_NO_WHATSTHIS
	nextBookmarkAction->setEnabled(false);
	connect(nextBookmarkAction, SIGNAL(triggered()), this, SLOT(goToNextBookmark()));
	m_bookmarksMenu->addAction(nextBookmarkAction);
//#ifndef QT_NO_SHORTCUT
//	ShortcutHandler::instance()->addAction(nextBookmarkAction);
//#endif // QT_NO_SHORTCUT

	m_bookmarksMenu->addSeparator();
}

BookmarksHandler::~BookmarksHandler()
{
	delete m_bookmarksMenu;
}

QAction *BookmarksHandler::menuAction()
{
	if (!m_bookmarksMenuAction)
	{
		m_bookmarksMenuAction = new QAction(parent());
		m_bookmarksMenuAction->setText(m_bookmarksMenu->title());
		m_bookmarksMenuAction->setMenu(m_bookmarksMenu);
	}
	return m_bookmarksMenuAction;
}

QMenu *BookmarksHandler::menu()
{
	return m_bookmarksMenu;
}

QAction *BookmarksHandler::action(int which)
{
	QList<QAction*> bookmarkActions = m_bookmarksMenu->actions();
	if (which < 0 || which >= bookmarkActions.size())
		return 0;
	return bookmarkActions.at(which);
}

/***************************************************************************/

void BookmarksHandler::setBookmarks(const QList<double> &bookmarks)
{
	m_bookmarks = bookmarks;
}

QList<double> BookmarksHandler::bookmarks() const
{
	return m_bookmarks;
}

void BookmarksHandler::setPageLabels(const QStringList &labels)
{
	m_pageLabels = labels;
}

void BookmarksHandler::updateActions()
{
	const double pos = m_bookmarksWidget->position();
	int which = -1;
	QList<QAction*> bookmarkActions = m_bookmarksMenu->actions();
	for (int i = 0; i < m_bookmarks.size(); ++i)
	{
		if (qFuzzyCompare(m_bookmarks.at(i), pos))
		{
			which = i;
			break;
		}
	}
	bookmarkActions.at(0)->setText(which >= 0 ? tr("Unset &Bookmark", "Action") : tr("Set &Bookmark", "Action"));
	// XXX disabling the following causes Alt+Up and Alt+Down not to be caught anymore :(
	bookmarkActions.at(1)->setEnabled(m_bookmarks.size() > 0 && pos > m_bookmarks.at(0) && !qFuzzyCompare(pos, m_bookmarks.at(0)));
	bookmarkActions.at(2)->setEnabled(m_bookmarks.size() > 0 && pos < m_bookmarks.at(m_bookmarks.size() - 1) && !qFuzzyCompare(pos, m_bookmarks.at(m_bookmarks.size() - 1)));
}

/***************************************************************************/
// Adding and removing bookmarks

void BookmarksHandler::insertBookmark(int index, double pos)
{
//	QAction *action = new QAction(tr("Page %1").arg(QString::number(int(pos))), m_bookmarksMenu);
	Q_ASSERT_X(int(pos) < m_pageLabels.size(), "BookmarksHandler", "make sure to call setPageLabels() before inserting bookmarks with insertBookmark(), appendBookmark(), toggleBookmark() or loadBookmarks()");
	QAction *action = new QAction(tr("Page %1").arg(m_pageLabels.at(int(pos))), m_bookmarksMenu);
	action->setData(pos);
	connect(action, SIGNAL(triggered()), this, SLOT(goToActionBookmark()));
	if (index >= 0 && index < m_bookmarks.size())
	{
		m_bookmarks.insert(index, pos);
		m_bookmarksMenu->insertAction(m_bookmarksMenu->actions().at(index+4), action);
	}
	else // append at the end
	{
		m_bookmarks.append(pos);
		m_bookmarksMenu->addAction(action);
	}
	updateActions();
	Q_EMIT bookmarkUpdated(pos);
}

void BookmarksHandler::appendBookmark(double pos)
{
	if (pos < 0)
		return;
	insertBookmark(-1, pos); // using -1 as index ensures that pos will be appended at the end of the bookmarks list
}

void BookmarksHandler::removeBookmark(int index)
{
	if (index >= 0 && index < m_bookmarks.size())
	{
		const double pos = m_bookmarks.at(index);
		m_bookmarks.removeAt(index);
		m_bookmarksMenu->removeAction(m_bookmarksMenu->actions().at(index+4)); // 4 is the number of actions defined in the constructor
		updateActions();
		Q_EMIT bookmarkUpdated(pos);
	}
}

void BookmarksHandler::removeBookmarkAtPosition(double pos)
{
	removeBookmark(m_bookmarks.indexOf(pos));
}

void BookmarksHandler::toggleBookmark()
{
	const double pos = m_bookmarksWidget->position();
	for (int i = 0; i < m_bookmarks.size(); ++i)
	{
		if (qFuzzyCompare(m_bookmarks.at(i), pos))
		{
			removeBookmark(i);
			return;
		}
		else if (m_bookmarks.at(i) > pos)
		{
			insertBookmark(i, pos);
			return;
		}
	}
	appendBookmark(pos); // if pos is larger than any number in the list, then we insert pos at the end of the list
}

void BookmarksHandler::clear()
{
	while (!m_bookmarks.isEmpty())
		removeBookmark(0);
}

/***************************************************************************/
// Going to a bookmark

void BookmarksHandler::goToActionBookmark()
{
	QAction *action = qobject_cast<QAction*>(sender());
	Q_EMIT goToPosition(action->data().toDouble());
}

void BookmarksHandler::goToPreviousBookmark()
{
	const double pos = m_bookmarksWidget->position();
	for (int i = m_bookmarks.size() - 1; i >= 0; --i)
	{
		if (qFuzzyCompare(pos, m_bookmarks.at(i)) && i > 0) // when the bookmarks are saved and reloaded on the next startup, they are not exact anymore, so we must use qFuzzyCompare() to test whether we are on a bookmark; this must happen before the else-part to avoid staying on the current bookmark
		{
			Q_EMIT goToPosition(m_bookmarks.at(i-1));
			return;
		}
		else if (pos > m_bookmarks.at(i))
		{
			Q_EMIT goToPosition(m_bookmarks.at(i));
			return;
		}
	}
}

void BookmarksHandler::goToNextBookmark()
{
	const double pos = m_bookmarksWidget->position();
	for (int i = 0; i < m_bookmarks.size(); ++i)
	{
		if (qFuzzyCompare(pos, m_bookmarks.at(i)) && i < m_bookmarks.size() - 1) // when the bookmarks are saved and reloaded on the next startup, they are not exact anymore, so we must use qFuzzyCompare() to test whether we are on a bookmark; this must happen before the else-part to avoid staying on the current bookmark
		{
			Q_EMIT goToPosition(m_bookmarks.at(i+1));
			return;
		}
		else if (pos < m_bookmarks.at(i))
		{
			Q_EMIT goToPosition(m_bookmarks.at(i));
			return;
		}
	}
}

/***************************************************************************/
// Recalculate the positions of the bookmarks. If in a text editor some lines are added or removed, then the bookmarks below it must be moved along with the corresponding lines and the bookmarks in the removed area must be removed themselves too.

void BookmarksHandler::recalculateBookmarks(double pos, double offset)
{
	if (offset == 0)
		return;

	for (int i = 0; i < m_bookmarks.size(); ++i)
	{
		// if an area containing a bookmark is removed, then the bookmark itself must also be removed
		if (offset < 0 && pos <= m_bookmarks.at(i) && pos - offset > m_bookmarks.at(i))
		{
			m_bookmarks.removeAt(i);
			--i;
		}
		else if (pos <= m_bookmarks.at(i)) // shift all bookmarks that come after the insertion or deletion
			m_bookmarks[i] += offset;
	}
}

/***************************************************************************/
// Load and save bookmarks from disk

void BookmarksHandler::loadBookmarks(const QString &fileName)
{
	m_fileName = fileName;
	while (!m_bookmarks.isEmpty())
		removeBookmark(0);

	// Load file specific bookmarks
	FileSettings fileSettings(m_fileName);
	QList<QVariant> bookmarksVariantList = fileSettings.value(QLatin1String("Bookmarks")).toList();
	for (int i = 0; i < bookmarksVariantList.size(); ++i)
		appendBookmark(bookmarksVariantList.at(i).toDouble());
}

void BookmarksHandler::saveBookmarks()
{
	QList<double> bookmarksList = bookmarks();
//	if (bookmarksList.isEmpty())
//	{
//		const QString fileSettingsFile = fileSettingsPath(m_fileName);
//		if (QFile::exists(fileSettingsFile))
//			QFile::remove(fileSettingsFile);
//	}
	FileSettings fileSettings(m_fileName);
	if (!bookmarksList.isEmpty())
	{
		QList<QVariant> bookmarksVariantList; // cannot use QStringList because QString::number() loses precision, QVariant() also loses precision :( but less
		for (int i = 0; i < bookmarksList.size(); ++i)
			bookmarksVariantList << QVariant(bookmarksList.at(i));
		fileSettings.setValue(QLatin1String("Bookmarks"), bookmarksVariantList);
	}
	else
		fileSettings.remove(QLatin1String("Bookmarks"));
}
