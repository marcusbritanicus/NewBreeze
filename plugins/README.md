# NBPluginInterface

NewBreeze is quite easily extensible using plugins. I am trying to provide

This the general plugin interface. Unlike the PreviewInterface, this plugin
is meant to enhance the working of NewBreeze widgets directly, or provide a
replacement for some widgets. Please note that replacements cannot be
provided for all widgets, NBFolderView, NBAddressBar, NBProcessManager to name
a few.

Note:
This PluginInterface can be used to enhance the NBFolderView class by providing
a suitable context. For the context to be applicable, the plugin must return
on of the FolderView interfaces for interface() and 'Enhancement' for type().
Otherwise 'context()' will be ignored and the plugin may not work.

### Note:
At the present moment, I have not enabled plugin interfaces for all the widgets
and hence your plugins may not work. If you have developed a plugin for some
interface, and if you know how to develop the interface for the widget, please
feel free to contribute them both to NewBreeze via github.

### Note:
I have finished porting the NBPreviewInterface code to NBPluginInterface,
I will be removing it in the next release.

**NBPreviewInterface** is deprecated. Please do not use it to write plugins.
