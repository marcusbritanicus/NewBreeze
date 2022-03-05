Headers = [
		'NBThumbnailerPlugin.hpp'
]

Sources = [
		'NBThumbnailerPlugin.cpp'
]

mocs = Qt.compile_moc(
 		headers : Headers,
	 	dependencies: QtDeps,
		include_directories: [ GlobalIncludes ]
)

DjVu = dependency( 'ddjvuapi' )
FFMpeg = dependency( 'libffmpegthumbnailer' )
deps = [ QtDeps, NB3Common, FFMpeg, DjVu ]

defplugins = shared_module(
    'Thumbnailer', [ Sources, mocs ],
    dependencies: deps,
		include_directories: [ GlobalIncludes ],
    install: true,
    install_dir: join_paths( get_option( 'libdir' ), 'newbreeze', 'plugins' )
)
