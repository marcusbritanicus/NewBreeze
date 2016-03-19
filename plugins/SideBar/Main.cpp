/*
	*
	* Main.cpp - NBSideBar Test File
	*
*/

#include <QApplication>
#include <QIcon>

#include <NBNavBar.hpp>

int main( int argc, char **argv ) {
	QApplication app( argc, argv );

	NBNavBar *navbar = new NBNavBar;

	NBNavBarItem *mainItem1 = new NBNavBarItem( QIcon::fromTheme( "folder" ).pixmap( 16 ), "Folders", navbar );
	navbar->addItem( mainItem1 );

	NBNavBarItem *mainItem2 = new NBNavBarItem( QIcon::fromTheme( "applications-system" ).pixmap( 16 ), "Applications", navbar );
	navbar->addItem( mainItem2 );

	NBNavBarItem *mainItem3 = new NBNavBarItem( QIcon::fromTheme( "applications-other" ).pixmap( 16 ), "Catalogs", navbar );
	navbar->addItem( mainItem3 );

	NBNavBarGroup *group = navbar->addGroup( "Devices" );
	navbar->addItem( group, QIcon::fromTheme( "drive-harddisk" ).pixmap( 16 ), "Debian" );
	navbar->addItem( group, QIcon::fromTheme( "drive-harddisk" ).pixmap( 16 ), "Users" );
	navbar->addItem( group, QIcon::fromTheme( "drive-harddisk" ).pixmap( 16 ), "Multimedia" );
	navbar->addItem( group, QIcon::fromTheme( "drive-harddisk" ).pixmap( 16 ), "Softwares" );
	navbar->addItem( group, QIcon::fromTheme( "drive-harddisk" ).pixmap( 16 ), "Games" );
	navbar->addItem( group, QIcon::fromTheme( "drive-harddisk" ).pixmap( 16 ), "WinXP" );

	group = navbar->addGroup( "BookMarks" );
	navbar->addItem( group, QIcon::fromTheme( "bookmarks" ).pixmap( 16 ), "Home" );
	navbar->addItem( group, QIcon::fromTheme( "bookmarks" ).pixmap( 16 ), "Documents" );
	navbar->addItem( group, QIcon::fromTheme( "bookmarks" ).pixmap( 16 ), "Downmoads" );
	navbar->addItem( group, QIcon::fromTheme( "bookmarks" ).pixmap( 16 ), "Multimedia" );
	navbar->addItem( group, QIcon::fromTheme( "bookmarks" ).pixmap( 16 ), "Softwares" );

	navbar->show();

	return( app.exec() );
}
