/*
	*
	* NBPermsWidget.hpp - NBPermsWidget.cpp header
	*
*/

#pragma once

#include "Global.hpp"

class NBPermissionsWidget: public QWidget {
    Q_OBJECT

	public:
		NBPermissionsWidget( QStringList, QWidget * );

	private:
		void createGUI();
		void setupConnections();

		void setWidgetProperties();
		void readPermissions();

		QStringList pathsList;

		QLabel *nameLbl;
		QLabel *pathLbl;

		QLabel *uNameLbl, *gNameLbl;

		QCheckBox *urCB, *uwCB, *uxCB;		// User
		QCheckBox *grCB, *gwCB, *gxCB;		// Group
		QCheckBox *orCB, *owCB, *oxCB;		// Other

		QPushButton *recursiveBtn;

	private slots:
		void addUR();
		void addUW();
		void addUX();

		void addGR();
		void addGW();
		void addGX();

		void addOR();
		void addOW();
		void addOX();

		void applyRecursive();
};
