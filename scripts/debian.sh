#!/bin/bash

ROOT="$(dirname $(pwd))"
SCRIPTS="${ROOT}/scripts"
LIBUCIMF="${ROOT}/libucimf"
UCIMFOV="${ROOT}/ucimf-openvanilla"
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

build_ucimf-openvanilla_deb(){
	test -L ${UCIMFOV}/debian && rm ${UCIMFOV}/debian
	ln -s  ${SCRIPTS}/debian/ucimf-openvanilla ${UCIMFOV}/debian
	cd ${UCIMFOV}
	dpkg-buildpackage -rfakeroot && cp -av *.deb ${ROOT} 
	rm ${UCIMFOV}/debian

	cd ${SCRIPTS}
}

build_libucimf_deb
build_ucimf-openvanilla_deb
