TEMPLATE = subdirs

SUBDIRS += common app plugins
CONFIG += ordered

app.depends = common
plugins.depends = common
