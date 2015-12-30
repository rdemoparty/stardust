FROM ubuntu:14.04
MAINTAINER Adrian Scripca <benishor@gmail.com>

RUN \
	apt-get update && \
	apt-get -y dist-upgrade && \
	apt-get install -y wget make cmake bzip2 gcc g++ zip build-essential git vim

RUN apt-get install -y libsdl2-dev libsdl2-mixer-dev 

RUN apt-get install -y mingw-w64
RUN \
	mkdir -p /opt/cross-tools/i686-w64-mingw32 && cd /tmp && \
	wget https://www.libsdl.org/release/SDL2-devel-2.0.3-mingw.tar.gz && \
	tar -xvzf SDL2-devel-2.0.3-mingw.tar.gz && mv SDL2-2.0.3/i686-w64-mingw32 /opt/cross-tools/ && \
	mv SDL2-2.0.3/x86_64-w64-mingw32 /opt/cross-tools/ && \
	wget https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.0.0-mingw.tar.gz && \
	tar -xvzf SDL2_mixer-devel-2.0.0-mingw.tar.gz && cp -R SDL2_mixer-2.0.0/i686-w64-mingw32/* /opt/cross-tools/i686-w64-mingw32/ && \
	cp -R SDL2_mixer-2.0.0/x86_64-w64-mingw32/* /opt/cross-tools/x86_64-w64-mingw32/

RUN \
	cd /tmp && \
	wget https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.0-mingw.tar.gz && \
	tar -xvzf SDL2_image-devel-2.0.0-mingw.tar.gz && \ 
	cp -R SDL2_image-2.0.0/i686-w64-mingw32/* /opt/cross-tools/i686-w64-mingw32/ && \
	cp -R SDL2_image-2.0.0/x86_64-w64-mingw32/* /opt/cross-tools/x86_64-w64-mingw32/

# Freetype mingw

RUN cd /tmp && wget http://sourceforge.net/projects/freetype/files/freetype2/2.6.2/freetype-2.6.2.tar.bz2/download && mv download freetype-2.6.2.tar.bz2
RUN cd /tmp && tar -xf freetype-2.6.2.tar.bz2

ENV TARGET /opt/cross-tools/x86_64-w64-mingw32
ENV HOST x86_64-w64-mingw32
ENV PATH "${TARGET}/gcc/bin:$PATH"
ENV PKG_CONFIG_PATH "${TARGET}/lib/pkgconfig"

RUN mkdir -p /tmp/build && cd /tmp/build && \
	../freetype-2.6.2/configure --prefix="${TARGET}" --host="${HOST}" --without-zlib --without-bzip2 --without-png && \
	make && make install

VOLUME /stardust
WORKDIR /stardust

