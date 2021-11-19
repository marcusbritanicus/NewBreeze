project(
	'ThumbnailerPlugin',
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
qt5deps = dependency( 'qt5', modules: ['Core', 'Gui', 'Widgets', 'Network', 'PrintSupport', 'Xml', 'WebEngine', 'WebEngineWidgets'] )

Headers = [
		'NBThumbnailerPlugin.hpp'
]

Sources = [
		'NBThumbnailerPlugin.cpp'
]

mocs = qt5.compile_moc(
 		headers : Headers,
	 	dependencies: qt5deps,
		include_directories: [ '/usr/include' ]
)

djvu = dependency( 'ddjvuapi' )
ffmpeg = dependency( 'libffmpegthumbnailer' )
nb3 = dependency( 'newbreeze-common' )
deps = [ qt5deps, nb3, ffmpeg, djvu ]

defplugins = shared_module(
    'Thumbnailer', [ Sources, mocs ],
    dependencies: deps,
    install: true,
    install_dir: join_paths( get_option( 'libdir' ), 'newbreeze', 'plugins' )
)

summary = [
	'',
	'----------------------------------',
	'NewBreeze Plugin Thumbnailer @0@'.format( meson.project_version() ),
	'----------------------------------',
	''
]
message( '\n'.join( summary ) )
