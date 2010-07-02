#!/bin/bash

if [ "x$(basename $0)" = "xenv.sh" ]; then
  ROOT="$( readlink -f $(dirname $0)/..)" 
fi

if [ "x$ROOT" = "x" ];
then
	echo ""
	echo "Shell variable \$ROOT is not set. Please set variable like"
	echo "    PATH=\$PATH:\"path to the repository trunk\""
	echo ""
fi

SCRIPTS=${ROOT}/scripts/
BUILD=${SCRIPTS}/_build
TARBALL=${SCRIPTS}/_tarball/

# package's directory
LIBUCIMF=${ROOT}/libucimf/
UCIMFOV=${ROOT}/ucimf-openvanilla/
FBTERMUCIMF=${ROOT}/console/fbterm_ucimf/

set_env () {
	export LIBRARY_PATH=${BUILD}/lib/
	export LD_LIBRARY_PATH=${BUILD}/lib/
	export LD_RUN_PATH=${BUILD}/lib/
	export CPATH=${CPATH}:${BUILD}/include/
	export PATH=${PATH}:${BUILD}/bin/
	export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${BUILD}/lib/pkgconfig/
}

unset_env (){
	unset LIBRARY_PATH
	unset LD_LIBRARY_PATH
	unset LD_RUN_PATH
	unset CPATH
	unset PATH
	unset PKG_CONFIG_PATH
}

if [ "x$(basename $0)" = "xenv.sh" ]; then
	LIBRARY_PATH=${BUILD}/lib/ \
	LD_LIBRARY_PATH=${BUILD}/lib/ \
	LD_RUN_PATH=${BUILD}/lib/ \
	CPATH=${BUILD}/include/:${CPATH} \
	PATH=${BUILD}/bin/:$PATH \
	PKG_CONFIG_PATH=${BUILD}/lib/pkgconfig/:${PKG_CONFIG_PATH}\
	PS1="ucimf_devel_shell>> " bash --norc
else
	set_env
fi
