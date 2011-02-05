#!/bin/bash

ROOT="$( readlink -f $(dirname $0)/..)" &&  source ${ROOT}/scripts/env.sh

tarball_clean(){
	#test -d ${TARBALL} && rm -rf ${TARBALL} 
	mkdir -p ${TARBALL}
}

make_tarball(){
	local SRC_DIR=$1

	echo "==== entering ${SRC_DIR} and start to make tarball ===="
	pushd .
	cd ${SRC_DIR}
		test -f ./configure || autoreconf -sif || return 1
		#test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
		test -f configure && ./configure --prefix=${BUILD} || return 1
		make distcheck && ( ls -t *.tar.gz | head -n1 | xargs cp -t ${TARBALL} ) || return 1
	popd
}

tarball_clean
make_tarball ${LIBUCIMF}
make_tarball ${UCIMFOV}
make_tarball ${UCIMFSUNPINYIN}
