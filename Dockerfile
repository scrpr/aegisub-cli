FROM --platform=linux/amd64 ubuntu
# linux/amd64 build of aegisub-cli

RUN apt-get update

# deps: python3 and meson / ninja
RUN apt-get install -y python3 python3-pip python3-setuptools \
                       python3-wheel ninja-build
# deps: ICU development
RUN apt-get install -y libc6-dev
# deps: Boost
RUN apt-get install -y libboost-all-dev
# deps: FFMS2
RUN apt-get install -y ffmsindex
# downloading source
RUN apt-get install -y git
# meson
RUN pip3 install meson
# deps: cmake
RUN apt-get install -y cmake
# deps: iconv
RUN apt-get install -y libfontconfig1-dev
# deps: readline
RUN apt-get install libreadline8 libreadline-dev
# deps: wxWidgets - https://gist.github.com/pemd-sys/6aed397bcbdb380cb53bc09183f3a8f4
RUN apt-get install -y build-essential
RUN apt-get install -y libgtk2.0-dev
RUN apt-get install -y libgtk-3-dev
RUN apt-get install -y mesa-utils
RUN apt-get install -y freeglut3-dev
RUN apt-get install -y libjpeg-dev
RUN apt-get install -y liblzma-dev

# deps: installing wxWidgets
WORKDIR /usr/bin
RUN curl -LJO https://github.com/wxWidgets/wxWidgets/releases/download/v3.0.5/wxWidgets-3.0.5.tar.bz2
RUN tar -xvf wxWidgets-3.0.5.tar.bz2
WORKDIR /usr/bin/wxWidgets-3.0.5
RUN mkdir gtk-build
WORKDIR /usr/bin/wxWidgets-3.0.5/gtk-build
RUN ../configure  --with-gtk=3 --with-opengl
RUN apt-get install -y libreadline8 libreadline-dev
RUN make -j3
RUN make install
RUN ldconfig

# main: preparing and compiling aegisub-cli

# RUN cd /usr/bin && git clone https://github.com/Myaamori/aegisub-cli.git

## Observed error:
## Fetching value of define "LJ_HASFFI" : 1 
## Build targets in project: 113
## Subproject luajit finished.
#
#
## subprojects/luabins/src/meson.build:1:14: ERROR: Sandbox violation: Tried to grab file fwrite.c from a nested subproject.

# WORKAROUND proposed by @arch1t3ct 
# https://github.com/arch1t3cht/Aegisub/commit/22fb4c6e8cb8be3ad135077980fe7c50b8929a62

# Forked from @Myaamori's repo, with @arch1t3cht's work around to fix "sandbox violation" issue on Linux/MacOS build
RUN cd /usr/bin && git clone https://github.com/mcbarb/aegisub-cli.git

WORKDIR /usr/bin/aegisub-cli

RUN mkdir builddir
RUN meson --prefix=/usr --buildtype=release builddir
RUN ninja -C builddir src/aegisub-cli
