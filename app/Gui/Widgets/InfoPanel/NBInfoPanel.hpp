/*
	*
	* NBInfoPanel.hpp - InfoPanel class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBMediaInfo.hpp"

class NBInfoPanel : public QWidget {
	Q_OBJECT

	public:
		NBInfoPanel( QWidget *parent );

		void updatePanel( QString, QModelIndexList );

	private:
		QLabel *iconLbl;
		QLabel *nameLbl;
		QLabel *sizeLbl;
		QLabel *pathLbl;
		QLabel *permLbl;
		QLabel *mimeLbl;

		QWidget *stretch;

		#ifdef USE_MEDIAINFO
			NBMediaInfoWidget *infoWidget;
		#endif

		void setIcon( QPixmap );
		void setIcon( QIcon );

		void setWidgetProperties();

	public slots:

	protected:
		void paintEvent( QPaintEvent* );

	Q_SIGNALS:
};
