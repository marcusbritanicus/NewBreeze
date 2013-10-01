/*
	*
	* NBInfoBar.hpp - NewBreeze InfoBar class header
	*
*/

#ifndef NBINFOBAR_HPP
#define NBINFOBAR_HPP

#include <Global.hpp>
#include <NBGuiWidgets.hpp>
#include <NBIOManager.hpp>

class NBInfoBar : public QWidget {
	Q_OBJECT

	public:
		NBInfoBar();
		void updateInfoBarCF( QString );
		void updateInfoBarSingle( QString );
		void updateInfoBarSelection( QString, QModelIndexList );

		void clear();

		QLabel *iconLbl, *nameLbl;
		NBInfoLabel *infoLbl;
		NBIOManager *ioManager;

	private:
		void setIcon( QPixmap );
		void setIcon( QIcon );

		void setWidgetProperties();
};

#endif
