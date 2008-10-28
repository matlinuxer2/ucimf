#!/bin/bash

#SRC_URL="http://sourceforge.jp/projects/jfbterm/downloads/13501/jfbterm-0.4.7.tar.gz"
SRC_URL="http://keihanna.dl.sourceforge.jp/jfbterm/13501/jfbterm-0.4.7.tar.gz"
SRC_TARBALL="jfbterm-0.4.7.tar.gz"

src_fetch(){
	echo "Fetch original source code"
	wget --continue ${SRC_URL}
}

src_unpack(){
	echo "Unpack original code"
	test -d jfbterm-0.4.7 && rm -rvf jfbterm-0.4.7
	test -f ${SRC_TARBALL} && tar -zxvf ${SRC_TARBALL} 

	echo "Adopt patches"
	patch -d jfbterm-0.4.7 < patches/jfbterm-0.4.7-ucimf.patch
}

src_fetch
src_unpack
