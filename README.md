#NewBreeze
## A fast and light-weight file manager based on Qt4

Built on top of the Powerful Qt4 GUI Toolkit, this file manager is sleek, stylish and fast.
Some of its features are
* Fast startup and browsing
* Folder, text, html and image quick preview
* File Association support
* Provides List and Icon View
* Lists the drives/boomarks in the sidebar
* Full drag and drop support
* Bookmark Support
* Auto update devices list
* Custom Folder Icons and Thumbnail Preview
* Complete mime-icon support
* BreadCrumbs Navigation Bar
* Inline terminal

## Windows and Linux Source Available

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
  - http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
  - http://malcer.deviantart.com/art/Caledonia-Icons-264978107

### Dependencies:
* Qt4
* libmagic and libmagic-dev
* zlib
* liblzma
* libtar
* libzip
* libbzip2

### Bugs
* Large paths run away from BreadCrumbs Bar
* Some issues with cut-copy-paste files and folders
* Issues with delete folders


### Upcoming
* File/folder delete GUI
* New functionality to file/folder properties
  - to set open with defaults
* Any other feature you request for... :)
