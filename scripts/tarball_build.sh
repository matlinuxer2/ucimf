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


tarball_build_libucimf(){
	LIBUCIMF_PATH=$( ls ${TARBALL} |grep 'libucimf.*.tar.gz'| head --lines=1 )
	LIBUCIMF_FILE=${LIBUCIMF_PATH%.tar.gz}

	echo "Start to build libucimf"

	cd ${TARBALL}
	tar -zxvf ${LIBUCIMF_PATH}
	cd ${LIBUCIMF_FILE}
	./configure --prefix=${BUILD} && make && make install

	cd ${TARBALL}
	rm -rvf ${LIBUCIMF_FILE}

	back_to_scripts
}

tarball_build_ucimf-openvanilla(){
	pushd .


	UCIMFOV_PATH=$( ls ${TARBALL} |grep 'ucimf-openvanilla.*.tar.gz'| head --lines=1 )
	UCIMFOV_FILE=${UCIMFOV_PATH%.tar.gz}

	echo "Start to build ucimf-openvanilla"

	cd ${TARBALL}
	tar -zxvf ${UCIMFOV_PATH}
	cd ${UCIMFOV_FILE}
	./configure --prefix=${BUILD} && make && make install

	cd ${TARBALL}
	rm -rvf ${UCIMFOV_FILE}

	popd

}


tarball_build_fbterm_ucimf(){
	pushd .


	FBTERMUCIMF_PATH=$( ls ${TARBALL} |grep 'fbterm_ucimf.*.tar.gz'| head --lines=1 )
	FBTERMUCIMF_FILE=${FBTERMUCIMF_PATH%.tar.gz}

	echo "Start to build fbterm_ucimf"

	cd ${TARBALL}
	tar -zxvf ${FBTERMUCIMF_PATH}
	cd ${FBTERMUCIMF_FILE}
	./configure --prefix=${BUILD} && make && make install

	cd ${TARBALL}
	rm -rvf ${FBTERMUCIMF_FILE}

	popd

}


build_clean
tarball_build_libucimf
tarball_build_ucimf-openvanilla
tarball_build_fbterm_ucimf

