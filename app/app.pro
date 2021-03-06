TEMPLATE = app
TARGET = newbreeze3

lessThan(QT_MAJOR_VERSION, 5) {
	message( "NewBreeze is built for Qt5 and higher. Qt5 is no longer supported." )
}

QT += network dbus
QT += widgets
QT += concurrent

LIBS +=  -L../common/ -lnewbreeze-common
LIBS += -larchive -lbz2 -llzma -lz

CONFIG += link_pkgconfig

contains ( DEFINES, USE_MEDIAINFO ) {
	message( "libmediainfo enabled. Media information will be shown in InfoPanel." )
	DEFINES += UNICODE
	PKGCONFIG += libmediainfo libzen
}

contains ( DEFINES, HAVE_LZLIB ) {
	message( "Builtin LZ compression enabled. lzip binary not required." )
	LIBS += -llz
}

qtermver = $$system($$pkgConfigExecutable() --modversion qtermwidget5)
!isEmpty( qtermver ) {
	message( "Using QTermWidget5 (version $$qtermver) for NBTerminal" )
	PKGCONFIG += qtermwidget5
} else {
	message( "QTermWidget5 was not found. NewBreeze will not feature an inbuilt terminal" )
	DEFINES += NO_QTERMWIDGET
}

# Common Sources
INCLUDEPATH += ../common/ ../common/include
DEPENDPATH += ../common/ ../common/include

# Source Paths
INCLUDEPATH += . Core/ArgParser Core/AutoMount Core/BookmarkInfo Core/DeleteManager Core/FSWatcher Core/IconProvider Core/Logger Core/Other Core/ProcessManager
INCLUDEPATH += Gui/Dialogs Gui/Dialogs/AppEditor Gui/Dialogs/Archive Gui/Dialogs/BookmarkEditor Gui/Dialogs/BugReporter Gui/Dialogs/ConfirmDeleteDialog Gui/Dialogs/Dialog
INCLUDEPATH += Gui/Dialogs/FileDialog Gui/Dialogs/IOErrorsDialog Gui/Dialogs/MessageDialog Gui/Dialogs/NewNodeDialog Gui/Dialogs/PropertiesDialog Gui/Dialogs/RenameDialog
INCLUDEPATH += Gui/Dialogs/RunCmdDialog Gui/Dialogs/SettingsManager Gui/Dialogs/StartupWizard Gui/ModelViews Gui/ModelViews/Models Gui/ModelViews/Models/ArchiveModel
INCLUDEPATH += Gui/ModelViews/Models/DirTreeModel Gui/ModelViews/Models/ItemViewModel Gui/ModelViews/Models/ItemViewModel/epeg Gui/ModelViews/Models/MimeTypeModel
INCLUDEPATH += Gui/ModelViews/Models/TrashModel Gui/ModelViews/Views Gui/ModelViews/Views/DirectoryView Gui/ModelViews/Views/IconView Gui/ModelViews/Views/OpenWithView
INCLUDEPATH += Gui/ModelViews/Views/TrashView Gui/NewBreezeUI Gui/NewBreezeUI/Application Gui/Others Gui/Others/GuiFunctions Gui/Others/PluginManager Gui/Others/Settings
INCLUDEPATH += Gui/Widgets Gui/Widgets/AddressBar Gui/Widgets/BreadCrumbsBar Gui/Widgets/build Gui/Widgets/build/moc Gui/Widgets/build/objs Gui/Widgets/Buttons
INCLUDEPATH += Gui/Widgets/ContextMenu Gui/Widgets/CustomActions Gui/Widgets/FolderView Gui/Widgets/GuiWidgets Gui/Widgets/InfoBar Gui/Widgets/InfoPanel
INCLUDEPATH += Gui/Widgets/MediaInfo Gui/Widgets/ProcessManager Gui/Widgets/SideBar Gui/Widgets/SidePanel Gui/Widgets/SystemMenu Gui/Widgets/Terminal
INCLUDEPATH += Gui/Widgets/TrashManager

DEPENDPATH += . Core/ArgParser Core/AutoMount Core/BookmarkInfo Core/DeleteManager Core/FSWatcher Core/IconProvider Core/Logger Core/Other Core/ProcessManager
DEPENDPATH += Gui/Dialogs Gui/Dialogs/AppEditor Gui/Dialogs/Archive Gui/Dialogs/BookmarkEditor Gui/Dialogs/BugReporter Gui/Dialogs/ConfirmDeleteDialog Gui/Dialogs/Dialog
DEPENDPATH += Gui/Dialogs/FileDialog Gui/Dialogs/IOErrorsDialog Gui/Dialogs/MessageDialog Gui/Dialogs/NewNodeDialog Gui/Dialogs/PropertiesDialog Gui/Dialogs/RenameDialog
DEPENDPATH += Gui/Dialogs/RunCmdDialog Gui/Dialogs/SettingsManager Gui/Dialogs/StartupWizard Gui/ModelViews Gui/ModelViews/Models Gui/ModelViews/Models/ArchiveModel
DEPENDPATH += Gui/ModelViews/Models/DirTreeModel Gui/ModelViews/Models/ItemViewModel Gui/ModelViews/Models/ItemViewModel/epeg Gui/ModelViews/Models/MimeTypeModel
DEPENDPATH += Gui/ModelViews/Models/TrashModel Gui/ModelViews/Views Gui/ModelViews/Views/DirectoryView Gui/ModelViews/Views/IconView Gui/ModelViews/Views/OpenWithView
DEPENDPATH += Gui/ModelViews/Views/TrashView Gui/NewBreezeUI Gui/NewBreezeUI/Application Gui/Others Gui/Others/GuiFunctions Gui/Others/PluginManager Gui/Others/Settings
DEPENDPATH += Gui/Widgets Gui/Widgets/AddressBar Gui/Widgets/BreadCrumbsBar Gui/Widgets/build Gui/Widgets/build/moc Gui/Widgets/build/objs Gui/Widgets/Buttons
DEPENDPATH += Gui/Widgets/ContextMenu Gui/Widgets/CustomActions Gui/Widgets/FolderView Gui/Widgets/GuiWidgets Gui/Widgets/InfoBar Gui/Widgets/InfoPanel
DEPENDPATH += Gui/Widgets/MediaInfo Gui/Widgets/ProcessManager Gui/Widgets/SideBar Gui/Widgets/SidePanel Gui/Widgets/SystemMenu Gui/Widgets/Terminal
DEPENDPATH += Gui/Widgets/TrashManager

# Headers
HEADERS += Core/ArgParser/NBCLParser.hpp
HEADERS += Core/AutoMount/NBAutoMount.hpp
HEADERS += Core/AutoMount/udisks2.h
HEADERS += Core/BookmarkInfo/NBBookmarkInfo.hpp
HEADERS += Core/DeleteManager/NBDeleteManager.hpp
HEADERS += Core/FSWatcher/NBFileSystemWatcher.hpp
HEADERS += Core/IconProvider/NBIconManager.hpp
HEADERS += Core/Logger/NBLogger.hpp
HEADERS += Core/Other/NBFunctions.hpp
HEADERS += Core/ProcessManager/NBAbstractProcess.hpp
HEADERS += Core/ProcessManager/NBDeleteProcess.hpp
HEADERS += Core/ProcessManager/NBIOProcess.hpp
HEADERS += Core/ProcessManager/NBProcessManager.hpp
HEADERS += Global.hpp
HEADERS += Gui/Dialogs/AppEditor/NBAppEditorDialog.hpp
HEADERS += Gui/Dialogs/Archive/NBArchiveDialog.hpp
HEADERS += Gui/Dialogs/BugReporter/NBBugReporter.hpp
HEADERS += Gui/Dialogs/ConfirmDeleteDialog/NBConfirmDeleteDialog.hpp
HEADERS += Gui/Dialogs/Dialog/AboutNB.hpp
HEADERS += Gui/Dialogs/Dialog/NBDialog.hpp
HEADERS += Gui/Dialogs/FileDialog/NBFileDialog.hpp
HEADERS += Gui/Dialogs/IOErrorsDialog/NBErrorsDialog.hpp
HEADERS += Gui/Dialogs/MessageDialog/NBMessageDialog.hpp
HEADERS += Gui/Dialogs/NewNodeDialog/NBNewNodeDialog.hpp
HEADERS += Gui/Dialogs/PropertiesDialog/NBDefaultsWidget.hpp
HEADERS += Gui/Dialogs/PropertiesDialog/NBDetailsWidget.hpp
HEADERS += Gui/Dialogs/PropertiesDialog/NBPropertiesDialog.hpp
HEADERS += Gui/Dialogs/PropertiesDialog/NBPropsWidget.hpp
HEADERS += Gui/Dialogs/PropertiesDialog/NBPermsWidget.hpp
HEADERS += Gui/Dialogs/PropertiesDialog/NBPreviewWidget.hpp
HEADERS += Gui/Dialogs/PropertiesDialog/NBPSideBar.hpp
HEADERS += Gui/Dialogs/RenameDialog/NBRenameDialog.hpp
HEADERS += Gui/Dialogs/RunCmdDialog/NBRunCmdDialog.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSettingsManager.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSGeneralWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSIconsWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSKeyBindingsWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSPluginsWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSSideBar.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSStyleWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSTerminalWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/qkeysequencewidget.h
HEADERS += Gui/Dialogs/SettingsManager/qkeysequencewidget_p.h
HEADERS += Gui/Dialogs/StartupWizard/NBStartupWizard.hpp
HEADERS += Gui/ModelViews/Models/ArchiveModel/NBArchiveTreeModel.hpp
HEADERS += Gui/ModelViews/Models/ArchiveModel/NBTreeBranch.hpp
HEADERS += Gui/ModelViews/Models/ItemViewModel/NBFileInfoGatherer.hpp
HEADERS += Gui/ModelViews/Models/ItemViewModel/NBItemViewModel.hpp
HEADERS += Gui/ModelViews/Models/ItemViewModel/NBItemViewNode.hpp
HEADERS += Gui/ModelViews/Models/ItemViewModel/NBThumbnailer.hpp
HEADERS += Gui/ModelViews/Models/MimeTypeModel/NBMimeTypeModel.hpp
HEADERS += Gui/ModelViews/Models/MimeTypeModel/NBMimeTypeNode.hpp
HEADERS += Gui/ModelViews/Models/TrashModel/NBTrashModel.hpp
HEADERS += Gui/ModelViews/Views/IconView/NBIconDelegate.hpp
HEADERS += Gui/ModelViews/Views/IconView/NBIconView.hpp
HEADERS += Gui/ModelViews/Views/IconView/NBIconViewRestricted.hpp
HEADERS += Gui/ModelViews/Views/OpenWithView/NBOpenWithDelegate.hpp
HEADERS += Gui/ModelViews/Views/OpenWithView/NBOpenWithView.hpp
HEADERS += Gui/ModelViews/Views/TrashView/NBTrashDelegate.hpp
HEADERS += Gui/ModelViews/Views/TrashView/NBTrashView.hpp
HEADERS += Gui/ModelViews/Views/DirectoryView/NBDirectoryView.hpp
HEADERS += Gui/NewBreezeUI/Application/NBApplication.hpp
HEADERS += Gui/NewBreezeUI/Application/qtlocalpeer.hpp
HEADERS += Gui/NewBreezeUI/Application/qtlockedfile.hpp
HEADERS += Gui/NewBreezeUI/NewBreeze.hpp
HEADERS += Gui/NewBreezeUI/NewBreezeTray.hpp
HEADERS += Gui/Others/GuiFunctions/NBGuiFunctions.hpp
HEADERS += Gui/Others/PluginManager/NBPluginManager.hpp
HEADERS += Gui/Others/Settings/NBSettings.hpp
HEADERS += Gui/Widgets/AddressBar/NBAddressBar.hpp
HEADERS += Gui/Widgets/BreadCrumbsBar/FlowLayout.hpp
HEADERS += Gui/Widgets/BreadCrumbsBar/NBCrumb.hpp
HEADERS += Gui/Widgets/BreadCrumbsBar/NBCrumbsBar.hpp
HEADERS += Gui/Widgets/BreadCrumbsBar/NBExtraCrumb.hpp
HEADERS += Gui/Widgets/Buttons/NBButton.hpp
HEADERS += Gui/Widgets/Buttons/NBButtons.hpp
HEADERS += Gui/Widgets/Buttons/NBPushButton.hpp
HEADERS += Gui/Widgets/ContextMenu/NBContextMenu.hpp
HEADERS += Gui/Widgets/ContextMenu/NBCategoryMenu.hpp
HEADERS += Gui/Widgets/CustomActions/NBCustomActions.hpp
HEADERS += Gui/Widgets/FolderView/NBFolderView.hpp
HEADERS += Gui/Widgets/GuiWidgets/NBFilterWidget.hpp
HEADERS += Gui/Widgets/GuiWidgets/NBGuiWidgets.hpp
HEADERS += Gui/Widgets/GuiWidgets/NBLabels.hpp
HEADERS += Gui/Widgets/GuiWidgets/NBProgressBar.hpp
HEADERS += Gui/Widgets/InfoBar/NBInfoBar.hpp
HEADERS += Gui/Widgets/InfoPanel/NBInfoPanel.hpp
HEADERS += Gui/Widgets/MediaInfo/NBMediaInfo.hpp
HEADERS += Gui/Widgets/ProcessManager/NBProcessManagerUI.hpp
HEADERS += Gui/Widgets/ProcessManager/NBProcessWidget.hpp
HEADERS += Gui/Widgets/SideBar/NBSideBar.hpp
HEADERS += Gui/Widgets/SideBar/NBSideBarEntry.hpp
HEADERS += Gui/Widgets/SideBar/NBSideBarMenu.hpp
HEADERS += Gui/Widgets/SidePanel/NBSidePanel.hpp
HEADERS += Gui/Widgets/SidePanel/NBSidePanelItem.hpp
HEADERS += Gui/Widgets/SystemMenu/NBMenuButton.hpp
HEADERS += Gui/Widgets/SystemMenu/NBSystemMenu.hpp
HEADERS += Gui/Widgets/Terminal/NBTerminal.hpp
HEADERS += Gui/Widgets/Terminal/NBTermWidget.hpp
HEADERS += Gui/Widgets/Terminal/NBTSettingsDialog.hpp
HEADERS += Gui/Widgets/Terminal/TtyTabWidget.hpp
HEADERS += Gui/Widgets/TrashManager/NBTrashManager.hpp
HEADERS += optionparser.hpp

# Sources
SOURCES += Core/ArgParser/NBCLParser.cpp
SOURCES += Core/AutoMount/NBAutoMount.cpp
SOURCES += Core/AutoMount/udisks2.cpp
SOURCES += Core/BookmarkInfo/NBBookmarkInfo.cpp
SOURCES += Core/DeleteManager/NBDeleteManager.cpp
SOURCES += Core/FSWatcher/NBFileSystemWatcher.cpp
SOURCES += Core/IconProvider/NBIconManager.cpp
SOURCES += Core/Logger/NBLogger.cpp
SOURCES += Core/Other/NBFunctions.cpp
SOURCES += Core/ProcessManager/NBDeleteProcess.cpp
SOURCES += Core/ProcessManager/NBIOProcess.cpp
SOURCES += Core/ProcessManager/NBProcessManager.cpp
SOURCES += Gui/Dialogs/AppEditor/NBAppEditorDialog.cpp
SOURCES += Gui/Dialogs/Archive/NBArchiveDialog.cpp
SOURCES += Gui/Dialogs/BugReporter/NBBugReporter.cpp
SOURCES += Gui/Dialogs/ConfirmDeleteDialog/NBConfirmDeleteDialog.cpp
SOURCES += Gui/Dialogs/Dialog/AboutNB.cpp
SOURCES += Gui/Dialogs/Dialog/NBDialog.cpp
SOURCES += Gui/Dialogs/FileDialog/NBFileDialog.cpp
SOURCES += Gui/Dialogs/IOErrorsDialog/NBErrorsDialog.cpp
SOURCES += Gui/Dialogs/MessageDialog/NBMessageDialog.cpp
SOURCES += Gui/Dialogs/NewNodeDialog/NBNewNodeDialog.cpp
SOURCES += Gui/Dialogs/PropertiesDialog/NBDefaultsWidget.cpp
SOURCES += Gui/Dialogs/PropertiesDialog/NBDetailsWidget.cpp
SOURCES += Gui/Dialogs/PropertiesDialog/NBPropertiesDialog.cpp
SOURCES += Gui/Dialogs/PropertiesDialog/NBPropsWidget.cpp
SOURCES += Gui/Dialogs/PropertiesDialog/NBPermsWidget.cpp
SOURCES += Gui/Dialogs/PropertiesDialog/NBPreviewWidget.cpp
SOURCES += Gui/Dialogs/PropertiesDialog/NBPSideBar.cpp
SOURCES += Gui/Dialogs/RenameDialog/NBRenameDialog.cpp
SOURCES += Gui/Dialogs/RunCmdDialog/NBRunCmdDialog.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSettingsManager.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSGeneralWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSIconsWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSKeyBindingsWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSPluginsWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSSideBar.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSStyleWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSTerminalWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/qkeysequencewidget.cpp
SOURCES += Gui/Dialogs/StartupWizard/NBStartupWizard.cpp
SOURCES += Gui/ModelViews/Models/ArchiveModel/NBArchiveTreeModel.cpp
SOURCES += Gui/ModelViews/Models/ArchiveModel/NBTreeBranch.cpp
#SOURCES += Gui/ModelViews/Models/ItemViewModel/epeg/epeg_main.c
#SOURCES += Gui/ModelViews/Models/ItemViewModel/epeg/saveJpegThumb.cpp
SOURCES += Gui/ModelViews/Models/ItemViewModel/NBFileInfoGatherer.cpp
SOURCES += Gui/ModelViews/Models/ItemViewModel/NBItemViewModel.cpp
SOURCES += Gui/ModelViews/Models/ItemViewModel/NBItemViewNode.cpp
SOURCES += Gui/ModelViews/Models/ItemViewModel/NBThumbnailer.cpp
SOURCES += Gui/ModelViews/Models/MimeTypeModel/NBMimeTypeModel.cpp
SOURCES += Gui/ModelViews/Models/MimeTypeModel/NBMimeTypeNode.cpp
SOURCES += Gui/ModelViews/Models/TrashModel/NBTrashModel.cpp
SOURCES += Gui/ModelViews/Views/IconView/NBIconDelegate.cpp
SOURCES += Gui/ModelViews/Views/IconView/NBIconView.cpp
SOURCES += Gui/ModelViews/Views/IconView/NBIconViewRestricted.cpp
SOURCES += Gui/ModelViews/Views/OpenWithView/NBOpenWithDelegate.cpp
SOURCES += Gui/ModelViews/Views/OpenWithView/NBOpenWithView.cpp
SOURCES += Gui/ModelViews/Views/TrashView/NBTrashDelegate.cpp
SOURCES += Gui/ModelViews/Views/TrashView/NBTrashView.cpp
SOURCES += Gui/ModelViews/Views/DirectoryView/NBDirectoryView.cpp
SOURCES += Gui/NewBreezeUI/Application/NBApplication.cpp
SOURCES += Gui/NewBreezeUI/Application/qtlocalpeer.cpp
SOURCES += Gui/NewBreezeUI/Application/qtlockedfile.cpp
SOURCES += Gui/NewBreezeUI/Application/qtlockedfile_unix.cpp
SOURCES += Gui/NewBreezeUI/NewBreeze.cpp
SOURCES += Gui/NewBreezeUI/NewBreezeTray.cpp
SOURCES += Gui/Others/GuiFunctions/NBGuiFunctions.cpp
SOURCES += Gui/Others/NBStartup.cpp
SOURCES += Gui/Others/PluginManager/NBPluginManager.cpp
SOURCES += Gui/Others/Settings/NBSettings.cpp
SOURCES += Gui/Widgets/AddressBar/NBAddressBar.cpp
SOURCES += Gui/Widgets/BreadCrumbsBar/FlowLayout.cpp
SOURCES += Gui/Widgets/BreadCrumbsBar/NBCrumb.cpp
SOURCES += Gui/Widgets/BreadCrumbsBar/NBCrumbsBar.cpp
SOURCES += Gui/Widgets/BreadCrumbsBar/NBExtraCrumb.cpp
SOURCES += Gui/Widgets/Buttons/NBButton.cpp
SOURCES += Gui/Widgets/Buttons/NBButtons.cpp
SOURCES += Gui/Widgets/Buttons/NBPushButton.cpp
SOURCES += Gui/Widgets/ContextMenu/NBContextMenu.cpp
SOURCES += Gui/Widgets/ContextMenu/NBCategoryMenu.cpp
SOURCES += Gui/Widgets/CustomActions/NBCustomActions.cpp
SOURCES += Gui/Widgets/FolderView/NBFolderView.cpp
SOURCES += Gui/Widgets/GuiWidgets/NBFilterWidget.cpp
SOURCES += Gui/Widgets/GuiWidgets/NBGuiWidgets.cpp
SOURCES += Gui/Widgets/GuiWidgets/NBLabels.cpp
SOURCES += Gui/Widgets/GuiWidgets/NBProgressBar.cpp
SOURCES += Gui/Widgets/InfoBar/NBInfoBar.cpp
SOURCES += Gui/Widgets/InfoPanel/NBInfoPanel.cpp
SOURCES += Gui/Widgets/MediaInfo/NBMediaInfo.cpp
SOURCES += Gui/Widgets/ProcessManager/NBProcessManagerUI.cpp
SOURCES += Gui/Widgets/ProcessManager/NBProcessWidget.cpp
SOURCES += Gui/Widgets/SideBar/NBSideBar.cpp
SOURCES += Gui/Widgets/SideBar/NBSideBarEntry.cpp
SOURCES += Gui/Widgets/SideBar/NBSideBarMenu.cpp
SOURCES += Gui/Widgets/SidePanel/NBSidePanel.cpp
SOURCES += Gui/Widgets/SidePanel/NBSidePanelItem.cpp
SOURCES += Gui/Widgets/SystemMenu/NBMenuButton.cpp
SOURCES += Gui/Widgets/SystemMenu/NBSystemMenu.cpp
SOURCES += Gui/Widgets/Terminal/NBTerminal.cpp
SOURCES += Gui/Widgets/Terminal/NBTermWidget.cpp
SOURCES += Gui/Widgets/Terminal/NBTSettingsDialog.cpp
SOURCES += Gui/Widgets/Terminal/TtyTabWidget.cpp
SOURCES += Gui/Widgets/TrashManager/NBTrashManager.cpp
SOURCES += Main.cpp

# Vault Sources
INCLUDEPATH += Vault Vault/EncFS Vault/KeyDialog Vault/Salsa20
DEPENDPATH += Vault Vault/EncFS Vault/KeyDialog Vault/Salsa20

HEADERS += Vault/NBCircle.hpp
HEADERS += Vault/NBVault.hpp
HEADERS += Vault/NBVaultDatabase.hpp
HEADERS += Vault/EncFS/NBEncFS.hpp
HEADERS += Vault/KeyDialog/NBPasswordDialog.hpp
HEADERS += Vault/KeyDialog/NBPasswordEdit.hpp
HEADERS += Vault/KeyDialog/NBPasswordInput.hpp
HEADERS += Vault/KeyDialog/NBPatternPad.hpp
HEADERS += Vault/Salsa20/NBSalsa20.hpp
HEADERS += Vault/Salsa20/Salsa20.hpp

SOURCES += Vault/NBCircle.cpp
SOURCES += Vault/NBVault.cpp
SOURCES += Vault/NBVaultDatabase.cpp
SOURCES += Vault/KeyDialog/NBPasswordDialog.cpp
SOURCES += Vault/KeyDialog/NBPasswordEdit.cpp
SOURCES += Vault/KeyDialog/NBPasswordInput.cpp
SOURCES += Vault/KeyDialog/NBPatternPad.cpp
SOURCES += Vault/EncFS/NBEncFS.cpp
SOURCES += Vault/Salsa20/NBSalsa20.cpp

# Icon and stylesheet resources
RESOURCES += NewBreeze.qrc ../data/data.qrc

# C18/C++17 Support for Qt5
QMAKE_CFLAGS += -std=c18
QMAKE_CXXFLAGS += -std=c++17

# Disable warnings and enable threading support
# Enable maximum optimizations
CONFIG += thread silent build_all optimize_full

# Build location
BUILD_PREFIX = $$(NB_BUILD_DIR)

isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-qt5
OBJECTS_DIR = $$BUILD_PREFIX/obj-qt5
RCC_DIR		= $$BUILD_PREFIX/qrc-qt5
UI_DIR      = $$BUILD_PREFIX/uic-qt5

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}
	BINDIR = $$PREFIX/bin

	INSTALLS += target desktop icons data mime templates
	target.path = $$BINDIR

	QMAKE_RPATHDIR += $$PREFIX/lib/newbreeze/

	desktop.path = $$PREFIX/share/applications/
	desktop.files = ../data/newbreeze.desktop

	icons.path = $$PREFIX/share/icons/hicolor/256x256/apps/
	icons.files = ../icons/newbreeze.png

	data.path = $$PREFIX/share/newbreeze/
	data.files = README ../Changelog ../ReleaseNotes

	templates.path =  $$PREFIX/share/newbreeze/templates/
	templates.files = ../data/templates/*

	mime.path = $$PREFIX/share/mime/packages/
	mime.files = ../data/x-encrypted-file.xml
}

DEFINES += "HAVE_POSIX_OPENPT"
