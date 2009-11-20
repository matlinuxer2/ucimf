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
	LIBUCIMF_PATH2=${LIBUCIMF_PATH/-/_}
	LIBUCIMF_PATH3=${LIBUCIMF_PATH2/tar.gz/orig.tar.gz}
	LIBUCIMF_FILE=${LIBUCIMF_PATH%.tar.gz}
	LIBUCIMF_FILE2=${LIBUCIMF_FILE/-/_}

	cd ${TARBALL}
	test -d debian && rm -r debian
        mkdir debian && cd debian
		cp ../${LIBUCIMF_PATH} .
		tar -zxvf ${LIBUCIMF_PATH}
		test -d ${LIBUCIMF_FILE} && mv ${LIBUCIMF_FILE} ${LIBUCIMF_FILE2}
		test -f ${LIBUCIMF_PATH} && mv ${LIBUCIMF_PATH} ${LIBUCIMF_PATH3}
		cd ${LIBUCIMF_FILE2}

		test -d debian && rm -r debian
		mkdir debian
		find ${SCRIPTS}/debian/libucimf | grep -v '.svn' | xargs cp -t debian
		find . -name '.svn'| xargs echo 
		#dpkg-buildpackage -rfakeroot -b -d -nc
		debuild 
		cd .. 
		mv *.deb ${BINARY}

	cd ..
	popd
}

build_ucimf-openvanilla_deb(){
        pushd .

	UCIMFOV_PATH=$( ls ${TARBALL} |grep 'ucimf-openvanilla.*.tar.gz'| head --lines=1 )
	UCIMFOV_PATH2=${UCIMFOV_PATH/lla-/lla_}
	UCIMFOV_PATH3=${UCIMFOV_PATH2/tar.gz/orig.tar.gz}
	UCIMFOV_FILE=${UCIMFOV_PATH%.tar.gz}
	UCIMFOV_FILE2=${UCIMFOV_FILE/lla-/lla_}

	cd ${TARBALL}
	test -d debian && rm -r debian
        mkdir debian && cd debian
		cp ../${UCIMFOV_PATH} .
		tar -zxvf ${UCIMFOV_PATH}
		test -d ${UCIMFOV_FILE} && mv ${UCIMFOV_FILE} ${UCIMFOV_FILE2}
		test -f ${UCIMFOV_PATH} && mv ${UCIMFOV_PATH} ${UCIMFOV_PATH3}
		cd ${UCIMFOV_FILE2}

		test -d debian && rm -r debian
		mkdir debian
		find ${SCRIPTS}/debian/ucimf-openvanilla | grep -v '.svn' | xargs cp -t debian
		find . -name '.svn'| xargs echo 
		#dpkg-buildpackage -rfakeroot -b -d -nc
		debuild 
		cd .. 
		mv *.deb ${BINARY}

	cd ..
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
