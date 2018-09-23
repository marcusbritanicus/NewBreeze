#!/bin/sh

## Create the generated folder
mkdir -p source/generated

## Create CMAP files
tools/cmapdump source/generated/pdf-cmap-cjk.c resources/cmaps/cjk/*
tools/cmapdump source/generated/pdf-cmap-extra.c resources/cmaps/extra/*
tools/cmapdump source/generated/pdf-cmap-utf8.c resources/cmaps/utf8/*
tools/cmapdump source/generated/pdf-cmap-utf32.c resources/cmaps/utf32/*

## Create headers files
tools/namedump resources/pdf/names.txt include/mupdf/pdf/name-table.h source/pdf/pdf-name-table.h

## Create font files
tools/hexdump -s source/generated/Dingbats.c resources/fonts/urw/Dingbats.cff
tools/hexdump -s source/generated/NimbusMonoPS-Bold.c resources/fonts/urw/NimbusMonoPS-Bold.cff
tools/hexdump -s source/generated/NimbusMonoPS-BoldItalic.c resources/fonts/urw/NimbusMonoPS-BoldItalic.cff
tools/hexdump -s source/generated/NimbusMonoPS-Italic.c resources/fonts/urw/NimbusMonoPS-Italic.cff
tools/hexdump -s source/generated/NimbusMonoPS-Regular.c resources/fonts/urw/NimbusMonoPS-Regular.cff
tools/hexdump -s source/generated/NimbusRoman-Bold.c resources/fonts/urw/NimbusRoman-Bold.cff
tools/hexdump -s source/generated/NimbusRoman-BoldItalic.c resources/fonts/urw/NimbusRoman-BoldItalic.cff
tools/hexdump -s source/generated/NimbusRoman-Italic.c resources/fonts/urw/NimbusRoman-Italic.cff
tools/hexdump -s source/generated/NimbusRoman-Regular.c resources/fonts/urw/NimbusRoman-Regular.cff
tools/hexdump -s source/generated/NimbusSans-Bold.c resources/fonts/urw/NimbusSans-Bold.cff
tools/hexdump -s source/generated/NimbusSans-BoldOblique.c resources/fonts/urw/NimbusSans-BoldOblique.cff
tools/hexdump -s source/generated/NimbusSans-Oblique.c resources/fonts/urw/NimbusSans-Oblique.cff
tools/hexdump -s source/generated/NimbusSans-Regular.c resources/fonts/urw/NimbusSans-Regular.cff
tools/hexdump -s source/generated/StandardSymbolsPS.c resources/fonts/urw/StandardSymbolsPS.cff
tools/hexdump -s source/generated/CharisSIL-B.c resources/fonts/sil/CharisSIL-B.cff
tools/hexdump -s source/generated/CharisSIL-BI.c resources/fonts/sil/CharisSIL-BI.cff
tools/hexdump -s source/generated/CharisSIL-I.c resources/fonts/sil/CharisSIL-I.cff
tools/hexdump -s source/generated/CharisSIL-R.c resources/fonts/sil/CharisSIL-R.cff
tools/hexdump -0 source/generated/pdf-js-util.c source/pdf/pdf-js-util.js
tools/hexdump source/generated/icc-profiles.c resources/icc/gray.icc resources/icc/rgb.icc resources/icc/cmyk.icc resources/icc/lab.icc
