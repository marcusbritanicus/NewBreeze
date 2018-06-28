/*
	*
	* NBSidePanelItem.hpp - SidePanelItem class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDeleteProcess.hpp"
#include "NBProcessManager.hpp"

class NBSidePanelLabel : public QWidget {
	Q_OBJECT

	public:
		NBSidePanelLabel( QString name, QString icon, QWidget *parent );

	private:
		QString mName;
		QPixmap mIcon;

	protected:
		void mouseReleaseEvent( QMouseEvent *mrEvent );
		void paintEvent( QPaintEvent *pEvent );

	Q_SIGNALS:
		void clicked();
};

class NBSidePanelItem : public QWidget {
	Q_OBJECT

	public:
		NBSidePanelItem( QString name, QString icon, QString target, int type, QWidget *parent );

		QString name();
		QIcon icon();
		QString target();
		int type();

		bool isHighlighted();
		void setHighlighted( bool );

		bool operator==( NBSidePanelItem* );

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

		bool mHighlight;
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
