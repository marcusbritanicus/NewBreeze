DEFINES += PDFVIEWLIB_PRINT_PROGRAM=\\\"$${PDFVIEWLIB_PRINT_PROGRAM}\\\"

HEADERS += $${PWD}/utils/bookmarkshandler.h \
	$${PWD}/utils/filesettings.h \
	$${PWD}/utils/globallocale.h \
	$${PWD}/utils/selectaction.h \
	$${PWD}/utils/selectpageaction.h \
	$${PWD}/utils/zoomaction.h \
	$${PWD}/actionhandler.h \
	$${PWD}/pageitem.h \
	$${PWD}/pdfview.h \
	$${PWD}/pdfview_p.h \
	$${PWD}/printhandler.h

SOURCES += $${PWD}/utils/bookmarkshandler.cpp \
	$${PWD}/utils/filesettings.cpp \
	$${PWD}/utils/globallocale.cpp \
	$${PWD}/utils/selectaction.cpp \
	$${PWD}/utils/selectpageaction.cpp \
	$${PWD}/utils/zoomaction.cpp \
	$${PWD}/actionhandler.cpp \
	$${PWD}/pageitem.cpp \
	$${PWD}/pdfview.cpp \
	$${PWD}/printhandler.cpp

