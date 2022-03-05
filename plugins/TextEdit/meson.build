QtDepsTxtPlug = [ QtPrint, QSci ]

PluginIncludes = [
		'Gui/Editor',
		'Gui/Widgets',
		'Gui/Widgets/Buttons',
		'Gui/Widgets/FileProps',
		'Gui/Widgets/Misc',
		'Gui/Widgets/SearchReplace',
		'Gui/UI',
]

Headers = [
		'Global.hpp',
		'Gui/Editor/QsciEditor.hpp',
		'Gui/Editor/QsciLexer.hpp',
		'Gui/Editor/QsciLexerLaTeX.hpp',
		'Gui/Widgets/Buttons/NBWindowButton.hpp',
		'Gui/Widgets/Buttons/ButtonSet.hpp',
		'Gui/Widgets/FileProps/FileProps.hpp',
		'Gui/Widgets/Misc/GuiWidgets.hpp',
		'Gui/Widgets/SearchReplace/SR.hpp',
		'Gui/UI/TextEditor.hpp',
		'NBTextEditPlugin.hpp',
]

Sources = [
		'Gui/Editor/QsciEditor.cpp',
		'Gui/Editor/QsciLexer.cpp',
		'Gui/Editor/QsciLexerLaTeX.cpp',
		'Gui/Widgets/Buttons/NBWindowButton.cpp',
		'Gui/Widgets/Buttons/ButtonSet.cpp',
		'Gui/Widgets/FileProps/FileProps.cpp',
		'Gui/Widgets/Misc/GuiWidgets.cpp',
		'Gui/Widgets/SearchReplace/SR.cpp',
		'Gui/UI/TextEditor.cpp',
		'NBTextEditPlugin.cpp',
]

mocs = Qt.compile_moc(
 		headers : Headers,
	 	dependencies: [ QtDeps, QtPrint ],
		include_directories: [ GlobalIncludes, PluginIncludes ],
)

textplugin = shared_module(
    'TextEditor', [ Sources, mocs ],
    dependencies: [ QtDeps, QtDepsTxtPlug, NB3Common],
		include_directories: [ GlobalIncludes, PluginIncludes ],
    install: true,
    install_dir: join_paths( get_option( 'libdir' ), 'newbreeze', 'plugins' )
)
