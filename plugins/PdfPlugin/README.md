# NBPdfPlugin

All the Pdf capabilities og NewBreeze are rolled into this single plugin.
The PDF reading and rendering is done using the MuPDf library. Because the
stock library provided by various OS is rather bulky (> 30 MiB), we have the
source for a stripped down version of MuPDF (1.13.0) provided here. The source
is roginally taken from Debian Sid repo.

In the stripped down version, majority of the font support is removed
(CJK, Hangul etc), reducing the size. This makes the entire library to a
manageable 5.73 MiB. This library may be updated based on the upstream release,
but it will be done at my discretion or if any one else volunteers to
maintain it.

Since even in the stripped down version, we use all the source of mupdf sans fonts,
we theoretically support all the formats supported by mupdf 1.13.0: PDF, ePub, CBZ,
XPS, SVG etc. Howeversupport for only PDF, ePub and CBZ is enabled as of now. Other
format support will be added later on.
