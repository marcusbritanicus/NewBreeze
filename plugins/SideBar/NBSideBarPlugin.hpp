/*
 *  This file is part of Quartica.
 *
 *  Copyright (c) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
 *
 *  Quartica is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Quartica is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Quartica.  If not, see <http://www.gnu.org/licenses/>.
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
