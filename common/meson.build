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
zlib = dependency( 'zlib' )
lzma = dependency( 'liblzma' )
archive = dependency( 'libarchive' )
bz2 = declare_dependency(
    link_args : [ '-L/usr/lib/x86_64-linux-gnu', '-lbz2' ],
    include_directories : include_directories( '/usr/include' )
)
lz = declare_dependency(
    link_args : [ '-L/usr/lib/x86_64-linux-gnu', '-llz' ],
    include_directories : include_directories( '/usr/include' )
)

Headers = [
		'include/common.hpp',
		'include/NBArchive.hpp',
		'include/NBBZip2.hpp',
		'include/NBDeviceInfo.hpp',
		'include/NBGZip.hpp',
		'include/NBLZip.hpp',
		'include/NBLzma2.hpp',
		'include/NBLzma.hpp',
		'include/NBSystemInfo.hpp',
		'include/NBTools.hpp',
		'include/NBXdg.hpp',
		'include/newbreeze.hpp',
		'include/SimpleCrypt.hpp',
		'NBPluginInterface.hpp'
]

Sources = [
		'Archive/lz4dec.c',
		'Archive/NBArchive.cpp',
		'Archive/NBBZip2.cpp',
		'Archive/NBGZip.cpp',
		'Archive/NBLZip.cpp',
		'Archive/NBLzma2.cpp',
		'Archive/NBLzma.cpp',
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

deps = [ qt5deps, zlib, archive, lzma, lz, bz2 ]
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
# pkgconfig = import( 'pkgconfig' )
# pkgconfig.generate(
# 		wlrootsqt,
# 		name: meson.project_name(),
# 		version: meson.project_version(),
# 		filebase: meson.project_name(),
# 		description: 'Qt Wrapper for Wayland and Wlroots protocols',
# )
