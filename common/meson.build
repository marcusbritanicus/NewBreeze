CommonHeaders = [
		'include/newbreeze/common.hpp',
		'include/newbreeze/NBDeviceInfo.hpp',
		'include/newbreeze/NBSystemInfo.hpp',
		'include/newbreeze/NBTools.hpp',
		'include/newbreeze/NBXdg.hpp',
		'include/newbreeze/newbreeze.hpp',
		'include/newbreeze/SimpleCrypt.hpp',
		'include/newbreeze/NBPluginInterface.hpp'
]

CommonSources = [
		'DeviceInfo/NBDeviceInfo.cpp',
		'DeviceInfo/NBDeviceInfoUnix.cpp',
		'SimpleCrypt/SimpleCrypt.cpp',
		'SystemInfo/NBSystemInfo.cpp',
		'Tools/NBTools.cpp',
		'XDG/NBXdg.cpp'
]

CommonMocs = Qt.compile_moc(
 		headers : CommonHeaders,
	 	dependencies: QtDeps
)

deps = [ QtDeps, ArchiveQt ]
includes = [ 'include' ]

nb3common = shared_library(
    libname, [ CommonSources, CommonMocs ],
		version: meson.project_version(),
		include_directories: [ includes ],
    dependencies: [ QtDeps, ArchiveQt ],
    install: true,
    install_dir: join_paths( get_option( 'libdir' ) )
)

install_subdir(
		'include/newbreeze',
		install_dir: get_option( 'includedir' ),
		strip_directory : false
)

NB3Common = declare_dependency(
		link_with: nb3common,
		sources: CommonHeaders,
)

## PkgConfig Section
pkgconfig = import( 'pkgconfig' )
pkgconfig.generate(
		nb3common,
		name: libname,
		version: meson.project_version(),
		filebase: libname,
		description: 'Some common classes for NewBreeze'
)
