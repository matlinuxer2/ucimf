#!/bin/bash

ROOT="$(dirname $(pwd))"
SCRIPTS="${ROOT}/scripts"
LIBUCIMF="${ROOT}/libucimf"
TARBALL_DIR="${ROOT}/tarball"
TARBALL_TEMP="${ROOT}/temp"

echo ${ROOT}

build_libucimf_deb(){
	test -L ${LIBUCIMF}/debian && rm ${LIBUCIMF}/debian
	ln -s  ${SCRIPTS}/debian/libucimf ${LIBUCIMF}/debian
	cd ${LIBUCIMF}
	dpkg-buildpackage -rfakeroot && cp -av *.deb ${ROOT} 
	rm ${LIBUCIMF}/debian

	cd ${SCRIPTS}
}

build_libucimf_deb
