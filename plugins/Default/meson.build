Zip = dependency( 'libzip' )
DjVu = dependency( 'ddjvuapi' )
QtDepsDefPlug = [ QtWebCore, QtWebView, QtXml ]

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

mocs = Qt.compile_moc(
 		headers : Headers,
	 	dependencies: [ QtDeps, QtDepsDefPlug ],
		include_directories: [ GlobalIncludes, 'DjvuDisplay', 'ImagePeek', 'OdfOgle', 'WebWatch' ],
)

deps = [ QtDeps, QtDepsDefPlug, NB3Common, DjVu, Zip ]

defplugins = shared_module(
    'DefaultPeekPlugins', [ Sources, mocs ],
		include_directories: [ GlobalIncludes, 'DjvuDisplay', 'ImagePeek', 'OdfOgle', 'WebWatch' ],
    dependencies: deps,
    install: true,
    install_dir: join_paths( get_option( 'libdir' ), 'newbreeze', 'plugins' )
)
