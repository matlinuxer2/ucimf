#!/bin/bash

#SRC_URL="http://sourceforge.jp/projects/jfbterm/downloads/13501/jfbterm-0.4.7.tar.gz"
SRC_URL="http://ftp.de.debian.org/debian/pool/main/b/bogl/bogl_0.1.18-3.tar.gz"
SRC_TARBALL=$(basename $SRC_URL)
SRC_DIR="bogl-0.1.18"
ROOT=$(pwd)

src_fetch(){
	cd ${ROOT}

	echo "Fetch original source code"
	test -f ${SRC_TARBALL} || wget --continue ${SRC_URL}
}

src_unpack(){
	cd ${ROOT}
	echo "Clean old source"
	test -d ${SRC_DIR} && rm -rvf ${SRC_DIR}

	echo "Unpack original code"
	test -f ${SRC_TARBALL} && tar -zxvf ${SRC_TARBALL} 

	patch -d ${SRC_DIR} < patches/ucimf_for_boglterm.patch
}

src_fetch
src_unpack
