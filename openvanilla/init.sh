#!/bin/bash

SRC_URL="http://ucimf.googlecode.com/files/openvanilla-modules-0.8.0_9.tar.gz"
SRC_TARBALL="$(basename $SRC_URL)"
SRC_ALIAS="openvanilla-modules"

src_fetch(){
	echo "Fetch original source code"
	test -f ${SRC_TARBALL} || wget --continue ${SRC_URL}
}

src_unpack(){
	echo "Unpack original code"
	SRC_DIR=${SRC_TARBALL%.tar.gz}

	test -d ${SRC_DIR} && rm -rvf ${SRC_DIR}
	test -f ${SRC_TARBALL} && tar -zxvf ${SRC_TARBALL} 
	test -L ${SRC_ALIAS} && rm -f ${SRC_ALIAS}
	ln -s ${SRC_DIR} ${SRC_ALIAS}

	echo "Adopt patches"
	#patch -p1 -d ${SRC_DIR} < patches/01.patch
}

src_fetch
src_unpack
