Headers = [
		'NBMarkDownPreviewWidget.hpp'
]

Sources = [
		'NBMarkDownPreviewWidget.cpp',
]

mocs = Qt.compile_moc(
 		headers : Headers,
	 	dependencies: QtDeps,
		include_directories: [ GlobalIncludes ]
)

deps = [ QtDeps, NB3Common ]

markdown = shared_module(
    'MarkDownPreview', [ Sources, mocs ],
    dependencies: deps,
		include_directories: [ GlobalIncludes ],
    install: true,
    install_dir: join_paths( get_option( 'libdir' ), 'newbreeze', 'plugins' )
)
