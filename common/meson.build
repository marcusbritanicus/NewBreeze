project(
		'newbreeze-common',
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
qt5deps = dependency( 'qt5', modules: [ 'Core', 'Gui', 'Widgets' ] )
archiveqt5 = dependency( 'archiveqt5' )

Headers = [
		'include/common.hpp',
		'include/NBDeviceInfo.hpp',
		'include/NBSystemInfo.hpp',
		'include/NBTools.hpp',
		'include/NBXdg.hpp',
		'include/newbreeze.hpp',
		'include/SimpleCrypt.hpp',
		'NBPluginInterface.hpp'
]

Sources = [
		'DeviceInfo/NBDeviceInfo.cpp',
		'DeviceInfo/NBDeviceInfoUnix.cpp',
		'SimpleCrypt/SimpleCrypt.cpp',
		'SystemInfo/NBSystemInfo.cpp',
		'Tools/NBTools.cpp',
		'XDG/NBXdg.cpp'
]

mocs = qt5.compile_moc(
 		headers : Headers,
	 	dependencies: qt5deps
)

deps = [ qt5deps, archiveqt5 ]
includes = [ 'include' ]

nb3common = shared_library(
    'newbreeze-common', [ Sources, mocs ],
		version: meson.project_version(),
		include_directories: [ includes ],
    dependencies: deps,
    install: true,
    install_dir: join_paths( get_option( 'libdir' ) )
)

install_subdir(
		'include',
		install_dir: get_option( 'includedir' ) + '/newbreeze',
		strip_directory : false
)

summary = [
	'',
	'----------------------',
	'NewBreeze Common @0@'.format( meson.project_version() ),
	'----------------------',
	''
]
message( '\n'.join( summary ) )

## PkgConfig Section
pkgconfig = import( 'pkgconfig' )
pkgconfig.generate(
		nb3common,
		name: meson.project_name(),
		version: meson.project_version(),
		filebase: meson.project_name(),
		description: 'Some common classes for NewBreeze'
)
