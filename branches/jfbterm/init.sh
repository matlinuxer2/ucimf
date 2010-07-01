#!/bin/bash

#SRC_URL="http://sourceforge.jp/projects/jfbterm/downloads/13501/jfbterm-0.4.7.tar.gz"
SRC_URL="http://keihanna.dl.sourceforge.jp/jfbterm/13501/jfbterm-0.4.7.tar.gz"
SRC_TARBALL="jfbterm-0.4.7.tar.gz"
SRC_DIR="jfbterm-0.4.7"
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

	patch -d ${SRC_DIR} -p3 < patches/fbdpsp.c.diff
	patch -d ${SRC_DIR} < patches/jfbterm-0.4.7-page.patch
}

src_update(){
	cd ${ROOT}
	# To check whether Automake 1.4 is available.
	which automake-1.4 || which automake1.4
	if (( $? )) 
	then
		echo "Regenerating autotools settings..."
		cd ${SRC_DIR}
		rm Makefile.in
		rm aclocal.m4
		autoreconf -i --force
	fi

}

src_unpack_debian(){
	cd ${ROOT}
	echo "Clean old source"
	test -d ${SRC_DIR} && rm -rvf ${SRC_DIR}

	echo "Unpack debian source package"
	apt-get source jfbterm
	patch -d ${SRC_DIR} -p1 < patches/debian_changelog.patch
}

src_patch(){
	cd ${ROOT}
	echo "Adopt patches"
	patch -d ${SRC_DIR} < patches/jfbterm-0.4.7-ucimf.patch
	patch -d ${SRC_DIR} < patches/Makefile.am.patch

}


which dpkg 2>/dev/null
if (( !$? )) 
then
	# unpack source for debian based distributions
	src_unpack_debian
else
	# unpack source for general distributions
	src_fetch
	src_unpack
fi

src_patch
src_update
