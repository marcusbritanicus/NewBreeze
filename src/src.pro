TEMPLATE = app
TARGET = newbreeze3

QT += network
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
	QT += concurrent
}

LIBS += -lmagic -larchive -lbz2 -llzma -lz

# Source Paths
INCLUDEPATH += Core Core/AppFile Core/Archive Core/ArgParser Core/BookmarkInfo Core/CoreTools Core/DeleteManager Core/DeviceInfo Core/DeviceInfo/QVolumeInfo
INCLUDEPATH += Core/FSWatcher Core/IconProvider Core/MimeHandler Core/ProcessManager Core/StandardPaths Core/SystemInfo Core/XDG Gui Gui/Dialogs Gui/Dialogs/Archive
INCLUDEPATH += Gui/Dialogs/ConfirmDeleteDialog Gui/Dialogs/Dialog Gui/Dialogs/FileDialog Gui/Dialogs/IOErrorsDialog Gui/Dialogs/MessageDialog Gui/Dialogs/NewNodeDialog
INCLUDEPATH += Gui/Dialogs/PropertiesDialog Gui/Dialogs/RenameDialog Gui/Dialogs/RunCmdDialog Gui/Dialogs/SettingsManager Gui/ModelViews Gui/ModelViews/Models
INCLUDEPATH += Gui/ModelViews/Models/ApplicationsModel Gui/ModelViews/Models/CatalogModel Gui/ModelViews/Models/FileSystemModel Gui/ModelViews/Models/TrashModel
INCLUDEPATH += Gui/ModelViews/Views Gui/ModelViews/Views/AppsView Gui/ModelViews/Views/CatalogView Gui/ModelViews/Views/CombiView Gui/ModelViews/Views/IconView
INCLUDEPATH += Gui/ModelViews/Views/OpenWithView Gui/ModelViews/Views/TrashView Gui/NewBreezeUI Gui/NewBreezeUI/Application Gui/Others Gui/Others/GuiFunctions
INCLUDEPATH += Gui/Others/PluginManager Gui/Others/Settings Gui/Widgets Gui/Widgets/AddressBar Gui/Widgets/BreadCrumbsBar Gui/Widgets/Buttons Gui/Widgets/ContextMenu
INCLUDEPATH += Gui/Widgets/CustomActions Gui/Widgets/CustomPeek Gui/Widgets/FolderFlash Gui/Widgets/FolderView Gui/Widgets/GuiWidgets Gui/Widgets/InfoBar
INCLUDEPATH += Gui/Widgets/ProcessManager Gui/Widgets/SideBar Gui/Widgets/SidePanel Gui/Widgets/Terminal Gui/Widgets/Terminal/lib Gui/Widgets/TrashManager icons

DEPENDPATH += Core Core/AppFile Core/Archive Core/ArgParser Core/BookmarkInfo Core/CoreTools Core/DeleteManager Core/DeviceInfo Core/DeviceInfo/QVolumeInfo
DEPENDPATH += Core/FSWatcher Core/IconProvider Core/MimeHandler Core/ProcessManager Core/StandardPaths Core/SystemInfo Core/XDG Gui Gui/Dialogs Gui/Dialogs/Archive
DEPENDPATH += Gui/Dialogs/ConfirmDeleteDialog Gui/Dialogs/Dialog Gui/Dialogs/FileDialog Gui/Dialogs/IOErrorsDialog Gui/Dialogs/MessageDialog Gui/Dialogs/NewNodeDialog
DEPENDPATH += Gui/Dialogs/PropertiesDialog Gui/Dialogs/RenameDialog Gui/Dialogs/RunCmdDialog Gui/Dialogs/SettingsManager Gui/ModelViews Gui/ModelViews/Models
DEPENDPATH += Gui/ModelViews/Models/ApplicationsModel Gui/ModelViews/Models/CatalogModel Gui/ModelViews/Models/FileSystemModel Gui/ModelViews/Models/TrashModel
DEPENDPATH += Gui/ModelViews/Views Gui/ModelViews/Views/AppsView Gui/ModelViews/Views/CatalogView Gui/ModelViews/Views/CombiView Gui/ModelViews/Views/IconView
DEPENDPATH += Gui/ModelViews/Views/OpenWithView Gui/ModelViews/Views/TrashView Gui/NewBreezeUI Gui/NewBreezeUI/Application Gui/Others Gui/Others/GuiFunctions
DEPENDPATH += Gui/Others/PluginManager Gui/Others/Settings Gui/Widgets Gui/Widgets/AddressBar Gui/Widgets/BreadCrumbsBar Gui/Widgets/Buttons Gui/Widgets/ContextMenu
DEPENDPATH += Gui/Widgets/CustomActions Gui/Widgets/CustomPeek Gui/Widgets/FolderFlash Gui/Widgets/FolderView Gui/Widgets/GuiWidgets Gui/Widgets/InfoBar
DEPENDPATH += Gui/Widgets/ProcessManager Gui/Widgets/SideBar Gui/Widgets/SidePanel Gui/Widgets/Terminal Gui/Widgets/Terminal/lib Gui/Widgets/TrashManager icons

# Headers
HEADERS += Core/AppFile/NBAppEngine.hpp
HEADERS += Core/AppFile/NBAppFile.hpp
HEADERS += Core/Archive/NBArchive.hpp
HEADERS += Core/Archive/NBLibBZip2.hpp
HEADERS += Core/Archive/NBLibGZip.hpp
HEADERS += Core/Archive/NBLibLzma.hpp
HEADERS += Core/Archive/NBLibLzma2.hpp
HEADERS += Core/ArgParser/NBCLParser.hpp
HEADERS += Core/BookmarkInfo/NBBookmarkInfo.hpp
HEADERS += Core/CoreTools/NBTools.hpp
HEADERS += Core/DeleteManager/NBDeleteManager.hpp
HEADERS += Core/DeviceInfo/NBDeviceInfo.hpp
HEADERS += Core/FileIO/NBFileIO.hpp
HEADERS += Core/FSWatcher/NBFileSystemWatcher.hpp
HEADERS += Core/IconProvider/NBIconProvider.hpp
HEADERS += Core/ProcessManager/NBAbstractProcess.hpp
HEADERS += Core/ProcessManager/NBDeleteProcess.hpp
HEADERS += Core/ProcessManager/NBIOProcess.hpp
HEADERS += Core/ProcessManager/NBProcessManager.hpp
HEADERS += Core/SystemInfo/NBSystemInfo.hpp
HEADERS += Core/XDG/NBXdg.hpp
HEADERS += Global.hpp
HEADERS += Gui/Dialogs/Archive/NBArchiveDialog.hpp
HEADERS += Gui/Dialogs/ConfirmDeleteDialog/NBConfirmDeleteDialog.hpp
HEADERS += Gui/Dialogs/Dialog/AboutNB.hpp
HEADERS += Gui/Dialogs/Dialog/NBDialog.hpp
HEADERS += Gui/Dialogs/FileDialog/NBFileDialog.hpp
HEADERS += Gui/Dialogs/IOErrorsDialog/NBErrorsDialog.hpp
HEADERS += Gui/Dialogs/MessageDialog/NBMessageDialog.hpp
HEADERS += Gui/Dialogs/NewNodeDialog/NBNewNodeDialog.hpp
HEADERS += Gui/Dialogs/PropertiesDialog/NBPropertiesDialog.hpp
HEADERS += Gui/Dialogs/RenameDialog/NBRenameDialog.hpp
HEADERS += Gui/Dialogs/RunCmdDialog/NBRunCmdDialog.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSettingsManager.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSGeneralWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSIconsWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSKeyBindingsWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSPluginsWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSSideBar.hpp
HEADERS += Gui/Dialogs/SettingsManager/NBSStyleWidget.hpp
HEADERS += Gui/Dialogs/SettingsManager/qkeysequencewidget.h
HEADERS += Gui/Dialogs/SettingsManager/qkeysequencewidget_p.h
HEADERS += Gui/ModelViews/Models/ApplicationsModel/NBApplicationItem.hpp
HEADERS += Gui/ModelViews/Models/ApplicationsModel/NBApplicationsModel.hpp
HEADERS += Gui/ModelViews/Models/CatalogModel/NBCatalogItem.hpp
HEADERS += Gui/ModelViews/Models/CatalogModel/NBCatalogModel.hpp
HEADERS += Gui/ModelViews/Models/FileSystemModel/NBFileInfoGatherer.hpp
HEADERS += Gui/ModelViews/Models/FileSystemModel/NBFileSystemModel.hpp
HEADERS += Gui/ModelViews/Models/FileSystemModel/NBFileSystemNode.hpp
HEADERS += Gui/ModelViews/Models/TrashModel/NBTrashModel.hpp
HEADERS += Gui/ModelViews/Models/TrashModel/NBTrashNode.hpp
HEADERS += Gui/ModelViews/Views/AppsView/NBAppsDelegate.hpp
HEADERS += Gui/ModelViews/Views/AppsView/NBAppsView.hpp
HEADERS += Gui/ModelViews/Views/CatalogView/NBCatalogDelegate.hpp
HEADERS += Gui/ModelViews/Views/CatalogView/NBCatalogView.hpp
HEADERS += Gui/ModelViews/Views/IconView/NBIconDelegate.hpp
HEADERS += Gui/ModelViews/Views/IconView/NBIconView.hpp
HEADERS += Gui/ModelViews/Views/IconView/NBIconViewRestricted.hpp
HEADERS += Gui/ModelViews/Views/OpenWithView/NBOpenWithDelegate.hpp
HEADERS += Gui/ModelViews/Views/OpenWithView/NBOpenWithView.hpp
HEADERS += Gui/ModelViews/Views/TrashView/NBTrashDelegate.hpp
HEADERS += Gui/ModelViews/Views/TrashView/NBTrashView.hpp
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
HEADERS += Gui/Widgets/ContextMenu/NBContextMenu.hpp
HEADERS += Gui/Widgets/CustomActions/NBCustomActions.hpp
HEADERS += Gui/Widgets/CustomPeek/NBCustomPeek.hpp
HEADERS += Gui/Widgets/FolderFlash/NBFolderFlash.hpp
HEADERS += Gui/Widgets/FolderView/NBFolderView.hpp
HEADERS += Gui/Widgets/GuiWidgets/NBFilterWidget.hpp
HEADERS += Gui/Widgets/GuiWidgets/NBFlashLabel.hpp
HEADERS += Gui/Widgets/GuiWidgets/NBGuiWidgets.hpp
HEADERS += Gui/Widgets/GuiWidgets/NBLabels.hpp
HEADERS += Gui/Widgets/GuiWidgets/NBProgressBar.hpp
HEADERS += Gui/Widgets/InfoBar/NBInfoBar.hpp
HEADERS += Gui/Widgets/ProcessManager/NBProcessManagerUI.hpp
HEADERS += Gui/Widgets/ProcessManager/NBProcessWidget.hpp
HEADERS += Gui/Widgets/SideBar/NBSideBar.hpp
HEADERS += Gui/Widgets/SideBar/NBSideBarGroup.hpp
HEADERS += Gui/Widgets/SideBar/NBSideBarItem.hpp
HEADERS += Gui/Widgets/SidePanel/NBBookmarkView.hpp
HEADERS += Gui/Widgets/SidePanel/NBDeviceView.hpp
HEADERS += Gui/Widgets/SidePanel/NBSidePanel.hpp
HEADERS += Gui/Widgets/SidePanel/NBVfsView.hpp
HEADERS += Gui/Widgets/Terminal/lib/BlockArray.h
HEADERS += Gui/Widgets/Terminal/lib/CharacterColor.h
HEADERS += Gui/Widgets/Terminal/lib/Character.h
HEADERS += Gui/Widgets/Terminal/lib/ColorScheme.h
HEADERS += Gui/Widgets/Terminal/lib/ColorTables.h
HEADERS += Gui/Widgets/Terminal/lib/DefaultTranslatorText.h
HEADERS += Gui/Widgets/Terminal/lib/Emulation.h
HEADERS += Gui/Widgets/Terminal/lib/ExtendedDefaultTranslator.h
HEADERS += Gui/Widgets/Terminal/lib/Filter.h
HEADERS += Gui/Widgets/Terminal/lib/History.h
HEADERS += Gui/Widgets/Terminal/lib/HistorySearch.h
HEADERS += Gui/Widgets/Terminal/lib/KeyboardTranslator.h
HEADERS += Gui/Widgets/Terminal/lib/konsole_wcwidth.h
HEADERS += Gui/Widgets/Terminal/lib/kprocess.h
HEADERS += Gui/Widgets/Terminal/lib/kptydevice.h
HEADERS += Gui/Widgets/Terminal/lib/kpty.h
HEADERS += Gui/Widgets/Terminal/lib/kpty_p.h
HEADERS += Gui/Widgets/Terminal/lib/kptyprocess.h
HEADERS += Gui/Widgets/Terminal/lib/LineFont.h
HEADERS += Gui/Widgets/Terminal/lib/Pty.h
HEADERS += Gui/Widgets/Terminal/lib/qtermwidget.h
HEADERS += Gui/Widgets/Terminal/lib/Screen.h
HEADERS += Gui/Widgets/Terminal/lib/ScreenWindow.h
HEADERS += Gui/Widgets/Terminal/lib/Session.h
HEADERS += Gui/Widgets/Terminal/lib/ShellCommand.h
HEADERS += Gui/Widgets/Terminal/lib/TerminalCharacterDecoder.h
HEADERS += Gui/Widgets/Terminal/lib/TerminalDisplay.h
HEADERS += Gui/Widgets/Terminal/lib/tools.h
HEADERS += Gui/Widgets/Terminal/lib/Vt102Emulation.h
HEADERS += Gui/Widgets/Terminal/NBTerminal.hpp
HEADERS += Gui/Widgets/TrashManager/NBTrashManager.hpp
HEADERS += NBPluginInterface.hpp
HEADERS += NBPreviewInterface.hpp
HEADERS += optionparser.hpp

# Sources
SOURCES += Core/AppFile/NBAppEngine.cpp
SOURCES += Core/AppFile/NBAppFile.cpp
SOURCES += Core/Archive/NBArchive.cpp
SOURCES += Core/Archive/NBLibBZip2.cpp
SOURCES += Core/Archive/NBLibGZip.cpp
SOURCES += Core/Archive/NBLibLzma.cpp
SOURCES += Core/Archive/NBLibLzma2.cpp
SOURCES += Core/ArgParser/NBCLParser.cpp
SOURCES += Core/BookmarkInfo/NBBookmarkInfo.cpp
SOURCES += Core/CoreTools/NBTools.cpp
SOURCES += Core/DeleteManager/NBDeleteManager.cpp
SOURCES += Core/DeviceInfo/NBDeviceInfo.cpp
SOURCES += Core/FileIO/NBFileIO.cpp
SOURCES += Core/FSWatcher/NBFileSystemWatcher.cpp
SOURCES += Core/IconProvider/NBIconProvider.cpp
SOURCES += Core/ProcessManager/NBDeleteProcess.cpp
SOURCES += Core/ProcessManager/NBIOProcess.cpp
SOURCES += Core/ProcessManager/NBProcessManager.cpp
SOURCES += Core/SystemInfo/NBSystemInfo.cpp
SOURCES += Core/XDG/NBXdg.cpp
SOURCES += Gui/Dialogs/Archive/NBArchiveDialog.cpp
SOURCES += Gui/Dialogs/ConfirmDeleteDialog/NBConfirmDeleteDialog.cpp
SOURCES += Gui/Dialogs/ConfirmDeleteDialog/Trial.cpp
SOURCES += Gui/Dialogs/Dialog/AboutNB.cpp
SOURCES += Gui/Dialogs/Dialog/NBDialog.cpp
SOURCES += Gui/Dialogs/FileDialog/NBFileDialog.cpp
SOURCES += Gui/Dialogs/IOErrorsDialog/NBErrorsDialog.cpp
SOURCES += Gui/Dialogs/MessageDialog/NBMessageDialog.cpp
SOURCES += Gui/Dialogs/NewNodeDialog/NBNewNodeDialog.cpp
SOURCES += Gui/Dialogs/PropertiesDialog/NBPropertiesDialog.cpp
SOURCES += Gui/Dialogs/RenameDialog/NBRenameDialog.cpp
SOURCES += Gui/Dialogs/RunCmdDialog/NBRunCmdDialog.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSettingsManager.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSGeneralWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSIconsWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSKeyBindingsWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSPluginsWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSSideBar.cpp
SOURCES += Gui/Dialogs/SettingsManager/NBSStyleWidget.cpp
SOURCES += Gui/Dialogs/SettingsManager/qkeysequencewidget.cpp
SOURCES += Gui/ModelViews/Models/ApplicationsModel/NBApplicationItem.cpp
SOURCES += Gui/ModelViews/Models/ApplicationsModel/NBApplicationsModel.cpp
SOURCES += Gui/ModelViews/Models/CatalogModel/NBCatalogItem.cpp
SOURCES += Gui/ModelViews/Models/CatalogModel/NBCatalogModel.cpp
SOURCES += Gui/ModelViews/Models/FileSystemModel/NBFileInfoGatherer.cpp
SOURCES += Gui/ModelViews/Models/FileSystemModel/NBFileSystemModel.cpp
SOURCES += Gui/ModelViews/Models/FileSystemModel/NBFileSystemNode.cpp
SOURCES += Gui/ModelViews/Models/TrashModel/NBTrashModel.cpp
SOURCES += Gui/ModelViews/Models/TrashModel/NBTrashNode.cpp
SOURCES += Gui/ModelViews/Views/AppsView/NBAppsDelegate.cpp
SOURCES += Gui/ModelViews/Views/AppsView/NBAppsView.cpp
SOURCES += Gui/ModelViews/Views/CatalogView/NBCatalogDelegate.cpp
SOURCES += Gui/ModelViews/Views/CatalogView/NBCatalogView.cpp
SOURCES += Gui/ModelViews/Views/IconView/NBIconDelegate.cpp
SOURCES += Gui/ModelViews/Views/IconView/NBIconView.cpp
SOURCES += Gui/ModelViews/Views/IconView/NBIconViewRestricted.cpp
SOURCES += Gui/ModelViews/Views/OpenWithView/NBOpenWithDelegate.cpp
SOURCES += Gui/ModelViews/Views/OpenWithView/NBOpenWithView.cpp
SOURCES += Gui/ModelViews/Views/TrashView/NBTrashDelegate.cpp
SOURCES += Gui/ModelViews/Views/TrashView/NBTrashView.cpp
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
SOURCES += Gui/Widgets/ContextMenu/NBContextMenu.cpp
SOURCES += Gui/Widgets/CustomActions/NBCustomActions.cpp
SOURCES += Gui/Widgets/CustomPeek/NBCustomPeek.cpp
SOURCES += Gui/Widgets/FolderFlash/NBFolderFlash.cpp
SOURCES += Gui/Widgets/FolderView/NBFolderView.cpp
SOURCES += Gui/Widgets/GuiWidgets/NBFilterWidget.cpp
SOURCES += Gui/Widgets/GuiWidgets/NBFlashLabel.cpp
SOURCES += Gui/Widgets/GuiWidgets/NBGuiWidgets.cpp
SOURCES += Gui/Widgets/GuiWidgets/NBLabels.cpp
SOURCES += Gui/Widgets/GuiWidgets/NBProgressBar.cpp
SOURCES += Gui/Widgets/InfoBar/NBInfoBar.cpp
SOURCES += Gui/Widgets/ProcessManager/NBProcessManagerUI.cpp
SOURCES += Gui/Widgets/ProcessManager/NBProcessWidget.cpp
SOURCES += Gui/Widgets/SideBar/NBSideBar.cpp
SOURCES += Gui/Widgets/SideBar/NBSideBarGroup.cpp
SOURCES += Gui/Widgets/SideBar/NBSideBarItem.cpp
SOURCES += Gui/Widgets/SidePanel/NBBookmarkView.cpp
SOURCES += Gui/Widgets/SidePanel/NBDeviceView.cpp
SOURCES += Gui/Widgets/SidePanel/NBSidePanel.cpp
SOURCES += Gui/Widgets/SidePanel/NBVfsView.cpp
SOURCES += Gui/Widgets/Terminal/lib/BlockArray.cpp
SOURCES += Gui/Widgets/Terminal/lib/ColorScheme.cpp
SOURCES += Gui/Widgets/Terminal/lib/Emulation.cpp
SOURCES += Gui/Widgets/Terminal/lib/Filter.cpp
SOURCES += Gui/Widgets/Terminal/lib/History.cpp
SOURCES += Gui/Widgets/Terminal/lib/HistorySearch.cpp
SOURCES += Gui/Widgets/Terminal/lib/KeyboardTranslator.cpp
SOURCES += Gui/Widgets/Terminal/lib/konsole_wcwidth.cpp
SOURCES += Gui/Widgets/Terminal/lib/kprocess.cpp
SOURCES += Gui/Widgets/Terminal/lib/kpty.cpp
SOURCES += Gui/Widgets/Terminal/lib/kptydevice.cpp
SOURCES += Gui/Widgets/Terminal/lib/kptyprocess.cpp
SOURCES += Gui/Widgets/Terminal/lib/Pty.cpp
SOURCES += Gui/Widgets/Terminal/lib/qtermwidget.cpp
SOURCES += Gui/Widgets/Terminal/lib/Screen.cpp
SOURCES += Gui/Widgets/Terminal/lib/ScreenWindow.cpp
SOURCES += Gui/Widgets/Terminal/lib/Session.cpp
SOURCES += Gui/Widgets/Terminal/lib/ShellCommand.cpp
SOURCES += Gui/Widgets/Terminal/lib/TerminalCharacterDecoder.cpp
SOURCES += Gui/Widgets/Terminal/lib/TerminalDisplay.cpp
SOURCES += Gui/Widgets/Terminal/lib/tools.cpp
SOURCES += Gui/Widgets/Terminal/lib/Vt102Emulation.cpp
SOURCES += Gui/Widgets/Terminal/NBTerminal.cpp
SOURCES += Gui/Widgets/TrashManager/NBTrashManager.cpp
SOURCES += Main.cpp

# Headers and sources for MimeHandler
lessThan(QT_MAJOR_VERSION, 5) {
	HEADERS += Core/MimeHandler/NBMimeDatabase.hpp
	HEADERS += Core/MimeHandler/NBMimeDatabase_p.hpp
	HEADERS += Core/MimeHandler/NBMimeGlobPattern_p.hpp
	HEADERS += Core/MimeHandler/NBMimeMagicRuleMatcher_p.hpp
	HEADERS += Core/MimeHandler/NBMimeMagicRule_p.hpp
	HEADERS += Core/MimeHandler/NBMimeProvider_p.hpp
	HEADERS += Core/MimeHandler/NBMimeType.hpp
	HEADERS += Core/MimeHandler/NBMimeTypeParser_p.hpp
	HEADERS += Core/MimeHandler/NBMimeType_p.hpp
	HEADERS += Core/StandardPaths/NBStandardPaths.hpp

	SOURCES += Core/MimeHandler/NBMimeDatabase.cpp
	SOURCES += Core/MimeHandler/NBMimeGlobPattern.cpp
	SOURCES += Core/MimeHandler/NBMimeMagicRule.cpp
	SOURCES += Core/MimeHandler/NBMimeMagicRuleMatcher.cpp
	SOURCES += Core/MimeHandler/NBMimeProvider.cpp
	SOURCES += Core/MimeHandler/NBMimeType.cpp
	SOURCES += Core/MimeHandler/NBMimeTypeParser.cpp
	SOURCES += Core/StandardPaths/NBStandardPaths.cpp
	SOURCES += Core/StandardPaths/NBStandardPaths_unix.cpp
}

lessThan(QT_MAJOR_VERSION, 5) {

	# QStorageInfo from Qt5 v5.4
	HEADERS += Core/DeviceInfo/QVolumeInfo/QVolumeInfo.hpp
	HEADERS += Core/DeviceInfo/QVolumeInfo/QVolumeInfoP.hpp

	SOURCES += Core/DeviceInfo/QVolumeInfo/QVolumeInfo.cpp
	SOURCES += Core/DeviceInfo/QVolumeInfo/QVolumeInfoUnix.cpp
}

isEqual(QT_MAJOR_VERSION, 5) {

	# QStorageInfo from Qt5 v5.4
	lessThan(QT_MINOR_VERSION, 4) {
		HEADERS += Core/DeviceInfo/QVolumeInfo/QVolumeInfo.hpp
		HEADERS += Core/DeviceInfo/QVolumeInfo/QVolumeInfoP.hpp

		SOURCES += Core/DeviceInfo/QVolumeInfo/QVolumeInfo.cpp
		SOURCES += Core/DeviceInfo/QVolumeInfo/QVolumeInfoUnix.cpp
	}
}

# Icon and stylesheet resources
RESOURCES += NewBreeze.qrc

# C++11 Support
#QMAKE_CFLAGS += -std=c++11
#QMAKE_CXXFLAGS += -std=c++11

# Enable warnings and threading support
CONFIG += thread silent warn_off build_all

# Disable Debug on Release
# CONFIG(release):DEFINES += QT_NO_DEBUG_OUTPUT

# Build location

BUILD_PREFIX = $$(NB_BUILD_DIR)

isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-X
OBJECTS_DIR = $$BUILD_PREFIX/obj-X
RCC_DIR		= $$BUILD_PREFIX/qrc-X
UI_DIR      = $$BUILD_PREFIX/uic-X

greaterThan(QT_MAJOR_VERSION, 4) {
	MOC_DIR 	= $$BUILD_PREFIX/moc-X5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-X5
	RCC_DIR		= $$BUILD_PREFIX/qrc-X5
	UI_DIR      = $$BUILD_PREFIX/uic-X5
}

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}
	BINDIR = $$PREFIX/bin

	INSTALLS += target header desktop icons data mime
	target.path = $$BINDIR

	header.path = $$PREFIX/include/newbreeze
	header.files = NBPreviewInterface.hpp NBPluginInterface.hpp

	desktop.path = $$PREFIX/share/applications/
	desktop.files = Data/newbreeze.desktop

	icons.path = $$PREFIX/share/icons/hicolor/256x256/apps/
	icons.files = icons/newbreeze.png

	data.path = $$PREFIX/share/newbreeze/
	data.files = README Changelog ReleaseNotes

	mime.path = $$PREFIX/share/mime/packages/
	mime.files = Data/x-encrypted-file.xml
}

DEFINES += "HAVE_POSIX_OPENPT"
