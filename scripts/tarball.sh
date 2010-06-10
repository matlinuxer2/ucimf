#!/bin/bash

ROOT="$( dirname $(echo $0))/.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh


test -d ${TARBALL} && rm -rvf ${TARBALL} 
install -d ${TARBALL}

make_tarball_of_libucimf(){
	pushd .

	echo "Start to make tarball of libucimf..."
	cd ${LIBUCIMF}
	autoreconf -sif
	test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
	make distcheck && ls -t libucimf*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	popd
}

make_tarball_of_ucimf-openvanilla(){
	pushd .

	echo "Start to make tarball of ucimf-openvanilla..."
	cd ${UCIMFOV}
	autoreconf -sif
	test -f configure && OV_MODULEDIR=${BUILD}/lib/openvanilla ./configure --prefix=${BUILD}
	make distcheck && ls -t ucimf-openvanilla*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	popd
}

# first
make_tarball_of_libucimf
make_tarball_of_ucimf-openvanilla
