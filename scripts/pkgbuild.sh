#!/bin/bash
source ./inc.sh

test -d ${PKGBUILD} && rm -rvf ${PKGBUILD} 
mkdir -p ${PKGBUILD}
# first
make_pkgbuild_of_libucimf-svn
make_pkgbuild_of_ucimf-openvanilla-svn
# second
make_pkgbuild_of_fbterm-ucimf-svn
# third
#make_pkgbuild_of_openvanilla-svn

