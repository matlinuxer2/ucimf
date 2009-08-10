#!/bin/bash

ROOT="$( dirname $(echo $0))/../.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh


if [ "x$(which dh_install)" = "x" ]
then
    echo "Please install debhelper first. ( aptitude install debhelper )"
    exit 3
fi


OVMOD="${OV}/openvanilla-modules"
BINARY="${SCRIPTS}/debian/binary"
REPOS="${SCRIPTS}/debian/repos"
test -d ${BINARY} || mkdir -p ${BINARY}

make_apt(){
	pushd .

	codename=`lsb_release -c | cut -c11-`

	rm -r $REPOS
	install -d $REPOS/conf

	cat > ${REPOS}/conf/distributions << EOF
Origin: UCIMF
Label: UCIMF
Codename: $codename
Architectures: amd64 i386
Components: main contrib non-free
Description: Repository of UCIMF
SignWith: default
EOF

	for deb_file in `find ${BINARY} -iname '*.deb'`
	do
		reprepro --ask-passphrase -Vb ${REPOS} includedeb $codename $deb_file
	done

	popd
}

clean_deb(){
	rm -v ${BINARY}/*.deb
}

build_libucimf_deb(){
	pushd . 

	test -L ${LIBUCIMF}/debian && rm ${LIBUCIMF}/debian
	ln -s  ${SCRIPTS}/debian/libucimf ${LIBUCIMF}/debian
	cd ${LIBUCIMF}
	dpkg-buildpackage -rfakeroot -b -d -nc
	rm ${LIBUCIMF}/debian
	cd .. ; #rm *.dsc *.changes *.tar.gz

	mv *.deb *.dsc *.changes *.tar.gz ${BINARY}

	popd
}

build_ucimf-openvanilla_deb(){
        pushd .

	test -L ${UCIMFOV}/debian && rm ${UCIMFOV}/debian
	ln -s  ${SCRIPTS}/debian/ucimf-openvanilla ${UCIMFOV}/debian
	cd ${UCIMFOV}
	dpkg-buildpackage -rfakeroot -b -d
	rm ${UCIMFOV}/debian
	cd .. ; rm *.dsc *.changes *.tar.gz 
	
	mv *.deb ${BINARY}

	popd
}

build_openvanilla-modules_deb(){
        pushd .

	test -L ${OVMOD}/debian && rm ${OVMOD}/debian
	ln -s  ${SCRIPTS}/debian/openvanilla-modules ${OVMOD}/debian
	cd ${OVMOD}
	dpkg-buildpackage -rfakeroot -b -d
	rm ${OVMOD}/debian
	cd .. ; rm *.dsc *.changes *.tar.gz 

	mv *.deb ${BINARY}

	popd
}

build_fbterm-ucimf_deb(){
	pushd .

	test -L ${FBTERMUCIMF}/debian && rm ${FBTERMUCIMF}/debian
	ln -s  ${SCRIPTS}/debian/fbterm_ucimf ${FBTERMUCIMF}/debian
	cd ${FBTERMUCIMF}
	dpkg-buildpackage -rfakeroot -b -d
	rm ${FBTERMUCIMF}/debian
	cd .. ; rm *.dsc *.changes *.tar.gz 

	mv *.deb ${BINARY}

	popd
}

build_jfbterm_deb(){
	pushd .

	cd ${CONSOLEJ}
	test -d jfbterm-0.4.7 || ./init.sh
	cd jfbterm-0.4.7
	dpkg-buildpackage -rfakeroot -b -d
	cd .. ; rm *.dsc *.changes *.tar.gz 

	mv *.deb ${BINARY}

	popd
}

clean_deb
build_libucimf_deb
build_ucimf-openvanilla_deb
build_openvanilla-modules_deb
build_fbterm-ucimf_deb
build_jfbterm_deb
make_apt
