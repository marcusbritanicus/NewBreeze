TEMPLATE = app
TARGET = DirTreeModel
INCLUDEPATH += .

# Input
HEADERS += NBDirTreeModel.hpp NBTreeBranch.hpp
SOURCES += NBDirTreeModel.cpp NBTreeBranch.cpp

isEqual( QT_MAJOR_VERSION, 4 ) {
	 LIBS += -L/home/cosmos/Softwares/Projects/NewBreeze/NewBreezeX/common/ -lnewbreeze-common
}

isEqual( QT_MAJOR_VERSION, 5 ) {
	 LIBS += -L/home/cosmos/Softwares/Projects/NewBreeze/NewBreezeX/common/ -lnewbreeze-common5
}

# Common Sources
INCLUDEPATH += . /home/cosmos/Softwares/Projects/NewBreeze/NewBreezeX/common/ /home/cosmos/Softwares/Projects/NewBreeze/NewBreezeX/common/include
DEPENDPATH += . /home/cosmos/Softwares/Projects/NewBreeze/NewBreezeX/common/ /home/cosmos/Softwares/Projects/NewBreeze/NewBreezeX/common/include

# RPATH for libnewbreeze-common
QMAKE_RPATHDIR += /usr/lib/newbreeze/

# Enable warnings and threading support
CONFIG += thread silent warn_off build_all
