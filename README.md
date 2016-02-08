# NewBreeze
## A fast and light-weight file manager based on Qt

### Qt5 builds are now supported

### Note: NewBreeze 3.0 is in the oven. Expect several changes..!!

Built on top of the Powerful Qt4 GUI Toolkit, this file manager is sleek, stylish and fast.
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

### Notes for compiling - linux and windows:
* Download and Extract the tar file
* Go to NewBreeze
* Please edit the $$PREFIX qmake variable to your choice
* To  compile and install, the command prompt and type
    + `qmake && make && make install`

### Notes for installing icon themes (Windows only):
* Copy the icon theme into the folder $$PREFIX/icons/
* Each theme should be in a separate folder
* Each theme folder must contain index.theme file
* To know how each theme must be structured check
  - [FreeDesktop.Org](http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html)
  - [Caledonia Icons](http://malcer.deviantart.com/art/Caledonia-Icons-264978107)

### Dependencies:
* Qt4
* libmagic and libmagic-dev
* zlib
* liblzma
* libbzip2
* libarchive

### Known Bugs
* Some issues with cut-copy-paste files and folders
* Issues with delete folders

### Upcoming
* Qt5 support
* Unified Folder, Application, and Catalog Viewer
* Application Editor ( Desktop file editor )
* Any other feature you request for... :)
