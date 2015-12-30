#!/bin/bash

sudo rm -rf /tmp/mingw

sudo mkdir -p /opt/cross-tools/i686-w64-mingw32
mkdir -p /tmp/mingw && cd /tmp/mingw
sudo mkdir -p /opt/cross-tools/i686-w64-mingw32 && sudo mkdir -p /opt/cross-tools/x86_64-w64-mingw32

rm -rf SDL2-devel-2.0.3-mingw.tar.gz && wget https://www.libsdl.org/release/SDL2-devel-2.0.3-mingw.tar.gz && tar -xvzf SDL2-devel-2.0.3-mingw.tar.gz && \
sudo cp -R SDL2-2.0.3/i686-w64-mingw32/* /opt/cross-tools/i686-w64-mingw32/ && \
sudo cp -R SDL2-2.0.3/x86_64-w64-mingw32/* /opt/cross-tools/x86_64-w64-mingw32/

rm -rf SDL2_mixer-devel-2.0.0-mingw.tar.gz && wget https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.0.0-mingw.tar.gz && tar -xvzf SDL2_mixer-devel-2.0.0-mingw.tar.gz && \
sudo cp -R SDL2_mixer-2.0.0/i686-w64-mingw32/* /opt/cross-tools/i686-w64-mingw32/ && \
sudo cp -R SDL2_mixer-2.0.0/x86_64-w64-mingw32/* /opt/cross-tools/x86_64-w64-mingw32/

rm -rf SDL2_image-devel-2.0.0-mingw.tar.gz && wget https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.0-mingw.tar.gz && tar -xvzf SDL2_image-devel-2.0.0-mingw.tar.gz && \ 
sudo cp -R SDL2_image-2.0.0/i686-w64-mingw32/* /opt/cross-tools/i686-w64-mingw32/ && \
sudo cp -R SDL2_image-2.0.0/x86_64-w64-mingw32/* /opt/cross-tools/x86_64-w64-mingw32/

# Freetype mingw
wget http://sourceforge.net/projects/freetype/files/freetype2/2.6.2/freetype-2.6.2.tar.bz2/download && mv download freetype-2.6.2.tar.bz2 && tar -xf freetype-2.6.2.tar.bz2
export TARGET=/opt/cross-tools/x86_64-w64-mingw32
export HOST=x86_64-w64-mingw32
export PATH="${TARGET}/gcc/bin:$PATH"
export PKG_CONFIG_PATH="${TARGET}/lib/pkgconfig"

mkdir -p build && cd build && \
../freetype-2.6.2/configure --prefix="${TARGET}" --host="${HOST}" --without-zlib --without-bzip2 --without-png && \
make && sudo make install
