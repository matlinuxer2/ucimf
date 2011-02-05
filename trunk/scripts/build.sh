#!/bin/bash

ROOT="$( readlink -f $(dirname $0)/..)" &&  source ${ROOT}/scripts/env.sh

build_clean(){
	test -d ${BUILD} && rm -rf ${BUILD}
	mkdir -p ${BUILD}
}

autotools_build(){
	local SRC_DIR=$1

	echo "==== entering ${SRC_DIR} and start to build ===="
	pushd .
	cd ${SRC_DIR}
		test -f ./configure || autoreconf -sif || return 1
		test -f Makefile && make clean && make distclean 
		( ./configure --prefix=${BUILD} && make && make install ) || return 1
	popd
}

build_clean
autotools_build ${LIBUCIMF} || exit
autotools_build ${UCIMFOV} || exit
autotools_build ${UCIMFSUNPINYIN} || exit

