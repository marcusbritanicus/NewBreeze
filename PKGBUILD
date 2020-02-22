#Maintainer:

pkgname=newbreeze-git
pkgver=r270.d799c4e
pkgrel=1
pkgdesc="A fast and light-weight file manager based on Qt"
arch=('x86_64')
url="https://gitlab.com/marcusbritanicus/NewBreeze"
license=('GPL3')
depends=( 'djvulibre' 'poppler-qt5' 'ffmpegthumbnailer' 'qt5-webengine' 'qt5-base' 'qscintilla-qt5' 'libmediainfo' 'libarchive', 'encfs' )
makedepends=('git' )
provides=('newbreeze')
conflicts=('newbreeze')
source=("${pkgname}::git+https://gitlab.com/marcusbritanicus/NewBreeze.git")
md5sums=('SKIP')

pkgver() {
  cd ${pkgname}
  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

prepare() {
  mkdir -p build
}

build() {
  cd build
  qmake-qt5 ../$pkgname PREFIX=/usr DEFINES+=USE_MEDIAINFO
  make
}

package() {
  cd build
  make INSTALL_ROOT="$pkgdir" install
}
