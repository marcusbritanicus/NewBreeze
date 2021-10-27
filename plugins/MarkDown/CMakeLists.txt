project( MarkDownPreview )
cmake_minimum_required( VERSION 3.1 )

set( PROJECT_VERSION 3.0.0 )
set( PROJECT_VERSION_MAJOR 3 )
set( PROJECT_VERSION_MINOR 0 )
set( PROJECT_VERSION_PATCH 0 )

set( PROJECT_VERSION_MAJOR_MINOR ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )
add_compile_definitions(VERSION="${PROJECT_VERSION}")

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_BUILD_TYPE Release )

add_definitions ( -Wall )
set( CMAKE_INSTALL_PREFIX /usr )

find_package( Qt5Core REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5Widgets REQUIRED )

set( MarkDownPreview_HDRS

)

set( MarkDownPreview_SRCS
	NBMarkDownPreviewWidget.cpp
	cmark/blocks.c
	cmark/buffer.c
	cmark/cmark.c
	cmark/cmark_ctype.c
	cmark/commonmark.c
	cmark/houdini_href_e.c
	cmark/houdini_html_e.c
	cmark/houdini_html_u.c
	cmark/html.c
	cmark/inlines.c
	cmark/iterator.c
	cmark/latex.c
	cmark/man.c
	cmark/node.c
	cmark/references.c
	cmark/render.c
	cmark/scanners.c
	cmark/utf8.c
	cmark/xml.c
)

set( MarkDownPreview_MOCS
	NBMarkDownPreviewWidget.hpp
)
QT5_WRAP_CPP( MOCS ${MarkDownPreview_MOCS} )

add_library( MarkDownPreview MODULE ${MarkDownPreview_SRCS} ${UIS} ${RSCS} ${MOCS} )
target_link_libraries ( MarkDownPreview  Qt5::Core Qt5::Gui Qt5::Widgets newbreeze-common )
