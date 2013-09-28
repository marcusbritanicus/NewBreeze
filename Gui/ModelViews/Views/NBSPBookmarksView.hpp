/*
	*
	* NBSPBookmarksView.hpp - NBSPBookmarksView.cpp Header
	*
*/

#ifndef NBSPBOOKMARKSVIEW_HPP
#define NBSPBOOKMARKSVIEW_HPP

#include <Global.hpp>
#include <NBBookmarksModel.hpp>
#include <NBSPBookmarksDelegate.hpp>

class NBBookmarkEdit : public QLineEdit {
	Q_OBJECT

	public:
		NBBookmarkEdit( QString text, int row );
		int bookmarkIndex;

	protected:
		virtual void keyPressEvent( QKeyEvent* );
};

class NBSPBookmarksView : public QListView {
	Q_OBJECT

	public :
		NBSPBookmarksView();
		QSize sizeHint() const;
		QSize minimumSizeHint() const;

		int maxItemWidth() const;

	private :
		void renameBookMark( QModelIndex );
		void showMenu( QModelIndex, QPoint );

		NBBookmarksModel *dataModel;
		NBSPBookmarksDelegate *dataDelegate;

		NBBookmarkEdit *bookmarkEdit;

		QFileSystemWatcher *settingsWatcher;
		int widthHint;

	protected:
		void mousePressEvent( QMouseEvent * );

	private slots:
		void updateText();

		void moveBookmarkUp();
		void moveBookmarkDown();
		void removeBookmark();

	signals :
		void mountDrive( QString );
};

#endif
