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
	test -f ./configure || autoreconf -sif || return 1
	test -f Makefile && make distclean 
	( ./configure --prefix=${BUILD} && make && make install ) || return 1
	sed -i "s@/usr@${BUILD}@g" ${BUILD}/etc/ucimf.conf

	popd
}

build_ucimf-openvanilla(){
	pushd .

	echo "Start to build ucimf-openvanilla"
	cd ${UCIMFOV}
	echo "Autotoolizing is somewhat slow, please wait a moment...:-)"
	test -f ./configure || autoreconf -sif || return 1
	test -f Makefile && make distclean
	( ./configure --prefix=${BUILD} && make && make install ) || return 1

	popd
}

build_openvanilla(){
	pushd .

	echo "Start to build openvanilla"
	cd ${OV}
	test -L openvanilla-modules || ./init.sh
	cd openvanilla-modules
	test -f Makefile && make distclean
	( ./configure --prefix=${BUILD} && make && make install ) || return 1

	popd
}

build_console_fbterm(){
	pushd .

	echo "Start to build fbterm"
	cd ${CONSOLE}
	FBTERM_FILE="fbterm-1.5.tar.gz"
	if [ ! -f "$FBTERM_FILE" ]; then
		wget --continue http://fbterm.googlecode.com/files/fbterm-1.5.tar.gz
	fi
	tar xzf ${FBTERM_FILE}
	cd fbterm-1.5/
	#    LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
	./configure --prefix=${BUILD} || return 1
	make || return 1
	fakeroot make install || return 1

	popd
}

build_fbterm-ucimf(){
	pushd .

	echo "Start to make tarball of fbterm-ucimf..."
	cd ${FBTERMUCIMF}
	test -f ./configure || autoreconf -sif || return 1
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
	test -f ./configure || autoreconf -sif || return 1
	test -f Makefile && make distclean
	LIBS="-L${BUILD}/lib/" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} || return 1
	make || return 1
	make install || return 1

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

build_libucimf || exit 
build_ucimf-openvanilla || exit 
build_openvanilla || exit 

build_fbterm-ucimf || exit
build_console_fbterm || exit

sudo chown root ${BUILD}/bin/fbterm
sudo chmod u+s ${BUILD}/bin/fbterm

build_console_testing

