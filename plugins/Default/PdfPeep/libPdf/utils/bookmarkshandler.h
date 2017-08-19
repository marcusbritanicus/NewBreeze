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

#ifndef BOOKMARKSHANDLER_H
#define BOOKMARKSHANDLER_H

#include <QWidget>

class QMenu;

class BookmarksWidget
{
public:
	virtual QWidget *widget()
	{
		return new QWidget();
	}
	virtual double position () const
	{
		return 0;
	}
};

class BookmarksHandler : public QObject
{
	Q_OBJECT

public:
	BookmarksHandler(BookmarksWidget *parent);
	~BookmarksHandler();

	/**
	 * Returns a pointer to an action which has the bookmarks menu set
	 * as popup menu.
	 * \return an action with popup menu containing the bookmark actions
	 */
	QAction *menuAction();
	/**
	 * Returns a pointer to the menu containing all the actions related
	 * to bookmark handling, such as "Set/Unset Bookmark", "Previous Bookmark",
	 * ...
	 * \return the menu with the bookmark actions
	 */
	QMenu *menu();
	/**
	 * \return the <em>which</em>-th action in the bookmarks menu
	 */
	QAction *action(int which);
	/**
	 * Set the bookmarks for the current file to the specified list
	 * \param bookmarks a list containing the new bookmarks
	 */
	void setBookmarks(const QList<double> &bookmarks);
	/**
	 * Returns the current list of bookmarks for the current file.
	 * \return the list of bookmarks
	 */
	QList<double> bookmarks() const;
	void setPageLabels(const QStringList &labels);
	/**
	 * Call this function whenever the view containing the pages is scrolled
	 * in order to enable/disable the necessary actions.
	 */
	void updateActions();
	/**
	 * Call this function to remove the position pos from the list of
	 * bookmarks (if <em>pos</em> is not in that list, nothing happens).
	 * This can be used for example when it is detected that an existing
	 * bookmark is invalid.
	 */
	void removeBookmarkAtPosition(double pos);
	/**
	 * Empties the list of bookmarks. This function does <em>not</em> store
	 * the empty bookmarks list to the file specific settings on disk.
	 * In order to store the empty bookmarks list to the file specific
	 * settings on disk, you must call saveBookmarks() after calling clear().
	 */
	void clear();
	/**
	 * When lines are added or removed in the editor (if the BookmarksWidget
	 * object is an editor), this functions allows to recalculate the line
	 * numbers of the bookmarks and remove the bookmarks which were on
	 * removed lines. The line numbers of the remaining bookmarks are
	 * recalculated so that they still point to the correct text.
	 */
	void recalculateBookmarks(double pos, double offset);
	/**
	 * Load the bookmarks from the file specific settings stored on disk.
	 */
	void loadBookmarks(const QString &fileName);
	/**
	 * Save the current bookmarks to the file specific settings on disk.
	 */
	void saveBookmarks();

Q_SIGNALS:
	/**
	 * This signal is emitted when a bookmark is added or removed at position
	 * pos. The BookmarksWidget object can connect this signal to a slot in
	 * which a visual indication of this change is given (for example, a "B"
	 * can be added to a line number widget at the left of the view).
	 */
	void bookmarkUpdated(double pos);
	/**
	 * When this signal is emitted, the BookmarksWidget object should be
	 * requested to scroll its contents to position pos.
	 * This signal is emitted when the user has triggered the bookmarks
	 * handler's "Go to previous bookmark" or "Go to next bookmark" action.
	 * This signal should be connected to a slot in the BookmarksWidget
	 * object in which the contents of the BookmarksWidget object are
	 * scrolled to the position <em>pos</em>.
	 */
	void goToPosition(double pos);

private Q_SLOTS:
	void toggleBookmark();
	void goToActionBookmark();
	void goToPreviousBookmark();
	void goToNextBookmark();

private:
	void insertBookmark(int index, double pos);
	void appendBookmark(double pos);
	void removeBookmark(int index);

	BookmarksWidget *m_bookmarksWidget;
	QAction *m_bookmarksMenuAction;
	QMenu *m_bookmarksMenu;
	QList<double> m_bookmarks;
	QString m_fileName;
	QStringList m_pageLabels;
};

#endif // BOOKMARKSHANDLER_H
