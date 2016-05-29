/*
	*
	* NBInfoBar.hpp - NewBreeze InfoBar class header
	*
*/

#pragma once
#ifndef NBINFOBAR_HPP
#define NBINFOBAR_HPP

#include <Global.hpp>
#include <NBGuiWidgets.hpp>
#include <NBIconProvider.hpp>
#include <NBTools.hpp>

class NBInfoBar : public QWidget {
	Q_OBJECT

	public:
		NBInfoBar( QWidget *parnet = NULL );

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

	private:
		void setIcon( QPixmap );
		void setIcon( QIcon );

		void setWidgetProperties();
};

#endif
