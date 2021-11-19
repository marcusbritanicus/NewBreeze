project(
	'TextEditPlugin',
    'c',
	'cpp',
	version: '3.0.0',
	license: 'MIT',
	meson_version: '>=0.59.0',
	default_options: [
		'cpp_std=c++17',
        'c_std=c11',
		'warning_level=2',
		'werror=false',
	],
)

add_global_arguments( '-DPROJECT_VERSION=' + meson.project_version(), language : 'cpp' )
add_project_link_arguments( ['-rdynamic','-fPIC'], language:'cpp' )

qt5 = import( 'qt5' )
qt5deps = dependency( 'qt5', modules: ['Core', 'Gui', 'Widgets', 'PrintSupport'] )
qsci = meson.get_compiler( 'cpp' ).find_library( 'qscintilla2_qt5' )

Includes = [
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

mocs = qt5.compile_moc(
 		headers : Headers,
	 	dependencies: qt5deps,
		include_directories: [ '/usr/include' ]
)

nb3 = dependency( 'newbreeze-common' )
deps = [ qt5deps, nb3 ]

textplugin = shared_module(
    'TextEditor', [ Sources, mocs ],
    dependencies: deps,
		include_directories: [ '/usr/include', Includes ],
    install: true,
    install_dir: join_paths( get_option( 'libdir' ), 'newbreeze', 'plugins' )
)

summary = [
	'',
	'---------------------------',
	'NewBreeze Plugin Text @0@'.format( meson.project_version() ),
	'---------------------------',
	''
]
message( '\n'.join( summary ) )
