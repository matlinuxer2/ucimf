#!/bin/bash

#SRC_URL="http://sourceforge.jp/projects/jfbterm/downloads/13501/jfbterm-0.4.7.tar.gz"
SRC_URL="http://keihanna.dl.sourceforge.jp/jfbterm/13501/jfbterm-0.4.7.tar.gz"
SRC_TARBALL="jfbterm-0.4.7.tar.gz"

src_fetch(){
	echo "Fetch original source code"
	wget --continue ${SRC_URL}
}

src_unpack(){
	echo "Clean old source"
	test -d jfbterm-0.4.7 && rm -rvf jfbterm-0.4.7

	echo "Unpack original code"
	test -f ${SRC_TARBALL} && tar -zxvf ${SRC_TARBALL} 

	patch -d jfbterm-0.4.7 -p3 < patches/fbdpsp.c.diff
}

src_unpack_debian(){
	echo "Clean old source"
	test -d jfbterm-0.4.7 && rm -rvf jfbterm-0.4.7

	echo "Unpack debian source package"
	apt-get source jfbterm
}

src_patch(){
	echo "Adopt patches"
	patch -d jfbterm-0.4.7 < patches/jfbterm-0.4.7-ucimf.patch
	patch -d jfbterm-0.4.7 -p3 < patches/Makefile.am.patch

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
