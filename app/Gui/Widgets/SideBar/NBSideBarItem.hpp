/*
	*
	* NBSideBarGroup.hpp - SideBarGruop class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBDeleteProcess.hpp"
#include "NBProcessManager.hpp"

class NBSideBarItem : public QLabel {
	Q_OBJECT

	public:
		NBSideBarItem( QWidget *parent );
		NBSideBarItem( QString name, QString icon, QString target, int type, QWidget *parent );

		QString name();
		void setName( QString );

		QIcon icon();
		void setIcon( QString );

		QString target();
		void setTarget( QString );

		int type();
		void setType( int );

		bool isHighlighted();
		void setHighlighted( bool );

		bool operator==( NBSideBarItem* );

		enum Type {
			Device = 0x761CD7,
			Vfs,
			Bookmark,
			QuickFile,
			Folders,
			Applications,
			Catalogs,
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
