nb-plugins
==========

The Peek Plugins to be used with newbreeze

This package provides the basic five plugins that can be used with NewBreeze/NewBreeze Native v2.3.0. NewBreeze v2.3.0-preview
and earlier came inbuilt with these plugins. I have separated them to reduce the dependencies of the core program.

### Notes on compilation

- Download the source of `NewBreeze` or `NewBreeze Native, v2.3.0` from `https://github.com/marcusbritanicus/`
- Download the source of nb-plugins from `https://github.com/marcusbritanicus/nb-plugins`
- Open the terminal in thefolder `nb-plugins`
- Type `export NB_INCLUDES=/path/to/the/newbreeze/source`

	For example, if you have downloaded `NB2` in `~/Downloads/`, then type, `export NB_INCLUDES=~/Downloads/NewBreeze`
- Type `qmake && make`
- This will automatically install the plugin in the folder ~/.config/NewBreeze/plugins

### Dependencies
- Qt4
- Qt4-WebKit
- Poppler-Qt4

#### Will provide support for custom plugins very soon
Though you can write yourown plugins using NBPreviewInterface.hpp, I have not enabled deployment of custom plugins. I shall enable this and make
the Interface a bit better by the next release.
