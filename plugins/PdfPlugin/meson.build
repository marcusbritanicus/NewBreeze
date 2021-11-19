project(
	'PdfPlugin',
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
qt5deps = dependency( 'qt5', modules: ['Core', 'Gui', 'Widgets'], private_headers: [ 'Widgets' ] )

Includes = [
		'PdfView',
		'PdfView/Document',
		'PdfView/Pdf',
		'PdfView/View',
]

Headers = [
		'Global.hpp',
		'NBPdfPlugin.hpp',
		'PdfView/NBPdfPeep.hpp',
		'PdfView/Document/Document.hpp',
		'PdfView/Document/Navigation.hpp',
		'PdfView/Document/RenderOptions.hpp',
		'PdfView/Document/Renderer.hpp',
		'PdfView/Document/RendererImpl.hpp',
		'PdfView/Pdf/MuPdfDocument.hpp',
		'PdfView/Pdf/PopplerDocument.hpp',
		'PdfView/View/ViewImpl.hpp',
		'PdfView/View/ViewWidgets.hpp',
		'PdfView/View/View.hpp'
]

Sources = [
		'NBPdfPlugin.cpp',
		'PdfView/NBPdfPeep.cpp',
		'PdfView/Document/Navigation.cpp',
		'PdfView/Document/Renderer.cpp',
		'PdfView/Document/Document.cpp',
		'PdfView/Pdf/MuPdfDocument.cpp',
		'PdfView/Pdf/PopplerDocument.cpp',
		'PdfView/View/ViewWidgets.cpp',
		'PdfView/View/View.cpp',
		'PdfView/View/ViewImpl.cpp',
]

mocs = qt5.compile_moc(
 		headers : Headers,
	 	dependencies: qt5deps,
		include_directories: [ '/usr/include' ]
)

nb3 = dependency( 'newbreeze-common' )
deps = [ qt5deps, nb3 ]

pdfplugin = shared_module(
    'PdfPlugin', [ Sources, mocs ],
		include_directories: [ 'PdfView' ],
    dependencies: deps,
		include_directories: [ '/usr/include', Includes ],
    install: true,
    install_dir: join_paths( get_option( 'libdir' ), 'newbreeze', 'plugins' )
)

summary = [
	'',
	'--------------------------',
	'NewBreeze Plugin Pdf @0@'.format( meson.project_version() ),
	'--------------------------',
	''
]
message( '\n'.join( summary ) )
