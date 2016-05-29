/*
	*
	* NBSideBarGroup.hpp - SideBarGruop class Header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBProcessManager.hpp>

class NBSideBarItem : public QLabel {
	Q_OBJECT

	public:
		NBSideBarItem( QWidget *parent );
		NBSideBarItem( QString name, QString icon, QString target, QWidget *parent );

		QString name();
		void setName( QString );

		QIcon icon();
		void setIcon( QString );

		QString target();
		void setTarget( QString );

		bool isHighlighted();
		void setHighlighted( bool );

		bool operator==( NBSideBarItem* );

	private:
		QString mName;
		QString mIcon;
		QString mTarget;

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

	Q_SIGNALS:
		void clicked();
		void clicked( QString );
};
