/*
	*
	* NBSideBarPlugin.cpp - An alternative SideBar Plugin for NewBreeze
	*
	* This implementation provides
	*
*/

// Qt4 Headers
#include <QApplication>

// Quartica Headers
#include <QfNavBar.hpp>

int main (int argc, char **argv) {
	QApplication app(argc, argv);

	QfNavBar *navbar = new QfNavBar;

	QfNavBarGroup *group = navbar->addGroup("APPLICATIONS");
	navbar->addItem(group, QPixmap("icons/z_allApps.png"), "All Applications");
	navbar->addItem(group, QPixmap("icons/z_updates.png"), "Updates");
	navbar->addItem(group, QPixmap("icons/z_upToDate.png"), "Up To Date");
	navbar->addItem(group, QPixmap("icons/z_unknown.png"), "Unknown");

	group = navbar->addGroup("BY KIND");
	navbar->addItem(group, QPixmap("icons/z_apps.png"), "Applications");
	navbar->addItem(group, QPixmap("icons/z_plugins.png"), "Plugins");
	navbar->addItem(group, QPixmap("icons/z_widgets.png"), "Widgets");
	navbar->addItem(group, QPixmap("icons/z_prefApp.png"), "Preference Panes");
	navbar->addItem(group, QPixmap("icons/z_systemUpdates.png"), "System Updates");

	group = navbar->addGroup("VENDORS");
	navbar->addItem(group, QPixmap("icons/z_apple.png"), "Apple");

	navbar->show();

	return(app.exec());
}
