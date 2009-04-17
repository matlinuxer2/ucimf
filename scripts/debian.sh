#!/bin/bash
source inc.sh

if [ "x$(which dh_install)" = "x" ]
then
    echo "Please install debhelper first. ( aptitude install debhelper )"
    exit 3
fi


OVMOD="${OV}/openvanilla-modules"
BINARY="${SCRIPTS}/debian/binary"
test -d ${BINARY} || mkdir -p ${BINARY}

make_apt(){
	cd ${BINARY}	
	test -L ${BINARY}/conf || ln -s  ${SCRIPTS}/debian/conf ${BINARY}/conf
	ls
	rm -rvf db dists pool
	#cd ..
	#dpkg-scanpackages binary /dev/null | gzip -9c > binary/Packages.gz
	for deb_file in `ls *.deb`
	do
		reprepro --ask-passphrase -Vb . includedeb lenny $deb_file
	done
}

clean_deb(){
	rm -v ${BINARY}/*.deb
}

build_libucimf_deb(){
	test -L ${LIBUCIMF}/debian && rm ${LIBUCIMF}/debian
	ln -s  ${SCRIPTS}/debian/libucimf ${LIBUCIMF}/debian
	cd ${LIBUCIMF}
	dpkg-buildpackage -rfakeroot -b -d -nc
	rm ${LIBUCIMF}/debian
	cd .. ; #rm *.dsc *.changes *.tar.gz

	mv *.deb *.dsc *.changes *.tar.gz ${BINARY}

	cd ${SCRIPTS}
}

build_ucimf-openvanilla_deb(){
	test -L ${UCIMFOV}/debian && rm ${UCIMFOV}/debian
	ln -s  ${SCRIPTS}/debian/ucimf-openvanilla ${UCIMFOV}/debian
	cd ${UCIMFOV}
	dpkg-buildpackage -rfakeroot -b -d
	rm ${UCIMFOV}/debian
	cd .. ; rm *.dsc *.changes *.tar.gz 
	
	mv *.deb ${BINARY}

	cd ${SCRIPTS}
}

build_openvanilla-modules_deb(){
	test -L ${OVMOD}/debian && rm ${OVMOD}/debian
	ln -s  ${SCRIPTS}/debian/openvanilla-modules ${OVMOD}/debian
	cd ${OVMOD}
	dpkg-buildpackage -rfakeroot -b -d
	rm ${OVMOD}/debian
	cd .. ; rm *.dsc *.changes *.tar.gz 

	mv *.deb ${BINARY}

	cd ${SCRIPTS}
}

build_fbterm-ucimf_deb(){
	test -L ${FBTERMUCIMF}/debian && rm ${FBTERMUCIMF}/debian
	ln -s  ${SCRIPTS}/debian/fbterm_ucimf ${FBTERMUCIMF}/debian
	cd ${FBTERMUCIMF}
	dpkg-buildpackage -rfakeroot -b -d
	rm ${FBTERMUCIMF}/debian
	cd .. ; rm *.dsc *.changes *.tar.gz 

	mv *.deb ${BINARY}

	cd ${SCRIPTS}
}

build_jfbterm_deb(){
	cd ${CONSOLEJ}
	test -d jfbterm-0.4.7 || ./init.sh
	cd jfbterm-0.4.7
	dpkg-buildpackage -rfakeroot -b -d
	cd .. ; rm *.dsc *.changes *.tar.gz 

	mv *.deb ${BINARY}

	cd ${SCRIPTS}
}

clean_deb
build_libucimf_deb
build_ucimf-openvanilla_deb
build_openvanilla-modules_deb
build_fbterm-ucimf_deb
build_jfbterm_deb
make_apt
