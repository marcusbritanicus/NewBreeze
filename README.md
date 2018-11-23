# NewBreeze
## A fast and light-weight file manager based on Qt

Chat with me at [![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/newbreeze-fm/)

### Qt5 migration complete. Qt5 is now the preferred Qt version.

### Note: NewBreeze 3.0 is in the oven. Expect several changes..!!

Built on top of the Powerful Qt GUI Toolkit, this file manager is sleek, stylish and fast.
Some of its features are

* Fast startup and browsing
* Folder, text, html, image, odt, and pdf quick preview
* File Association support
* Provides Categorized List, Icon and Details View
* Sleek side panel showing drives and bookmarks
* Full drag and drop support
* Applications, Catalog View and SuperStart
* Auto update devices list
* Custom Folder Icons and Thumbnail Preview
* Complete mime-icon support
* BreadCrumbs Navigation Bar
* Custom Actions and Inline terminal
* SingleApplication Mode for resource sharing
* Plugin System Support

## Linux Source Available
### Windows source is no longer being maintained

### Please READ the ReleaseNotes for information on the new features

### Notes for compiling - linux:

* Download the sources
   - Git: `git clone https://gitlab.com/marcusbritanicus/NewBreeze.git NewBreeze-master`
   - Tar: `wget https://gitlab.com/marcusbritanicus/NewBreeze/-/archive/master/NewBreeze-master.tar.gz && tar -xf NewBreeze-master.tar.gz`
* Enter `NewBreeze-master`
* Open the terminal and type: `qmake && make`
* To install, as root type: `make install`

### Dependencies:
* Qt4/Qt5 (libqt4-dev, libqt4-dev-bin, qtbase5-dev, qtbase5-dev-tools)
    * QtWebKit in case of Qt4 and upto Qt 5.4 (libqtwebkit-dev, libqt5webkit5-dev, qtwebkit5-dev)
    * QtWebEngine Qt 5.5 onwards (qtwebengine5-dev)
    * QScintilla (libqt5scintilla2-dev, libqscintilla2-qt5-dev)
* zlib (zlib1g-dev)
* liblzma (liblzma-dev)
* libbzip2 (libbz2-dev)
* libarchive (libarchive-dev)
* xdg-utils
* libdjvulibre-dev (libdjvulibre-dev)

## My System Info
 - OS:					Debian Sid
 - Qt:					Qt5 5.11.2
 - QScintilla:			2.10.4
 - LibArchive:			3.2.2
 - LibLzma: 			5.2.2
 - LibBz2:				1.0.6
 - zlib:				1.2.11
 - liblz (lzip):		1.10
 - lzop (binary):		1.03
 - libdjvulibre:		3.5.27.1
 - mediainfo:			18.08.1

### Known Bugs
* Reloading causes custom folder icons not to be rendered in some cases
* MuPDf does not render first page (perhaps also other) images in epub documents

### Upcoming
* Any other feature you request for... :)
