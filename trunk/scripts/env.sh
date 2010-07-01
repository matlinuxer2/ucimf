#!/bin/bash

if [ "x$ROOT" = "x" ];
then
	echo ""
	echo "Shell variable \$ROOT is not set. Please set variable like"
	echo "    PATH=\$PATH:\"path to the repository trunk\""
	echo ""
fi

SCRIPTS=${ROOT}/scripts/
BUILD=$ROOT/build
TARBALL=${ROOT}/tarball/

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
export CPATH=${CPATH}:${BUILD}/include/
export PATH=${PATH}:${BUILD}/bin/
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${BUILD}/lib/pkgconfig/

unset_env (){
	unset LIBRARY_PATH
	unset LD_LIBRARY_PATH
	unset LD_RUN_PATH
	unset CPATH
	unset PATH
	unset PKG_CONFIG_PATH
}
