Headers = [
		'Global.hpp',
		'NBPdfPlugin.hpp',
		'NBPdfPeep.hpp',
]

Sources = [
		'NBPdfPlugin.cpp',
		'NBPdfPeep.cpp',
]

mocs = Qt.compile_moc(
 		headers : Headers,
	 	dependencies: [ QtDeps, QDV ],
		include_directories: [ GlobalIncludes ]
)

pdfplugin = shared_module(
    'PdfPlugin', [ Sources, mocs ],
		include_directories: [ 'PdfView' ],
	 	dependencies: [ QtDeps, QDV, NB3Common ],
		include_directories: [ GlobalIncludes ],
    install: true,
    install_dir: join_paths( get_option( 'libdir' ), 'newbreeze', 'plugins' )
)
