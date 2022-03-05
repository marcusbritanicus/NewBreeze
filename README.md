# NewBreeze
## A fast and light-weight file manager based on Qt

Chat with me at [![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/newbreeze-fm/)

### Qt5 migration complete. Qt5 is now the preferred Qt version.

### Note: NewBreeze 3.0 is in the oven. Expect several changes..!!

Built on top of the Powerful Qt GUI Toolkit, this file manager is sleek, stylish and fast. It's heavily customized while not compromizing on speed.

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
* Inbuilt folder encryption support using encfs

Its features such as inbuilt file and folder encryption, SuperStart, and Quick file previews, are not found in any other file manager available for linux.

### Please READ the ReleaseNotes for information on the new features

### Notes for compiling (Qt5) - linux:

* Download the sources
  * Git: `git clone https://gitlab.com/marcusbritanicus/NewBreeze.git NewBreeze-master`
  * Tar: `wget https://gitlab.com/marcusbritanicus/NewBreeze/-/archive/master/NewBreeze-master.tar.gz && tar -xf NewBreeze-master.tar.gz`
* Enter `NewBreeze-master`
* Open the terminal and type: `qmake -qt5 DEFINES+=USE_MEDIAINFO DEFINES+=HAVE_LZLIB && make`
  * In case you do not want mediainfo, skip `DEFINES+=USE_MEDIAINFO`
  * In case you do not want/have liblz (for lzip compression), skip `DEFINES+=HAVE_LZLIB`.
  * To use lzip without the headers, install the lzip binary and skip `DEFINES+=HAVE_LZLIB`.
* To install, as root type: `make install`

### Dependencies:
* Qt5 (qtbase5-dev, qtbase5-dev-tools)
  * QtWebKit in case of Qt5 and upto Qt 5.4 (libqtwebkit-dev, libqt5webkit5-dev, qtwebkit5-dev)
  * QtWebEngine Qt 5.5 onwards (qtwebengine5-dev)
  * QScintilla (libqt5scintilla2-dev, libqscintilla2-qt5-dev)
* libarchiveqt (https://gitlab.com/marcusbritanicus/libarchive-qt)
* libdjvulibre-dev (libdjvulibre-dev)
* ffmpegthumbnailer (libffmpegthumbnailer-dev)
* poppler (libpoppler-qt5-dev)
* mpv (libmpv-dev)
* QDocumentView (https://gitlab.com/marcusbritanicus/QDocumentView)

## My System Info
* OS:				Debian Sid
* Qt:				Qt5 5.14.2
* QScintilla:		2.11.2
* LibArchiveQt:		2.0.6
* popper-qt5:       0.85.0-2
* libdjvulibre:		3.5.27.1
* mediainfo:		20.08
* libexif:			0.6.22-2
* libmpv-dev:       0.32.0-2

### Known Bugs
* Reloading causes custom folder icons not to be rendered in some cases
* Loading of folders with large number of nodes freezes the app for a significant time (~5 - 10s)
* Rapid changes in a folder might cause crash in NewBreeze

### Upcoming
* Any other feature you request for... :)
