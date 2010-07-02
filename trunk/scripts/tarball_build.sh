#!/bin/bash

ROOT="$( readlink -f $(dirname $0)/..)" &&  source ${ROOT}/scripts/env.sh

build_clean(){
	test -d ${BUILD} && rm -rf ${BUILD}
	mkdir -p ${BUILD}
}

tarball_build(){
	local NAME=$1

	echo "==== entering ${TARBALL} and start to build ${NAME} ===="
	pushd .
	cd ${TARBALL}
		TARBALL_FILE=$( ls -t |grep "$NAME.*.tar.gz"| head --lines=1 )
		TARBALL_DIR=${TARBALL_FILE%.tar.gz}

		( tar -zxf ${TARBALL_FILE} && test -d ${TARBALL_DIR} ) || return 1

		pushd .
		cd ${TARBALL_DIR}
			./configure --prefix=${BUILD} && make && make install
		popd .

		rm -rf ${TARBALL_DIR}
	popd
}

build_clean
tarball_build "libucimf"
tarball_build "ucimf-openvanilla"
tarball_build "openvanilla-modules"
tarball_build "fbterm_ucimf"

