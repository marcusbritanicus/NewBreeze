project( newbreeze-common )
cmake_minimum_required( VERSION 3.1 )

set( PROJECT_VERSION 3.0.0 )
set( PROJECT_VERSION_MAJOR 3 )
set( PROJECT_VERSION_MINOR 0 )
set( PROJECT_VERSION_PATCH 0 )

set( PROJECT_VERSION_MAJOR_MINOR ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_BUILD_TYPE Release )

add_definitions ( -Wall )
set( CMAKE_INSTALL_PREFIX /usr )

find_package( Qt5Core REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5Widgets REQUIRED )

include_directories(
	Archive
	DeviceInfo
	SimpleCrypt
	SystemInfo
	Tools
	XDG
	include
)

set( newbreeze-common_HDRS
	include/common.hpp
	include/NBBZip2.hpp
	include/NBGZip.hpp
	include/NBLZip.hpp
	include/NBLzma2.hpp
	include/NBLzma.hpp
	include/NBSystemInfo.hpp
	include/NBTools.hpp
	include/NBXdg.hpp
	include/newbreeze.hpp
	include/SimpleCrypt.hpp
)

set( newbreeze-common_SRCS
	Archive/lz4dec.c
	Archive/NBArchive.cpp
	Archive/NBBZip2.cpp
	Archive/NBGZip.cpp
	Archive/NBLZip.cpp
	Archive/NBLzma2.cpp
	Archive/NBLzma.cpp
	DeviceInfo/NBDeviceInfo.cpp
	DeviceInfo/NBDeviceInfoUnix.cpp
	SimpleCrypt/SimpleCrypt.cpp
	SystemInfo/NBSystemInfo.cpp
	Tools/NBTools.cpp
	XDG/NBXdg.cpp
)

set( newbreeze-common_MOCS
	include/NBArchive.hpp
	include/NBDeviceInfo.hpp
)
QT5_WRAP_CPP( MOCS ${newbreeze-common_MOCS} )

set ( newbreeze-common_RSCS
	MimeTypes/mimetypes.qrc
)
QT5_ADD_RESOURCES( RSCS ${newbreeze-common_RSCS} )

add_library( newbreeze-common SHARED ${newbreeze-common_SRCS} ${UIS} ${RSCS} ${MOCS} )
target_link_libraries ( newbreeze-common  Qt5::Core Qt5::Gui Qt5::Widgets lzma archive bz2 lz )
