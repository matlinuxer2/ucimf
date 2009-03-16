#!/bin/bash
source inc.sh

build_libucimf_deb(){
	test -L ${LIBUCIMF}/debian && rm ${LIBUCIMF}/debian
	ln -s  ${SCRIPTS}/debian/libucimf ${LIBUCIMF}/debian
	cd ${LIBUCIMF}
	dpkg-buildpackage -rfakeroot -b 
	cd .. && rm *.dsc *.changes *.tar.gz && cd -
	rm ${LIBUCIMF}/debian

	cd ${SCRIPTS}
}

build_ucimf-openvanilla_deb(){
	test -L ${UCIMFOV}/debian && rm ${UCIMFOV}/debian
	ln -s  ${SCRIPTS}/debian/ucimf-openvanilla ${UCIMFOV}/debian
	cd ${UCIMFOV}
	dpkg-buildpackage -rfakeroot -b 
	cd .. && rm *.dsc *.changes *.tar.gz && cd -
	rm ${UCIMFOV}/debian
	

	cd ${SCRIPTS}
}

build_libucimf_deb
build_ucimf-openvanilla_deb
