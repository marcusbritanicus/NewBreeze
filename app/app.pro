TEMPLATE = app

TARGET = newbreeze3qt4
greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = newbreeze3
}

QT += network sql
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
	QT += concurrent
}

isEqual( QT_MAJOR_VERSION, 4 ) {
	 LIBS += -L../common/ -lnewbreeze-common4
}

isEqual( QT_MAJOR_VERSION, 5 ) {
	LIBS +=  -L../common/ -lnewbreeze-common
}

LIBS += -larchive -lz

# Common Sources
INCLUDEPATH += ../common/ ../common/include
DEPENDPATH += ../common/ ../common/include

# Source Paths
INCLUDEPATH += . Core Core/ArgParser Core/BookmarkInfo Core/DeleteManager Core/FSWatcher Core/IconProvider Core/Logger Core/ProcessManager Gui Gui/Dialogs
INCLUDEPATH += Gui/Dialogs/AppEditor Gui/Dialogs/Archive Gui/Dialogs/BugCatcher Gui/Dialogs/ConfirmDeleteDialog Gui/Dialogs/Dialog Gui/Dialogs/FileDialog
INCLUDEPATH += Gui/Dialogs/IOErrorsDialog Gui/Dialogs/MessageDialog Gui/Dialogs/NewNodeDialog Gui/Dialogs/PropertiesDialog Gui/Dialogs/RenameDialog
INCLUDEPATH += Gui/Dialogs/RunCmdDialog Gui/Dialogs/SettingsManager Gui/Dialogs/StartupWizard Gui/ModelViews Gui/ModelViews/Models Gui/ModelViews/Models/DirTreeModel
INCLUDEPATH += Gui/ModelViews/Models/ItemViewModel Gui/ModelViews/Models/MimeTypeModel Gui/ModelViews/Models/TrashModel Gui/ModelViews/Views Gui/ModelViews/Views/IconView
INCLUDEPATH += Gui/ModelViews/Views/OpenWithView Gui/ModelViews/Views/TrashView Gui/ModelViews/Views/DirectoryView Gui/NewBreezeUI Gui/NewBreezeUI/Application Gui/Others
INCLUDEPATH += Gui/Others/GuiFunctions Gui/Others/PluginManager Gui/Others/Settings Gui/Widgets Gui/Widgets/AddressBar Gui/Widgets/BreadCrumbsBar Gui/Widgets/Buttons
INCLUDEPATH += Gui/Widgets/ContextMenu Gui/Widgets/CustomActions Gui/Widgets/CustomPeek Gui/Widgets/FolderFlash Gui/Widgets/FolderView Gui/Widgets/GuiWidgets
INCLUDEPATH += Gui/Widgets/InfoBar Gui/Widgets/InfoPanel Gui/Widgets/ProcessManager Gui/Widgets/SideBar Gui/Widgets/SidePanel Gui/Widgets/Terminal Gui/Widgets/Terminal/lib
INCLUDEPATH += Gui/Widgets/TrashManager

DEPENDPATH += . Core Core/ArgParser Core/BookmarkInfo Core/DeleteManager Core/FSWatcher Core/IconProvider Core/Logger Core/ProcessManager Gui Gui/Dialogs
DEPENDPATH += Gui/Dialogs/AppEditor Gui/Dialogs/Archive Gui/Dialogs/BugCatcher Gui/Dialogs/ConfirmDeleteDialog Gui/Dialogs/Dialog Gui/Dialogs/FileDialog
DEPENDPATH += Gui/Dialogs/IOErrorsDialog Gui/Dialogs/MessageDialog Gui/Dialogs/NewNodeDialog Gui/Dialogs/PropertiesDialog Gui/Dialogs/RenameDialog
DEPENDPATH += Gui/Dialogs/RunCmdDialog Gui/Dialogs/SettingsManager Gui/Dialogs/StartupWizard Gui/ModelViews Gui/ModelViews/Models Gui/ModelViews/Models/DirTreeModel
DEPENDPATH += Gui/ModelViews/Models/ItemViewModel Gui/ModelViews/Models/MimeTypeModel Gui/ModelViews/Models/TrashModel Gui/ModelViews/Views Gui/ModelViews/Views/IconView
DEPENDPATH += Gui/ModelViews/Views/OpenWithView Gui/ModelViews/Views/TrashView Gui/ModelViews/Views/DirectoryView Gui/NewBreezeUI Gui/NewBreezeUI/Application Gui/Others
DEPENDPATH += Gui/Others/GuiFunctions Gui/Others/PluginManager Gui/Others/Settings Gui/Widgets Gui/Widgets/AddressBar Gui/Widgets/BreadCrumbsBar Gui/Widgets/Buttons
DEPENDPATH += Gui/Widgets/ContextMenu Gui/Widgets/CustomActions Gui/Widgets/CustomPeek Gui/Widgets/FolderFlash Gui/Widgets/FolderView Gui/Widgets/GuiWidgets
DEPENDPATH += Gui/Widgets/InfoBar Gui/Widgets/InfoPanel Gui/Widgets/ProcessManager Gui/Widgets/SideBar Gui/Widgets/SidePanel Gui/Widgets/Terminal Gui/Widgets/Terminal/lib
DEPENDPATH += Gui/Widgets/TrashManager

# Headers
HEADERS += Core/ArgParser/NBCLParser.hpp
HEADERS += Core/BookmarkInfo/NBBookmarkInfo.hpp
HEADERS += Core/DeleteManager/NBDeleteManager.hpp
HEADERS += Core/FSWatcher/NBFileSystemWatcher.hpp
HEADERS += Core/IconProvider/NBIconManager.hpp
HEADERS += Core/Logger/NBLogger.hpp
HEADERS += Core/ProcessManager/NBAbstractProcess.hpp
HEADERS += Core/ProcessManager/NBDeleteProcess.hpp
HEADERS += Core/ProcessManager/NBIOProcess.hpp
HEADERS += Core/ProcessManager/NBProcessManager.hpp
HEADERS += Global.hpp
HEADERS += Gui/Dialogs/AppEditor/NBAppEditorDialog.hpp
HEADERS += Gui/Dialogs/Archive/NBArchiveDialog.hpp
HEADERS += Gui/Dialogs/BugCatcher/NBBugReporter.hpp
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
HEADERS += Gui/Dialogs/StartupWizard/NBStartupWizard.hpp
HEADERS += Gui/ModelViews/Models/DirTreeModel/NBDirTreeModel.hpp
HEADERS += Gui/ModelViews/Models/DirTreeModel/NBTreeBranch.hpp
HEADERS += Gui/ModelViews/Models/ItemViewModel/NBFileInfoGatherer.hpp
HEADERS += Gui/ModelViews/Models/ItemViewModel/NBItemViewModel.hpp
HEADERS += Gui/ModelViews/Models/ItemViewModel/NBItemViewNode.hpp
HEADERS += Gui/ModelViews/Models/ItemViewModel/NBThumbnailer.hpp
HEADERS += Gui/ModelViews/Models/MimeTypeModel/NBMimeTypeModel.hpp
HEADERS += Gui/ModelViews/Models/MimeTypeModel/NBMimeTypeNode.hpp
HEADERS += Gui/ModelViews/Models/TrashModel/NBTrashModel.hpp
HEADERS += Gui/ModelViews/Models/TrashModel/NBTrashNode.hpp
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
HEADERS += Gui/Widgets/InfoPanel/NBInfoPanel.hpp
HEADERS += Gui/Widgets/ProcessManager/NBProcessManagerUI.hpp
HEADERS += Gui/Widgets/ProcessManager/NBProcessWidget.hpp
HEADERS += Gui/Widgets/SideBar/NBSideBarGroup.hpp
HEADERS += Gui/Widgets/SideBar/NBSideBar.hpp
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
HEADERS += optionparser.hpp

# Sources
SOURCES += Core/ArgParser/NBCLParser.cpp
SOURCES += Core/BookmarkInfo/NBBookmarkInfo.cpp
SOURCES += Core/DeleteManager/NBDeleteManager.cpp
SOURCES += Core/FSWatcher/NBFileSystemWatcher.cpp
SOURCES += Core/IconProvider/NBIconManager.cpp
SOURCES += Core/Logger/NBLogger.cpp
SOURCES += Core/ProcessManager/NBDeleteProcess.cpp
SOURCES += Core/ProcessManager/NBIOProcess.cpp
SOURCES += Core/ProcessManager/NBProcessManager.cpp
SOURCES += Gui/Dialogs/AppEditor/NBAppEditorDialog.cpp
SOURCES += Gui/Dialogs/Archive/NBArchiveDialog.cpp
SOURCES += Gui/Dialogs/BugCatcher/NBBugReporter.cpp
SOURCES += Gui/Dialogs/ConfirmDeleteDialog/NBConfirmDeleteDialog.cpp
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
SOURCES += Gui/Dialogs/StartupWizard/NBStartupWizard.cpp
SOURCES += Gui/ModelViews/Models/DirTreeModel/NBDirTreeModel.cpp
SOURCES += Gui/ModelViews/Models/DirTreeModel/NBTreeBranch.cpp
SOURCES += Gui/ModelViews/Models/ItemViewModel/NBFileInfoGatherer.cpp
SOURCES += Gui/ModelViews/Models/ItemViewModel/NBItemViewModel.cpp
SOURCES += Gui/ModelViews/Models/ItemViewModel/NBItemViewNode.cpp
SOURCES += Gui/ModelViews/Models/ItemViewModel/NBThumbnailer.cpp
SOURCES += Gui/ModelViews/Models/MimeTypeModel/NBMimeTypeModel.cpp
SOURCES += Gui/ModelViews/Models/MimeTypeModel/NBMimeTypeNode.cpp
SOURCES += Gui/ModelViews/Models/TrashModel/NBTrashModel.cpp
SOURCES += Gui/ModelViews/Models/TrashModel/NBTrashNode.cpp
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
SOURCES += Gui/Widgets/InfoPanel/NBInfoPanel.cpp
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

# C99/C++11 Support for Qt5
isEqual( QT_MAJOR_VERSION, 5 ) {
	QMAKE_CFLAGS += -std=c99
	QMAKE_CXXFLAGS += -std=c++11
}

# Enable warnings and threading support
CONFIG += thread silent warn_off build_all

# Disable Debug on Release
# CONFIG(release):DEFINES += QT_NO_DEBUG_OUTPUT

# Build location

BUILD_PREFIX = $$(NB_BUILD_DIR)

isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-qt4
OBJECTS_DIR = $$BUILD_PREFIX/obj-qt4
RCC_DIR		= $$BUILD_PREFIX/qrc-qt4
UI_DIR      = $$BUILD_PREFIX/uic-qt4

greaterThan(QT_MAJOR_VERSION, 4) {
	MOC_DIR 	= $$BUILD_PREFIX/moc-qt5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-qt5
	RCC_DIR		= $$BUILD_PREFIX/qrc-qt5
	UI_DIR      = $$BUILD_PREFIX/uic-qt5
}

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
	icons.files = icons/newbreeze.png

	data.path = $$PREFIX/share/newbreeze/
	data.files = README ../Changelog ../ReleaseNotes

	templates.path =  $$PREFIX/share/newbreeze/templates/
	templates.files = ../data/templates/*

	mime.path = $$PREFIX/share/mime/packages/
	mime.files = ../data/x-encrypted-file.xml
}

DEFINES += "HAVE_POSIX_OPENPT"
