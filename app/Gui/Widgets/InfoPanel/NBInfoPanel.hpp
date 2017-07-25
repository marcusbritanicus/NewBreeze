/*
	*
	* NBInfoPanel.hpp - SideBar class Header
	*
*/

#pragma once

#include "Global.hpp"

class NBInfoPanel : public QWidget {
	Q_OBJECT

	public:
		NBInfoPanel( QWidget *parent );

		void updatePanel( QString, QModelIndexList );

	private:
		QLabel *iconLbl;
		QLabel *infoLbl;

		void setIcon( QPixmap );
		void setIcon( QIcon );

		void setWidgetProperties();

	public slots:

	protected:
		void paintEvent( QPaintEvent* );

	Q_SIGNALS:
};
