project(
	'DefaultPeekPlugins',
    'c',
	'cpp',
	version: '3.0.0',
	license: 'MIT',
	meson_version: '>=0.51.0',
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
qt5deps = dependency( 'qt5', modules: ['Core', 'Gui', 'Widgets', 'Network', 'PrintSupport', 'Xml', 'WebEngine', 'WebEngineWidgets'] )

Headers = [
		'DjvuDisplay/NBDjvuDisplay.hpp',
		'Global.hpp',
		'ImagePeek/NBImagePeek.hpp',
		'NBPreviewWidget.hpp',
		'OdfOgle/converter.h',
		'OdfOgle/docmargin.h',
		'OdfOgle/document.h',
		'OdfOgle/formatproperty.h',
		'OdfOgle/kzip.h',
		'OdfOgle/NBOdfOgle.hpp',
		'OdfOgle/odt_reader.h',
		'OdfOgle/styleinformation.h',
		'OdfOgle/styleparser.h',
		'WebWatch/NBWebWatch.hpp'
]

Sources = [
		'DjvuDisplay/NBDjvuDisplay.cpp',
		'ImagePeek/NBImagePeek.cpp',
		'NBPreviewWidget.cpp',
		'OdfOgle/converter.cpp',
		'OdfOgle/docmargin.cpp',
		'OdfOgle/document.cpp',
		'OdfOgle/formatproperty.cpp',
		'OdfOgle/kzip.cpp',
		'OdfOgle/NBOdfOgle.cpp',
		'OdfOgle/odt_reader.cpp',
		'OdfOgle/styleinformation.cpp',
		'OdfOgle/styleparser.cpp',
		'WebWatch/NBWebWatch.cpp'
]

mocs = qt5.compile_moc(
 		headers : Headers,
	 	dependencies: qt5deps
)

djvu = dependency( 'ddjvuapi' )
nb3 = dependency( 'newbreeze-common' )
deps = [ qt5deps, djvu ]

defplugins = shared_module(
    'DefaultPeekPlugins', [ Sources, mocs ],
		include_directories: [ 'DjvuDisplay', 'ImagePeek', 'OdfOgle', 'WebWatch' ],
    dependencies: deps,
    install: true,
    install_dir: join_paths( get_option( 'libdir' ) / 'newbreeze/plugins' )
)

summary = [
	'',
	'----------------------',
	'NewBreeze Common @0@'.format( meson.project_version() ),
	'----------------------',
	''
]
message( '\n'.join( summary ) )
