TEMPLATE = subdirs

SUBDIRS += app common plugins

app.depends = common
