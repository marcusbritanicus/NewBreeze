MPV = dependency( 'mpv' )

Headers = [
		'Global.hpp',
		'NBAVPlayer.hpp',
		'NBAVPlugin.hpp',
]

Sources = [
		'NBAVPlayer.cpp',
		'NBAVPlugin.cpp',
]

mocs = Qt.compile_moc(
 		headers : Headers,
	 	dependencies: QtDeps,
		include_directories: [ GlobalIncludes ],
		# include_directories: [ '/usr/include' ]
)

deps = [ QtDeps, NB3Common, MPV ]

avplugin = shared_module(
    'AVPlugin', [ Sources, mocs ],
    dependencies: deps,
		include_directories: [ GlobalIncludes ],
    install: true,
    install_dir: join_paths( get_option( 'libdir' ), 'newbreeze', 'plugins' )
)
