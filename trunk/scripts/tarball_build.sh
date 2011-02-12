#!/bin/bash

ROOT="$( readlink -f $(dirname $0)/..)" &&  source ${ROOT}/scripts/env.sh

build_clean(){
	test -d ${BUILD} && rm -rf ${BUILD}
	mkdir -p ${BUILD}
}

tarball_build(){
	local NAME=$1

	install -d ${TARBALL}

	echo "==== entering ${TARBALL} and start to build ${NAME} ===="
	pushd .
	cd ${TARBALL}
		TARBALL_FILE=$( ls -t |grep "$NAME.*.tar.gz"| head --lines=1 )
		TARBALL_DIR=${TARBALL_FILE%.tar.gz}

		( tar -zxf ${TARBALL_FILE} && test -d "${TARBALL_DIR}" ) || return 1

		pushd .
		cd ${TARBALL_DIR}
			./configure --prefix=${BUILD} && make && make install
		popd .

		rm -rf ${TARBALL_DIR}
	popd
}

function prepare_tarball(){
	local NAME=$1
	local VERSION=$2

	install -d ${TARBALL}

	pushd .
	cd ${TARBALL}
		THE_TARBALL=$( ls -t |grep "$NAME.*.tar.gz"| head --lines=1 )

		test -f "$THE_TARBALL" || ( THE_TARBALL="$NAME-$VERSION.tar.gz" ; wget --continue http://ucimf.googlecode.com/files/$THE_TARBALL )
	popd
}


prepare_tarball libucimf 2.3.7
prepare_tarball ucimf-openvanilla 2.10.11
prepare_tarball ucimf-sunpinyin 0.4
prepare_tarball openvanilla-modules 0.9.0a1.3
prepare_tarball fbterm_ucimf 0.2.9

build_clean
tarball_build "libucimf"
tarball_build "ucimf-openvanilla"
tarball_build "ucimf-sunpinyin"
tarball_build "ucimf-chewing"
tarball_build "openvanilla-modules"
tarball_build "fbterm_ucimf"

