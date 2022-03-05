project(
		'NewBreeze',
	  'c',
		'cpp',
		version: '3.0.0',
		license: 'GPLv3',
		meson_version: '>=0.59.0',
		default_options: [
				'cpp_std=c++17',
				'c_std=c11',
				'warning_level=2',
				'werror=false',
		],
)

add_global_arguments( '-DPROJECT_VERSION=' + meson.project_version(), language : 'cpp' )
add_global_arguments( '-fPIC', language:'cpp' )

if get_option('use_qt_version') == 'qt5'
		Qt = import( 'qt5' )
		QtCore     = dependency( 'Qt5Core' )
    QtGui      = dependency( 'Qt5Gui' )
    QtGuiPriv  = dependency( 'qt5', modules: [ 'Gui'], private_headers: [ 'Gui' ] )
    QtWidgets  = dependency( 'Qt5Widgets' )
    QtNetwork  = dependency( 'Qt5Network' )
    QtDBus     = dependency( 'Qt5DBus' )
    QtConcur   = dependency( 'Qt5Concurrent' )
		QtPrint    = dependency( 'Qt5PrintSupport' )
		QtWebCore  = dependency( 'Qt5WebEngine' )
		QtWebView  = dependency( 'Qt5WebEngineWidgets' )
		QtXml      = dependency( 'Qt5Xml' )
		QSci       = meson.get_compiler( 'cpp' ).find_library( 'qscintilla2_qt5' )
		ArchiveQt  = dependency( 'archiveqt5' )
		TermWidget = dependency( 'qtermwidget5' )
		QDV        = dependency( 'Qt5DocumentView' )

		libname = 'newbreeze-qt5'

elif get_option('use_qt_version') == 'qt6'
		Qt = import( 'qt6' )
		QtCore     = dependency( 'Qt6Core' )
    QtGui      = dependency( 'Qt6Gui' )
    QtGuiPriv  = dependency( 'qt6', private_headers: [ 'Gui' ] )
    QtWidgets  = dependency( 'Qt6Widgets' )
    QtNetwork  = dependency( 'Qt6Network' )
    QtDBus     = dependency( 'Qt6DBus' )
    QtConCur   = dependency( 'Qt6Concurrent' )
		QtPrint    = dependency( 'Qt6PrintSupport' )
		QtWebCore  = dependency( 'Qt6WebEngine' )
		QtWebView  = dependency( 'Qt6WebEngineWidgets' )
		QtXml      = dependency( 'Qt6Xml' )
		QSci       = meson.get_compiler( 'cpp' ).find_library( 'qscintilla2_qt6' )
		ArchiveQt  = dependency( 'archiveqt6' )
		TermWidget = dependency( 'qtermwidget6' )
		QDV        = dependency( 'Qt6DocumentView' )

		libname = 'newbreeze-qt6'

endif

QtDeps    = [ QtCore, QtGui, QtWidgets ]

GlobalIncludes = include_directories( [ 'common', 'common/include', 'common/include/newbreeze' ] )

subdir( 'common' )
subdir( 'app' )
subdir( 'plugins/AVPlugin' )
subdir( 'plugins/Default' )
subdir( 'plugins/MarkDown' )
subdir( 'plugins/PdfPlugin' )
subdir( 'plugins/TextEdit' )
subdir( 'plugins/Thumbnailer' )


# Desktop file
install_data(
		'data/newbreeze.desktop',
		install_dir : get_option( 'datadir' ) / 'applications'
)

# Icons
install_data(
		'icons/newbreeze.png',
		install_dir : get_option( 'datadir' ) / 'icons/hicolor/256x256/apps'
)

# Files
install_data(
		[ 'README.md', 'ChangeLog', 'ReleaseNotes' ],
		install_dir : get_option( 'datadir' ) / 'newbreeze'
)

# Mime
install_data(
		'data/x-encrypted-file.xml',
		install_dir : get_option( 'datadir' ) / 'mime/packages/'
)

# Templates
install_subdir(
		'data/templates',
		install_dir : get_option( 'datadir' ) / 'newbreeze'
)

summary = [
	'',
	'----------------',
	'NewBreeze @0@'.format( meson.project_version() ),
	'----------------',
	''
]
message( '\n'.join( summary ) )
