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


	LIBUCIMF_PATH=$( ls ${TARBALL} |grep 'libucimf.*.tar.gz'| head --lines=1 )
	LIBUCIMF_FILE=${LIBUCIMF_PATH%.tar.gz}

	cd ${TARBALL}
	tar -zxvf ${LIBUCIMF_PATH}
	cd ${LIBUCIMF_FILE}

        cp -vr ${SCRIPTS}/debian/libucimf debian
	dpkg-buildpackage -rfakeroot -b -d -nc
	cd .. 
	mv *.deb ${BINARY}

	popd
}

build_ucimf-openvanilla_deb(){
        pushd .

	UCIMFOV_PATH=$( ls ${TARBALL} |grep 'ucimf-openvanilla.*.tar.gz'| head --lines=1 )
	UCIMFOV_FILE=${UCIMFOV_PATH%.tar.gz}

	cd ${TARBALL}
	tar -zxvf ${UCIMFOV_PATH}
	cd ${UCIMFOV_FILE}

        cp -vr ${SCRIPTS}/debian/ucimf-openvanilla debian
	dpkg-buildpackage -rfakeroot -b -d -nc
	cd .. 
	mv *.deb ${BINARY}

	popd
}

build_openvanilla-modules_deb(){
        pushd .

	OVMOD_PATH=$( ls ${TARBALL} |grep 'openvanilla-modules.*.tar.gz'| head --lines=1 )
	OVMOD_FILE=${OVMOD_PATH%.tar.gz}

	cd ${TARBALL}
	tar -zxvf ${OVMOD_PATH}
	cd ${OVMOD_FILE}

        cp -vr ${SCRIPTS}/debian/openvanilla-modules debian
	dpkg-buildpackage -rfakeroot -b -d -nc
	cd .. 
	mv *.deb ${BINARY}

	popd
}

build_fbterm-ucimf_deb(){
	pushd .

	FBTERMUCIMF_PATH=$( ls ${TARBALL} |grep 'fbterm_ucimf.*.tar.gz'| head --lines=1 )
	FBTERMUCIMF_FILE=${FBTERMUCIMF_PATH%.tar.gz}

	cd ${TARBALL}
	tar -zxvf ${FBTERMUCIMF_PATH}
	cd ${FBTERMUCIMF_FILE}

        cp -vr ${SCRIPTS}/debian/fbterm_ucimf debian
	dpkg-buildpackage -rfakeroot -b -d -nc
	cd .. 
	mv *.deb ${BINARY}

	popd
}

build_jfbterm_deb(){
	pushd .

	cd ${CONSOLEJ}
	test -d jfbterm-0.4.7 || ./init.sh
	cd jfbterm-0.4.7
	dpkg-buildpackage -rfakeroot -b -d
	cd .. 

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
