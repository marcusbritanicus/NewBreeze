TEMPLATE = lib
TARGET = PdfPlugin4

# Same as NewBreeze version
VERSION = "3.0.0"

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}

# Common Locations
# ================
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

# MuPDF locations
# ===============
INCLUDEPATH += mupdf/source/cbz mupdf/source/fitz mupdf/source/gprf mupdf/source/html mupdf/source/pdf mupdf/source/svg mupdf/source/xps mupdf/mujs
INCLUDEPATH += /usr/include/freetype2 /usr/include/harfbuzz
DEPENDPATH += mupdf/source/cbz mupdf/source/fitz mupdf/source/gprf mupdf/source/html mupdf/source/pdf mupdf/source/svg mupdf/source/xps mupdf/mujs

# MuPDF and PdfPeep
# =================
INCLUDEPATH += mupdf/include/ PdfPeep
DEPENDPATH += mupdf/include/ PdfPeep

# cflags and defines
# ==================
QMAKE_CFLAGS += -ffunction-sections -fdata-sections -pipe -O2 -fomit-frame-pointer -fPIC -fno-dce -fno-dse -fno-tree-dce -fno-tree-dse
QMAKE_CFLAGS += $$system( pkg-config --cflags libopenjp2 )					# openJP2
QMAKE_CPPFLAGS += -fPIC -fno-dce -fno-dse -fno-tree-dce -fno-tree-dse
DEFINES += NDEBUG JBIG_NO_MEMENTO TOFU NOCJK SHARE_JPEG NO_ICC HAVE_LIBCRYPTO

# LIBS
# ====
LIBS         += -L/usr/lib -lharfbuzz -lfreetype -lz -ljpeg -lopenjp2 -ljbig2dec

# Poppler
# =======
equals( QT_MAJOR_VERSION, 4 ) {
	INCLUDEPATH  += /usr/include/poppler/qt4
	LIBS += -lpoppler-qt4
}

equals( QT_MAJOR_VERSION, 5 ) {
	INCLUDEPATH  += /usr/include/poppler/qt5
	LIBS += -lpoppler-qt5
}

# Silent and No warning
# =====================
CONFIG += warn_off silent

# Plugin Mode
# ===========
CONFIG += plugin

# MuJS
# ====
HEADERS += mupdf/mujs/astnames.h
HEADERS += mupdf/mujs/jsbuiltin.h
HEADERS += mupdf/mujs/jscompile.h
HEADERS += mupdf/mujs/jsi.h
HEADERS += mupdf/mujs/jslex.h
HEADERS += mupdf/mujs/jsparse.h
HEADERS += mupdf/mujs/jsrun.h
HEADERS += mupdf/mujs/jsvalue.h
HEADERS += mupdf/mujs/mujs.h
HEADERS += mupdf/mujs/opnames.h
HEADERS += mupdf/mujs/regexp.h
HEADERS += mupdf/mujs/utf.h

SOURCES += mupdf/mujs/one.c

# MuPDF
# =====
HEADERS += mupdf/include/mupdf/*.h mupdf/include/mupdf/fitz/*.h mupdf/include/mupdf/helpers/*.h mupdf/include/mupdf/pdf/*.h
HEADERS += mupdf/source/fitz/*.h mupdf/source/html/*.h mupdf/source/pdf/*.h mupdf/source/svg/*.h mupdf/source/xps/*.h

SOURCES += mupdf/source/cbz/mucbz.c mupdf/source/cbz/muimg.c mupdf/source/cbz/mutiff.c

SOURCES += mupdf/source/fitz/archive.c mupdf/source/fitz/bbox-device.c mupdf/source/fitz/bidi.c mupdf/source/fitz/bidi-std.c mupdf/source/fitz/bitmap.c
SOURCES += mupdf/source/fitz/buffer.c mupdf/source/fitz/color-icc-create.c mupdf/source/fitz/color-lcms.c mupdf/source/fitz/colorspace.c mupdf/source/fitz/compress.c
SOURCES += mupdf/source/fitz/compressed-buffer.c mupdf/source/fitz/context.c mupdf/source/fitz/crypt-aes.c mupdf/source/fitz/crypt-arc4.c mupdf/source/fitz/crypt-md5.c
SOURCES += mupdf/source/fitz/crypt-sha2.c mupdf/source/fitz/device.c mupdf/source/fitz/directory.c mupdf/source/fitz/document-all.c mupdf/source/fitz/document.c
SOURCES += mupdf/source/fitz/draw-affine.c mupdf/source/fitz/draw-blend.c mupdf/source/fitz/draw-device.c mupdf/source/fitz/draw-edgebuffer.c
SOURCES += mupdf/source/fitz/draw-edge.c mupdf/source/fitz/draw-glyph.c mupdf/source/fitz/draw-mesh.c mupdf/source/fitz/draw-paint.c mupdf/source/fitz/draw-path.c
SOURCES += mupdf/source/fitz/draw-rasterize.c mupdf/source/fitz/draw-scale-simple.c mupdf/source/fitz/draw-unpack.c mupdf/source/fitz/encode-basic.c
SOURCES += mupdf/source/fitz/error.c mupdf/source/fitz/filter-basic.c mupdf/source/fitz/filter-dct.c mupdf/source/fitz/filter-fax.c mupdf/source/fitz/filter-flate.c
SOURCES += mupdf/source/fitz/filter-jbig2.c mupdf/source/fitz/filter-leech.c mupdf/source/fitz/filter-lzw.c mupdf/source/fitz/filter-predict.c
SOURCES += mupdf/source/fitz/filter-sgi.c mupdf/source/fitz/filter-thunder.c mupdf/source/fitz/font.c mupdf/source/fitz/ftoa.c mupdf/source/fitz/geometry.c
SOURCES += mupdf/source/fitz/getopt.c mupdf/source/fitz/glyph.c mupdf/source/fitz/halftone.c mupdf/source/fitz/harfbuzz.c mupdf/source/fitz/hash.c
SOURCES += mupdf/source/fitz/image.c mupdf/source/fitz/jmemcust.c mupdf/source/fitz/link.c mupdf/source/fitz/list-device.c mupdf/source/fitz/load-bmp.c
SOURCES += mupdf/source/fitz/load-gif.c mupdf/source/fitz/load-jpeg.c mupdf/source/fitz/load-jpx.c mupdf/source/fitz/load-jxr.c mupdf/source/fitz/load-png.c
SOURCES += mupdf/source/fitz/load-pnm.c mupdf/source/fitz/load-tiff.c mupdf/source/fitz/memento.c mupdf/source/fitz/memory.c mupdf/source/fitz/noto.c
SOURCES += mupdf/source/fitz/outline.c mupdf/source/fitz/output.c mupdf/source/fitz/output-cbz.c mupdf/source/fitz/output-pcl.c mupdf/source/fitz/output-pclm.c
SOURCES += mupdf/source/fitz/output-png.c mupdf/source/fitz/output-pnm.c mupdf/source/fitz/output-ps.c mupdf/source/fitz/output-psd.c mupdf/source/fitz/output-pwg.c
SOURCES += mupdf/source/fitz/output-svg.c mupdf/source/fitz/output-tga.c mupdf/source/fitz/path.c mupdf/source/fitz/pixmap.c mupdf/source/fitz/pool.c
SOURCES += mupdf/source/fitz/printf.c mupdf/source/fitz/random.c mupdf/source/fitz/separation.c mupdf/source/fitz/shade.c mupdf/source/fitz/stext-device.c
SOURCES += mupdf/source/fitz/stext-output.c mupdf/source/fitz/stext-search.c mupdf/source/fitz/store.c mupdf/source/fitz/stream-open.c mupdf/source/fitz/stream-prog.c
SOURCES += mupdf/source/fitz/stream-read.c mupdf/source/fitz/string.c mupdf/source/fitz/strtof.c mupdf/source/fitz/svg-device.c mupdf/source/fitz/tempfile.c
SOURCES += mupdf/source/fitz/test-device.c mupdf/source/fitz/text.c mupdf/source/fitz/time.c mupdf/source/fitz/trace-device.c mupdf/source/fitz/track-usage.c
SOURCES += mupdf/source/fitz/transition.c mupdf/source/fitz/tree.c mupdf/source/fitz/ucdn.c mupdf/source/fitz/untar.c mupdf/source/fitz/unzip.c mupdf/source/fitz/util.c
SOURCES += mupdf/source/fitz/writer.c mupdf/source/fitz/xml.c mupdf/source/fitz/zip.c

SOURCES += mupdf/source/generated/CharisSIL-B.c mupdf/source/generated/CharisSIL-BI.c mupdf/source/generated/CharisSIL-I.c mupdf/source/generated/CharisSIL-R.c
SOURCES += mupdf/source/generated/Dingbats.c mupdf/source/generated/icc-profiles.c mupdf/source/generated/NimbusMonoPS-Bold.c
SOURCES += mupdf/source/generated/NimbusMonoPS-BoldItalic.c mupdf/source/generated/NimbusMonoPS-Italic.c mupdf/source/generated/NimbusMonoPS-Regular.c
SOURCES += mupdf/source/generated/NimbusRoman-Bold.c mupdf/source/generated/NimbusRoman-BoldItalic.c mupdf/source/generated/NimbusRoman-Italic.c
SOURCES += mupdf/source/generated/NimbusRoman-Regular.c mupdf/source/generated/NimbusSans-Bold.c mupdf/source/generated/NimbusSans-BoldOblique.c
SOURCES += mupdf/source/generated/NimbusSans-Oblique.c mupdf/source/generated/NimbusSans-Regular.c mupdf/source/generated/pdf-cmap-cjk.c
SOURCES += mupdf/source/generated/pdf-cmap-extra.c mupdf/source/generated/pdf-cmap-utf32.c mupdf/source/generated/pdf-cmap-utf8.c mupdf/source/generated/pdf-js-util.c
SOURCES += mupdf/source/generated/StandardSymbolsPS.c

SOURCES += mupdf/source/gprf/gprf-doc.c mupdf/source/gprf/gprf-skeleton.c
SOURCES += mupdf/source/html/css-apply.c mupdf/source/html/css-parse.c mupdf/source/html/epub-doc.c mupdf/source/html/html-doc.c mupdf/source/html/html-font.c
SOURCES += mupdf/source/html/html-layout.c

SOURCES += mupdf/source/pdf/pdf-annot.c mupdf/source/pdf/pdf-annot-edit.c mupdf/source/pdf/pdf-appearance.c mupdf/source/pdf/pdf-clean.c mupdf/source/pdf/pdf-clean-file.c
SOURCES += mupdf/source/pdf/pdf-cmap.c mupdf/source/pdf/pdf-cmap-load.c mupdf/source/pdf/pdf-cmap-parse.c mupdf/source/pdf/pdf-cmap-table.c
SOURCES += mupdf/source/pdf/pdf-colorspace.c mupdf/source/pdf/pdf-crypt.c mupdf/source/pdf/pdf-device.c mupdf/source/pdf/pdf-encoding.c mupdf/source/pdf/pdf-event.c
SOURCES += mupdf/source/pdf/pdf-field.c mupdf/source/pdf/pdf-font.c mupdf/source/pdf/pdf-form.c mupdf/source/pdf/pdf-function.c mupdf/source/pdf/pdf-graft.c
SOURCES += mupdf/source/pdf/pdf-image.c mupdf/source/pdf/pdf-interpret.c mupdf/source/pdf/pdf-js.c mupdf/source/pdf/pdf-layer.c mupdf/source/pdf/pdf-lex.c
SOURCES += mupdf/source/pdf/pdf-metrics.c mupdf/source/pdf/pdf-nametree.c mupdf/source/pdf/pdf-object.c mupdf/source/pdf/pdf-op-buffer.c mupdf/source/pdf/pdf-op-filter.c
SOURCES += mupdf/source/pdf/pdf-op-run.c mupdf/source/pdf/pdf-outline.c mupdf/source/pdf/pdf-page.c mupdf/source/pdf/pdf-parse.c mupdf/source/pdf/pdf-pattern.c
SOURCES += mupdf/source/pdf/pdf-portfolio.c mupdf/source/pdf/pdf-repair.c mupdf/source/pdf/pdf-resources.c mupdf/source/pdf/pdf-run.c mupdf/source/pdf/pdf-shade.c
SOURCES += mupdf/source/pdf/pdf-signature.c mupdf/source/pdf/pdf-store.c mupdf/source/pdf/pdf-stream.c mupdf/source/pdf/pdf-type3.c mupdf/source/pdf/pdf-unicode.c
SOURCES += mupdf/source/pdf/pdf-write.c mupdf/source/pdf/pdf-xobject.c mupdf/source/pdf/pdf-xref.c

SOURCES += mupdf/source/svg/svg-color.c mupdf/source/svg/svg-doc.c mupdf/source/svg/svg-parse.c mupdf/source/svg/svg-run.c

SOURCES += mupdf/source/xps/xps-common.c mupdf/source/xps/xps-doc.c mupdf/source/xps/xps-glyphs.c mupdf/source/xps/xps-gradient.c mupdf/source/xps/xps-image.c
SOURCES += mupdf/source/xps/xps-link.c mupdf/source/xps/xps-outline.c mupdf/source/xps/xps-path.c mupdf/source/xps/xps-resource.c mupdf/source/xps/xps-tile.c
SOURCES += mupdf/source/xps/xps-util.c mupdf/source/xps/xps-zip.c

# PdfPeep Sources
# ===============
HEADERS += Global.hpp
HEADERS += NBPdfPlugin.hpp
HEADERS += PdfPeep/MuPdfDocument.hpp
HEADERS += PdfPeep/NBPdfPeep.hpp
HEADERS += PdfPeep/PdfDocument.hpp
HEADERS += PdfPeep/PdfView.hpp
HEADERS += PdfPeep/PopplerDocument.hpp

SOURCES += NBPdfPlugin.cpp
SOURCES += PdfPeep/MuPdfDocument.cpp
SOURCES += PdfPeep/NBPdfPeep.cpp
SOURCES += PdfPeep/PdfView.cpp
SOURCES += PdfPeep/PopplerDocument.cpp

# Silent compilation
# ==================
CONFIG += silent

# Build location
# ==============
BUILD_PREFIX = $$(NB_BUILD_DIR)
isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins4/PdfPlugin
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins4/PdfPlugin
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins4/PdfPlugin
UI_DIR      = $$BUILD_PREFIX/uic-plugins4/PdfPlugin

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = PdfPlugin
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5/PdfPlugin
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5/PdfPlugin
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5/PdfPlugin
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5/PdfPlugin
}

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target

	QMAKE_RPATHDIR += $$PREFIX/lib/newbreeze/

	target.path = $$PREFIX/lib/newbreeze/plugins4
	greaterThan(QT_MAJOR_VERSION, 4) {
		target.path = $$PREFIX/lib/newbreeze/plugins
	}
}
