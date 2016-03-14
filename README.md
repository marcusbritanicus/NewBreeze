# NewBreeze

## A fast and light-weight file manager based on Qt

### Qt5 builds are now supported

### Note: NewBreeze 3.0 is in the oven. Expect several changes..!!

Built on top of the Powerful Qt4/Qt5 GUI Toolkit, this file manager is sleek, stylish and fast.
Some of its features are

* Fast startup and browsing
* Folder, text, html, image, odt, and pdf quick preview
* File Association support
* Provides Categorized List, Icon and Details View
* Sleek side panel showing drives and boomarks
* Full drag and drop support (N/A)
* Applications and Catalog View
* Auto update devices list
* Custom Folder Icons and Thumbnail Preview
* Complete mime-icon support
* BreadCrumbs Navigation Bar
* Custom Actions
* Inline terminal
* SingleApplication Mode for resource sharing

## Linux Source Available
### Windows source is no longer being maintained

## Please READ the ReleaseNotes for information on the new features

### Notes for compiling - linux:

* Download the sources
   - Git: `git clone https://github.com/marcusbritanicus/NewBreeze NewBreeze-master`
   - Tar: `wget https://github.com/marcusbritanicus/NewBreeze/archive/master.tar.gz && tar -xf master.tar.gz`
* Enter `NewBreeze-master`
* Open the terminal and type: `qmake && make`
* To install, as root type: `make install`
* To compile the plugins, type: `./BuildPlugins.sh build`
* To install plugins (global), type: `./BuildPlugins.sh install`
* To install plugins (local), type: `./BuildPlugins.sh local`

### Dependencies:
* Qt4/Qt5
* libmagic and libmagic-dev
* zlib
* liblzma
* libbzip2
* libarchive
* libpoppler-qt4/qt5

### Known Bugs
* SegmentationFault in `NBIconView` `::aboveIndex(...)`, `::belowIndex(...)`
* No update of nodes in NBIconView which are being copied.

### Upcoming
* Application Editor ( Desktop file editor )
* Any other feature you request for... :)
