#!/bin/bash

ROOT=$( dirname $(pwd) )
LIBUCIMF=${ROOT}/libucimf/
UCIMFOV=${ROOT}/ucimf-openvanilla/
OV=${ROOT}/openvanilla/
CONSOLE=${ROOT}/console/fbterm/
FBTERMUCIMF=${ROOT}/console/fbterm_ucimf/
CONSOLEJ=${ROOT}/console/jfbterm/
DUMMY=${ROOT}/console/dummy/
BUILD=${ROOT}/build/
TARBALL=${ROOT}/tarball/

export LIBRARY_PATH=${BUILD}/lib/
export LD_LIBRARY_PATH=${BUILD}/lib/
export LD_RUN_PATH=${BUILD}/lib/
export PATH=${PATH}:${BUILD}/bin/


build_libucimf(){
	echo "Start to build libucimf"
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	cd ${LIBUCIMF}
	autoreconf -i --force
	UCIMF_CONF_PATH=${BUILD}/etc/ ./configure --prefix=${BUILD}
	make
	make install

	cd ${BUILD}
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
	
	cd ${BUILD}
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

	cd ${BUILD}
}

build_ucimf-openvanilla(){
	echo "Start to build ucimf-openvanilla"
	cd ${UCIMFOV}
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	autoreconf -i --force
	OV_MODULEDIR=${BUILD}/lib/openvanilla ./configure --prefix=${BUILD}
	make
	make install

	cd ${BUILD}
}

build_openvanilla(){
	echo "Start to build openvanilla"
	cd ${OV}
	./init.sh
	cd Modules/
	make
	make install INSTALL_PREFIX=${BUILD}

	cd ${BUILD}
}

build_console_fbterm(){
	echo "Start to build fbterm"
	cd ${CONSOLE}
	./init.sh
	cd fbterm-1.2/
	LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	make
	make install

	cd ${BUILD}
}

build_console_jfbterm(){
	echo "Start to build jfbterm"
	cd ${CONSOLEJ}
	./init.sh
	cd jfbterm-0.4.7/
	LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	make
	make install

	cd ${BUILD}
}

build_console_testing(){
	echo "Start to build testing"
	cd ${DUMMY}
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	autoreconf -i --force
	LIBS="-L${BUILD}/lib/" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
	make
	make install

	cd ${BUILD}
}

make_tarball_of_libucimf(){
	echo "Start to make tarball of libucimf..."
	cd ${LIBUCIMF}
	./autogen.sh
	test -f configure || LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	make distcheck && ls -t libucimf*.tar.gz | head -n1 | xargs cp -t ${TARBALL}
}

make_tarball_of_ucimf-openvanilla(){
	echo "Start to make tarball of ucimf-openvanilla..."
	cd ${UCIMFOV}
	./autogen.sh
	test -f configure || OV_MODULEDIR=${BUILD}/lib/openvanilla ./configure --prefix=${BUILD}
	make distcheck && ls -t ucimf-openvanilla*.tar.gz | head -n1 | xargs cp -t ${TARBALL}
}

make_tarball_of_jfbterm(){
	echo "Start to make tarball of jfbterm..."
	cd ${CONSOLEJ}
	./init.sh
	cd jfbterm-0.4.7/
	test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	fakeroot make distcheck && ls -t jfbterm*.tar.gz | head -n1 | xargs cp -t ${TARBALL}
}

make_tarball_of_fbterm-ucimf(){
	echo "Start to make tarball of fbterm-ucimf..."
	cd ${FBTERMUCIMF}
	./autogen.sh
	test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	make distcheck && ls -t fbterm_ucimf*.tar.gz | head -n1 | xargs cp -t ${TARBALL}
}

make_tarball_of_fbterm-ucimf(){
	echo "Start to make tarball of fbterm-ucimf..."
	cd ${FBTERMUCIMF}
	./autogen.sh
	test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	make distcheck && ls -t fbterm_ucimf*.tar.gz | head -n1 | xargs cp -t ${TARBALL}
}
