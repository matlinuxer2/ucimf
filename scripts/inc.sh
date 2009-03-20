#!/bin/bash

ROOT=$( dirname $(pwd) )
SCRIPTS=${ROOT}/scripts/
BUILD=${ROOT}/build/
TARBALL=${ROOT}/tarball/
PKGBUILD=${ROOT}/pkgbuild/

# package's directory
LIBUCIMF=${ROOT}/libucimf/
UCIMFOV=${ROOT}/ucimf-openvanilla/
OV=${ROOT}/openvanilla/
CONSOLE=${ROOT}/console/fbterm/
FBTERMUCIMF=${ROOT}/console/fbterm_ucimf/
CONSOLEJ=${ROOT}/console/jfbterm/
DUMMY=${ROOT}/console/dummy/


export LIBRARY_PATH=${BUILD}/lib/
export LD_LIBRARY_PATH=${BUILD}/lib/
export LD_RUN_PATH=${BUILD}/lib/
export PATH=${PATH}:${BUILD}/bin/

back_to_scripts(){
	cd ${SCRIPTS}
}

build_clean(){
	test -d ${BUILD} && rm -rvf ${BUILD}
	mkdir -p ${BUILD}

	back_to_scripts
}

build_libucimf(){
	echo "Start to build libucimf"
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	cd ${LIBUCIMF}
	./autogen.sh
	test -f Makefile && make distclean
	./configure --prefix=${BUILD} && make && make install

	back_to_scripts
}

check_libucimf(){
	# 檢查設定檔
	echo "系統設定檔:"
	FILE=/etc/ucimf.conf
	test -f ${FILE} \
	&& ls -l ${FILE} \
	&& cat ${FILE} |grep UCIMF_FONTPATH \
	&& cat ${FILE} |grep IMF_MODULE_DIR
	echo "個人設定檔:"
	FILE=${HOME}/.ucimf.conf
	test -f ${FILE} \
	&& ls -l ${FILE} \
	&& cat ${FILE} |grep UCIMF_FONTPATH \
	&& cat ${FILE} |grep IMF_MODULE_DIR
	
	back_to_scripts
}

setup_libucimf(){
	FILE=${BUILD}/etc/ucimf.conf
	#cat ${FILE} | sed 's/.*UCIMF_FONTPATH.*=.*//g' | sed 's/.*IMF_MODULE_DIR.*=.*//g' 
	sed -i 's/^.*UCIMF_FONTPATH.*$//g' ${FILE}
	sed -i 's/^.*IMF_MODULE_DIR.*$//g' ${FILE}
	sed -i "1a UCIMF_FONTPATH = ${BUILD}/share/ucimf/unifont.pcf" ${FILE}
	sed -i "1a IMF_MODULE_DIR = ${BUILD}/lib/ucimf/" ${FILE}

	echo "測試設定檔:"
	FILE=${BUILD}/etc/ucimf.conf
	test -f ${FILE} \
	&& ls -l ${FILE} \
	&& cat ${FILE} |grep UCIMF_FONTPATH| sed 's/^.*=//g' \
	&& cat ${FILE} |grep IMF_MODULE_DIR| sed 's/^.*=//g'

	echo "檢查字型檔:"
	FONTFILE=$(cat ${FILE} |grep UCIMF_FONTPATH| sed 's/^.*=//g')
	ls -l ${FONTFILE}

	back_to_scripts
}

build_ucimf-openvanilla(){
	echo "Start to build ucimf-openvanilla"
	cd ${UCIMFOV}
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	./autogen.sh
	test -f Makefile && make distclean
	./configure --prefix=${BUILD} && make &&  make install

	back_to_scripts
}

build_openvanilla(){
	echo "Start to build openvanilla"
	cd ${OV}
	test -L openvanilla-modules || ./init.sh
	cd openvanilla-modules
	test -f Makefile && make distclean
	./configure --prefix=${BUILD} && make && make install

	back_to_scripts
}

build_console_fbterm(){
	echo "Start to build fbterm"
	cd ${CONSOLE}
	./init.sh
	cd fbterm-1.2/
	LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	make
	make install

	back_to_scripts
}

build_fbterm-ucimf(){
	echo "Start to make tarball of fbterm-ucimf..."
	cd ${FBTERMUCIMF}
	./autogen.sh
	test -f Makefile && make distclean
	./configure --prefix=${BUILD} && make && make install

	back_to_scripts
}

build_console_jfbterm(){
	echo "Start to build jfbterm"
	cd ${CONSOLEJ}
	./init.sh
	cd jfbterm-0.4.7/
	test -f Makefile && make distclean
	LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	make
	make install

	back_to_scripts
}

build_console_testing(){
	echo "Start to build testing"
	cd ${DUMMY}
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	autoreconf -i --force
	test -f Makefile && make distclean
	LIBS="-L${BUILD}/lib/" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
	make
	make install

	back_to_scripts
}

make_tarball_of_libucimf(){
	echo "Start to make tarball of libucimf..."
	cd ${LIBUCIMF}
	./autogen.sh
	test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	make distcheck && ls -t libucimf*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	back_to_scripts
}

make_tarball_of_ucimf-openvanilla(){
	echo "Start to make tarball of ucimf-openvanilla..."
	cd ${UCIMFOV}
	./autogen.sh
	test -f configure && OV_MODULEDIR=${BUILD}/lib/openvanilla ./configure --prefix=${BUILD}
	make distcheck && ls -t ucimf-openvanilla*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	back_to_scripts
}

make_tarball_of_openvanilla(){
	echo "Start to make tarball of openvanilla..."
	cd ${OV}
	test -L openvanilla-modules || ./init.sh
	cd openvanilla-modules
	test -f configure && ./configure --prefix=${BUILD}
	make distcheck && ls -t openvanilla*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	back_to_scripts
}

make_tarball_of_jfbterm(){
	echo "Start to make tarball of jfbterm..."
	cd ${CONSOLEJ}
	./init.sh
	cd jfbterm-0.4.7/
	test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	fakeroot make distcheck && ls -t jfbterm*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	back_to_scripts
}

make_tarball_of_fbterm(){
	cd ${TARBALL}
	
	FBTERM_FILE=$( ls -t fbterm*.tar.gz|grep -v 'ucimf'| head --lines=1 )
	test -f "$FBTERM_FILE" || wget --continue http://fbterm.googlecode.com/files/fbterm-1.4.tar.gz

	back_to_scripts
}

make_tarball_of_fbterm-ucimf(){
	echo "Start to make tarball of fbterm-ucimf..."
	cd ${FBTERMUCIMF}
	./autogen.sh
	test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	make distcheck && ls -t fbterm_ucimf*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	back_to_scripts
}

make_pkgbuild_of_libucimf-svn(){
	echo "Start to make pkgbuild of libucimf..."
	cd ${PKGBUILD}
	wget -O PKGBUILD_libucimf http://aur.archlinux.org/packages/libucimf-svn/libucimf-svn/PKGBUILD
	makepkg -p PKGBUILD_libucimf

	back_to_scripts
}

make_pkgbuild_of_ucimf-openvanilla-svn(){
	echo "Start to make pkgbuild of ucimf-openvanilla..."
	cd ${PKGBUILD}
	wget -O PKGBUILD_ucimf-openvanilla http://aur.archlinux.org/packages/ucimf-openvanilla-svn/ucimf-openvanilla-svn/PKGBUILD
	makepkg -p PKGBUILD_ucimf-openvanilla

	back_to_scripts
}

make_pkgbuild_of_openvanilla-svn(){
	echo "Start to make pkgbuild of openvanilla..."
	cd ${PKGBUILD}
	wget -O PKGBUILD_openvanilla http://aur.archlinux.org/packages/openvanilla-svn/openvanilla-svn/PKGBUILD
	makepkg -p PKGBUILD_openvanilla

	back_to_scripts
}

make_pkgbuild_of_fbterm-ucimf-svn(){
	echo "Start to make pkgbuild of fbterm-ucimf..."
	cd ${PKGBUILD}
	wget -O PKGBUILD_fbterm-ucimf http://aur.archlinux.org/packages/fbterm-ucimf-svn/fbterm-ucimf-svn/PKGBUILD
	makepkg -p PKGBUILD_fbterm-ucimf

	back_to_scripts
}

