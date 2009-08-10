#!/bin/bash

ROOT=$( dirname $(pwd) )
SCRIPTS=${ROOT}/scripts/
BUILD=${ROOT}/build/
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

