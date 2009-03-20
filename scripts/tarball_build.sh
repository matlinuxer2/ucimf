#!/bin/bash
source ./inc.sh

tarball_build_libucimf(){
	LIBUCIMF_PATH=$( ls ${TARBALL} |grep 'libucimf.*.tar.gz'| head --lines=1 )
	LIBUCIMF_FILE=${LIBUCIMF_PATH%.tar.gz}

	echo "Start to build libucimf"

	cd ${TARBALL}
	tar -zxvf ${LIBUCIMF_PATH}
	cd ${LIBUCIMF_FILE}
	./configure --prefix=${BUILD} && make && make install

	cd ${TARBALL}
	rm -rvf ${LIBUCIMF_FILE}

	back_to_scripts
}

tarball_build_ucimf-openvanilla(){

	UCIMFOV_PATH=$( ls ${TARBALL} |grep 'ucimf-openvanilla.*.tar.gz'| head --lines=1 )
	UCIMFOV_FILE=${UCIMFOV_PATH%.tar.gz}

	echo "Start to build ucimf-openvanilla"

	cd ${TARBALL}
	tar -zxvf ${UCIMFOV_PATH}
	cd ${UCIMFOV_FILE}
	./configure --prefix=${BUILD} && make && make install

	cd ${TARBALL}
	rm -rvf ${UCIMFOV_FILE}

	back_to_scripts

}

tarball_build_openvanilla-modules(){

	OVMOD_PATH=$( ls ${TARBALL} |grep 'openvanilla-modules.*.tar.gz'| head --lines=1 )
	OVMOD_FILE=${OVMOD_PATH%.tar.gz}

	echo "Start to build openvanilla-modules"

	cd ${TARBALL}
	tar -zxvf ${OVMOD_PATH}
	cd ${OVMOD_FILE}
	./configure --prefix=${BUILD} && make && make install

	cd ${TARBALL}
	rm -rvf ${OVMOD_FILE}

	back_to_scripts
}

tarball_build_fbterm_ucimf(){

	FBTERMUCIMF_PATH=$( ls ${TARBALL} |grep 'fbterm_ucimf.*.tar.gz'| head --lines=1 )
	FBTERMUCIMF_FILE=${FBTERMUCIMF_PATH%.tar.gz}

	echo "Start to build fbterm_ucimf"

	cd ${TARBALL}
	tar -zxvf ${FBTERMUCIMF_PATH}
	cd ${FBTERMUCIMF_FILE}
	./configure --prefix=${BUILD} && make && make install

	cd ${TARBALL}
	rm -rvf ${FBTERMUCIMF_FILE}

	back_to_scripts

}

tarball_build_fbterm(){

	FBTERM_PATH=$( ls ${TARBALL} |grep 'fbterm.*.tar.gz'| head --lines=1 )
	FBTERM_FILE=${FBTERM_PATH%.tar.gz}

	echo "Start to build fbterm"

	cd ${TARBALL}
	tar -zxvf ${FBTERM_PATH}
	cd ${FBTERM_FILE}
	./configure --prefix=${BUILD} && make && make install

	cd ${TARBALL}
	rm -rvf ${FBTERM_FILE}

	back_to_scripts
}

tarball_build_jfbterm(){

	JFBTERM_PATH=$( ls ${TARBALL} |grep 'jfbterm.*.tar.gz'| head --lines=1 )
	JFBTERM_FILE=${JFBTERM_PATH%.tar.gz}

	echo "Start to build jfbterm"

	cd ${TARBALL}
	tar -zxvf ${JFBTERM_PATH}
	cd ${JFBTERM_FILE}
	./configure --prefix=${BUILD} && make && make install

	cd ${TARBALL}
	rm -rvf ${JFBTERM_FILE}

	back_to_scripts
}

build_clean
tarball_build_libucimf
tarball_build_ucimf-openvanilla
tarball_build_openvanilla-modules
tarball_build_fbterm_ucimf
tarball_build_fbterm
tarball_build_jfbterm

