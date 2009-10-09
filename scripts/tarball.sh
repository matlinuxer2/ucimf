#!/bin/bash

ROOT="$( dirname $(echo $0))/.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh


test -d ${TARBALL} && rm -rvf ${TARBALL} 
install -d ${TARBALL}

make_tarball_of_libucimf(){
	pushd .

	echo "Start to make tarball of libucimf..."
	cd ${LIBUCIMF}
	autoreconf -sif
	test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
	make distcheck && ls -t libucimf*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	popd
}

make_tarball_of_ucimf-openvanilla(){
	pushd .

	echo "Start to make tarball of ucimf-openvanilla..."
	cd ${UCIMFOV}
	autoreconf -sif
	test -f configure && OV_MODULEDIR=${BUILD}/lib/openvanilla ./configure --prefix=${BUILD}
	make distcheck && ls -t ucimf-openvanilla*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	popd
}

make_tarball_of_openvanilla(){
	pushd .

	echo "Start to make tarball of openvanilla..."
	cd ${OV}
	test -L openvanilla-modules || ./init.sh
	cd openvanilla-modules
	test -f configure && ./configure --prefix=${BUILD}
	make distcheck && ls -t openvanilla*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	popd
}

make_tarball_of_jfbterm(){
	pushd .

	echo "Start to make tarball of jfbterm..."
	cd ${CONSOLEJ}
	./init.sh
	cd jfbterm-0.4.7/
	test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
	fakeroot make distcheck && ls -t jfbterm*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	popd
}

make_tarball_of_fbterm(){
	pushd .

	cd ${TARBALL}

	FBTERM_FILE=$( ls -t fbterm*.tar.gz|grep -v 'ucimf'| head --lines=1 )
	test -f "$FBTERM_FILE" || wget --continue http://fbterm.googlecode.com/files/fbterm-1.4.tar.gz

	popd
}

make_tarball_of_fbterm-ucimf(){
	pushd .

	echo "Start to make tarball of fbterm-ucimf..."
	cd ${FBTERMUCIMF}
	autoreconf -sif
	test -f configure && LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD}
	make distcheck && ls -t fbterm_ucimf*.tar.gz | head -n1 | xargs cp -t ${TARBALL}

	popd
}

# first
make_tarball_of_libucimf
make_tarball_of_ucimf-openvanilla
# second
make_tarball_of_openvanilla
## third
make_tarball_of_fbterm-ucimf
make_tarball_of_fbterm
make_tarball_of_jfbterm

