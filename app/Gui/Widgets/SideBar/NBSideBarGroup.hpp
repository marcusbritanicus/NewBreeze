/*
	*
	* NBSideBarGroup.hpp - SideBarGruop class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBSideBarItem.hpp"

class NBGroupHeader : public QLabel {
	Q_OBJECT

	public:
		NBGroupHeader( QString name, QString icon, QWidget *parent );

	private:
		QString mName;
		QString mIcon;

	protected:
		void mouseReleaseEvent( QMouseEvent *mEvent );

	Q_SIGNALS:
		void clicked();
};

class NBSideBarGroup : public QWidget {
	Q_OBJECT

	public:
		NBSideBarGroup( QString name, QString icon, QWidget *parent );

		int itemCount();
		NBSideBarItem* item( int );
		int addItem( QString name, QString icon, QString target );

		void clear();
		void clearHighlights();

	private:
		NBGroupHeader *header;

		QList<NBSideBarItem*> itemList;

		QWidget *itemsBase;
		QVBoxLayout *itemsLayout;

	private Q_SLOTS:
		void showHideItems();

	Q_SIGNALS:
		void clicked( QString );
};
