#!/bin/bash
source ./inc.sh

test -d ${TARBALL} && rm -rvf ${TARBALL} 
mkdir -p ${TARBALL}
# first
make_tarball_of_libucimf
make_tarball_of_ucimf-openvanilla
# second
make_tarball_of_jfbterm
make_tarball_of_fbterm-ucimf
## third
#make_tarball_of_openvanilla

