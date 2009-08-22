#!/bin/bash

ROOT="$( dirname $(echo $0))/.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh

build_clean(){
	pushd .

	test -d ${BUILD} && rm -rvf ${BUILD}
	mkdir -p ${BUILD}
	
	popd
}

build_libucimf(){
	pushd .

	echo "Start to build libucimf"
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	cd ${LIBUCIMF}
	./autogen.sh
	test -f Makefile && make distclean
	./configure --prefix=${BUILD} && make && make install
	sed -i "s@/usr@${BUILD}@g" ${BUILD}/etc/ucimf.conf

	popd
}

check_libucimf(){
	pushd .

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

	popd
}

setup_libucimf(){
	pushd .

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

	popd
}



build_ucimf-openvanilla(){
	pushd .

	echo "Start to build ucimf-openvanilla"
	cd ${UCIMFOV}
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	./autogen.sh
	test -f Makefile && make distclean
	./configure --prefix=${BUILD} && make &&  make install

	popd
}

build_openvanilla(){
	pushd .

	echo "Start to build openvanilla"
	cd ${OV}
	test -L openvanilla-modules || ./init.sh
	cd openvanilla-modules
	test -f Makefile && make distclean
	./configure --prefix=${BUILD} && make && make install

	popd
}

build_console_fbterm(){
	pushd .

	echo "Start to build fbterm"
	cd ${CONSOLE}
	FBTERM_FILE="fbterm-1.4.tar.gz"
	if [ ! -f "$FBTERM_FILE" ]; then
		wget --continue http://fbterm.googlecode.com/files/fbterm-1.4.tar.gz
	fi
	tar xzf ${FBTERM_FILE}
	cd fbterm-1.4/
	#    LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
	./configure --prefix=${BUILD}
	make
	fakeroot make install

	popd
}

build_fbterm-ucimf(){
	pushd .

	echo "Start to make tarball of fbterm-ucimf..."
	cd ${FBTERMUCIMF}
	autoreconf -sif
	test -f Makefile && make distclean
	./configure --prefix=${BUILD} && make && make install

	popd
}

build_console_jfbterm(){
	pushd .

	echo "Start to build jfbterm"
	cd ${CONSOLEJ}
	./init.sh
	cd jfbterm-0.4.7/
	test -f Makefile && make distclean
	LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
	make
	make install

	popd
}

build_console_testing(){
	pushd .

	echo "Start to build testing"
	cd ${DUMMY}
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	autoreconf -i --force
	test -f Makefile && make distclean
	LIBS="-L${BUILD}/lib/" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
	make
	make install

	popd
}


#read -t 10 -p "Choose console: [f(bterm)/j(fbterm)]" myconsole
#
#if [[ $myconsole == "j" ]]
#then
#  echo "Build jfbterm as console program"
#else
#  echo "Build FbTerm as console program"
#fi

build_clean

build_libucimf
build_ucimf-openvanilla
build_openvanilla

build_fbterm-ucimf
build_console_fbterm
build_console_testing

sudo chown root ${BUILD}/bin/fbterm
sudo chmod u+s ${BUILD}/bin/fbterm

#if [[ $myconsole == "j" ]]
#then
#  echo "Build jfbterm as console program"
#  build_console_jfbterm
#else
#  echo "Build FbTerm as console program"
#  build_console_fbterm
#fi
