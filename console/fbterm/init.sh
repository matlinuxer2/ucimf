#!/bin/bash

#SRC_URL="http://sourceforge.jp/projects/jfbterm/downloads/13501/jfbterm-0.4.7.tar.gz"
SRC_URL="http://fbterm.googlecode.com/files/fbterm-1.2.tar.gz"
SRC_TARBALL="fbterm-1.2.tar.gz"

src_fetch(){
	echo "Fetch original source code"
	wget --continue ${SRC_URL}
}

src_unpack(){
	echo "Unpack original code"
	test -d fbterm-1.2 && rm -rvf fbterm-1.2
	test -f ${SRC_TARBALL} && tar -zxvf ${SRC_TARBALL} 

	echo "Adopt patches"
	patch -p1 -d fbterm-1.2 < patches/fbterm-1.2-ucimf.patch
}

src_fetch
src_unpack
