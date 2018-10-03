/*
	*
	* NBPropertiesDialog.hpp - NBPropertiesDialog.cpp header
	*
*/

#pragma once

#include "Global.hpp"

#include "NBPSideBar.hpp"
#include "NBDefaultsWidget.hpp"
#include "NBDetailsWidget.hpp"
#include "NBPropsWidget.hpp"
#include "NBPermsWidget.hpp"
#include "NBPreviewWidget.hpp"

class NBPropertiesDialog: public NBDialog {
	Q_OBJECT

	public:
		enum PropertiesTab {
			Properties  = 0x00,
			Permissions = 0x01,
			Defaults    = 0x02,
			Preview     = 0x03,
			Details     = 0x04
		};

		NBPropertiesDialog( QStringList paths, PropertiesTab tab, QWidget *parent = 0 );
		~NBPropertiesDialog();

	private:
		QStackedWidget *stack;

		NBPSideBar *tabs;
		NBPropertiesWidget *propsW;
		NBPermissionsWidget *permsW;
		NBDefaultsWidget *defaultsW;
		NBPreviewWidget *previewW;
		NBDetailsWidget *detailsW;

		QStringList pathsList;

		bool mTerminate;

	public Q_SLOTS:
		void show();
};
