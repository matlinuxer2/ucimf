#!/bin/bash

ROOT="$( dirname $(echo $0))/.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh


build_clean(){
	pushd .

	test -d ${BUILD} && rm -rvf ${BUILD}
	mkdir -p ${BUILD}
	
	popd
}

build_libucimf(){
	pushd .

	echo "Start to build libucimf"
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	cd ${LIBUCIMF}
	test -f ./configure || autoreconf -sif || return 1
	test -f Makefile && make distclean 
	( ./configure --prefix=${BUILD} && make && make install ) || return 1
	sed -i "s@/usr@${BUILD}@g" ${BUILD}/etc/ucimf.conf

	popd
}

build_ucimf-openvanilla(){
	pushd .

	echo "Start to build ucimf-openvanilla"
	cd ${UCIMFOV}
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	test -f ./configure || autoreconf -sif || return 1
	test -f Makefile && make distclean
	( ./configure --prefix=${BUILD} && make && make install ) || return 1

	popd
}

build_clean
build_libucimf || exit 
build_ucimf-openvanilla || exit 
