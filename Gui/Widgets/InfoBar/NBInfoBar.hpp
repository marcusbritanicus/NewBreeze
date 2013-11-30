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
#include <NBIconProvider.hpp>

class NBInfoBar : public QWidget {
	Q_OBJECT

	public:
		NBInfoBar();

		// FileSystem Mode
		void updateInfoBarCF( QString );
		void updateInfoBarSingle( QString );
		void updateInfoBarSelection( QString, QModelIndexList );

		// Application and Catalog Mode
		void showApplicationInfo( QModelIndex );
		void showCatalogInfo( QModelIndex );

		void clear();

		QLabel *iconLbl;
		NBInfoLabel *infoLbl;
		NBIOManagerMini *ioManagerMini;

	private:
		void setIcon( QPixmap );
		void setIcon( QIcon );

		void setWidgetProperties();
};

#endif
