TEMPLATE = subdirs

# Directories
SUBDIRS += app common plugins

app.depends = common
