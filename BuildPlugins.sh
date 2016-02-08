#!/bin/bash

build() {
	# Switch to Plugins folder
	cd Plugins

	# Build Default Plugin Set
	cd Default
	qmake && make
	cd ..

	# Build MarkDown Plugin
	cd MarkDown
	qmake && make
	cd ..

	# Switch to current directory
	cd ..
}

cleanObjs() {
	# Switch to Plugins folder
	cd Plugins

	# Build Default Plugin Set
	cd Default
	make clean
	cd ..

	# Build MarkDown Plugin
	cd MarkDown
	make clean
	cd ..

	# Switch to current directory
	cd ..
}

distclean() {
	# Switch to Plugins folder
	cd Plugins

	# Build Default Plugin Set
	cd Default
	make distclean
	cd ..

	# Build MarkDown Plugin
	cd MarkDown
	make distclean
	cd ..

	# Switch to current directory
	cd ..
}

if [ "$1" == "build" ]; then
	build

elif [ "$1" == "clean" ]; then
	cleanObjs

elif [ "$1" == "distclean" ]; then
	distclean

else
	build

fi
