/*
	*
	* NBSideBarMenu.hpp - SidePanelItem class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDeleteProcess.hpp"
#include "NBProcessManager.hpp"

class NBSideBarMenu : public QWidget {
	Q_OBJECT

	public:
		NBSideBarMenu( QString icon, QString name, QWidget *parent );

	private:
		QString mName;
		QPixmap mIcon;

		int mHeight;
		int mWidth;

		void buildMenu();

		QVBoxLayout *addDeviceEntries();
		QVBoxLayout *addCatalogEntries();
		QVBoxLayout *addBookmarkEntries();
		QVBoxLayout *addQuickFileEntries();

		QFontMetrics *fm;

	public Q_SLOTS:
		void show( QPoint );

	protected:
		void closeEvent( QCloseEvent* );
		void mousePressEvent( QMouseEvent *mpEvent );

		void paintEvent( QPaintEvent* );

	Q_SIGNALS:
		void driveClicked( QString );
		void closed();
};

class NBSideBarMenuEntry : public QWidget {
	Q_OBJECT

	public:
		NBSideBarMenuEntry( QString name, QString icon, QString target, int type, QWidget *parent );

		QString name();
		QIcon icon();
		QString target();
		int type();

		bool operator==( NBSideBarMenuEntry* );

		enum Type {
			Device = 0x761CD7,
			Vfs,
			Bookmark,
			Catalogs,
			QuickFile,
			Trash
		};

	private:
		QString mName;
		QString mIcon;
		QString mTarget;
		int mType;

		bool mPressed;
		bool mHover;

		QLabel *iconLbl, *textLbl;

	protected:
		void enterEvent( QEvent* );
		void leaveEvent( QEvent* );

		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );

		void mouseReleaseEvent( QMouseEvent *mrEvent );
		void mousePressEvent( QMouseEvent *mpEvent );
		void paintEvent( QPaintEvent *pEvent );

	private Q_SLOTS:
		void removeBookmark();
		void removeQuickFile();

	Q_SIGNALS:
		void clicked();
		void clicked( QString );
};
