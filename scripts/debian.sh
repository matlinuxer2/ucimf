#!/bin/bash
source inc.sh

OVMOD="${OV}/openvanilla-modules"

clean_deb(){
	rm -v ${SCRIPTS}/debian/*.deb
}

build_libucimf_deb(){
	test -L ${LIBUCIMF}/debian && rm ${LIBUCIMF}/debian
	ln -s  ${SCRIPTS}/debian/libucimf ${LIBUCIMF}/debian
	cd ${LIBUCIMF}
	dpkg-buildpackage -rfakeroot -b 
	cd .. && rm *.dsc *.changes *.tar.gz && cd -
	rm ${LIBUCIMF}/debian

	mv *.deb ${SCRIPTS}/debian

	cd ${SCRIPTS}
}

build_ucimf-openvanilla_deb(){
	test -L ${UCIMFOV}/debian && rm ${UCIMFOV}/debian
	ln -s  ${SCRIPTS}/debian/ucimf-openvanilla ${UCIMFOV}/debian
	cd ${UCIMFOV}
	dpkg-buildpackage -rfakeroot -b 
	cd .. && rm *.dsc *.changes *.tar.gz && cd -
	rm ${UCIMFOV}/debian
	
	mv *.deb ${SCRIPTS}/debian

	cd ${SCRIPTS}
}

build_openvanilla-modules_deb(){
	test -L ${OVMOD}/debian && rm ${OVMOD}/debian
	ln -s  ${SCRIPTS}/debian/openvanilla-modules ${OVMOD}/debian
	cd ${OVMOD}
	dpkg-buildpackage -rfakeroot -b 
	cd .. && rm *.dsc *.changes *.tar.gz && cd -
	rm ${OVMOD}/debian

	mv *.deb ${SCRIPTS}/debian

	cd ${SCRIPTS}
}

build_fbterm-ucimf_deb(){
	test -L ${FBTERMUCIMF}/debian && rm ${FBTERMUCIMF}/debian
	ln -s  ${SCRIPTS}/debian/fbterm_ucimf ${FBTERMUCIMF}/debian
	cd ${FBTERMUCIMF}
	dpkg-buildpackage -rfakeroot -b 
	cd .. && rm *.dsc *.changes *.tar.gz && cd -
	rm ${FBTERMUCIMF}/debian

	mv *.deb ${SCRIPTS}/debian

	cd ${SCRIPTS}
}

clean_deb
build_libucimf_deb
build_ucimf-openvanilla_deb
build_openvanilla-modules_deb
build_fbterm-ucimf_deb
