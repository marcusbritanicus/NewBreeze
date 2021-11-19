project(
	'MarkdownPlugin',
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
qt5deps = dependency( 'qt5', modules: ['Core', 'Gui', 'Widgets'] )

Headers = [
		'NBMarkDownPreviewWidget.hpp'
]

Sources = [
		'NBMarkDownPreviewWidget.cpp',
		'cmark/blocks.c',
		'cmark/buffer.c',
		'cmark/cmark.c',
		'cmark/cmark_ctype.c',
		'cmark/commonmark.c',
		'cmark/houdini_href_e.c',
		'cmark/houdini_html_e.c',
		'cmark/houdini_html_u.c',
		'cmark/html.c',
		'cmark/inlines.c',
		'cmark/iterator.c',
		'cmark/latex.c',
		'cmark/man.c',
		'cmark/node.c',
		'cmark/references.c',
		'cmark/render.c',
		'cmark/scanners.c',
		'cmark/utf8.c',
		'cmark/xml.c'
]

mocs = qt5.compile_moc(
 		headers : Headers,
	 	dependencies: qt5deps,
		include_directories: [ '/usr/include' ]
)

nb3 = dependency( 'newbreeze-common' )
deps = [ qt5deps, nb3 ]

markdown = shared_module(
    'MarkDownPreview', [ Sources, mocs ],
		include_directories: [ 'cmark' ],
    dependencies: deps,
    install: true,
    install_dir: join_paths( get_option( 'libdir' ), 'newbreeze', 'plugins' )
)

summary = [
	'',
	'-------------------------------',
	'NewBreeze MarkDown Plugin @0@'.format( meson.project_version() ),
	'-------------------------------',
	''
]
message( '\n'.join( summary ) )
