/*
	*
	* Copyright 2012 Britanicus <marcusbritanicus@gmail.com>
	*

	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 2 of the License, or
	* (at your option) any later version.
	*

	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.
	*

	*
	* You should have received a copy of the GNU General Public License
	* along with this program; if not, write to the Free Software
	* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	* MA 02110-1301, USA.
	*
*/

#include "NBTerminal.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

int main( int argc, char **argv ) {

	for( int i = 1; i < argc; i++ ) {
		if ( ( QString( argv[ i ] ) == QString( "--help" ) ) or ( QString( argv[ i ] ) == QString( "-h" ) ) ) {
			std::cout << "NBTerminal v1.0.0\n" << std::endl;
			std::cout << "Usage: NBTerminal [options]" << std::endl;
			std::cout << "Options:" << std::endl;
			std::cout << "  -h|--help               Print this help" << std::endl;
			std::cout << "  -v|--version            Prints application version and exits" << std::endl;
			std::cout << "  -w|--workdir <dir>      Start session with specified work directory" << std::endl;
			std::cout << "  -e|--execute <command>  Execute command in the shell" << std::endl;
			std::cout << "Feature requests, bug reports etc please send to: <marcusbritanicus@gmail.com>\n" << std::endl;

			return 0;
		}

		if ( ( QString( argv[ i ] ) == QString( "--version" ) ) or ( QString( argv[ i ] ) == QString( "-v" ) ) ) {
			std::cout << "NBTerminal v1.0.0" << std::endl;
			std::cout << platform() << std::endl;

			return 0;
		}
	}

	QApplication app( argc, argv );
	QStringList args = qApp->arguments();

	NBTerminal Gui( "/home/cosmos" );
	Gui.showMaximized();

	return app.exec();

	return 0;
};
