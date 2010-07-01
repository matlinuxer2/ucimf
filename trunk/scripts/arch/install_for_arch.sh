#!/bin/bash

SCRIPTS=$(pwd)
PKGBUILD=${SCRIPTS}/pkgbuilds/
test -d ${PKGBUILD} || mkdir -p ${PKGBUILD}

back_to_scripts(){
	cd ${SCRIPTS}
}

make_pkgbuild_of_libucimf-svn(){
	echo "Start to make pkgbuild of libucimf..."
	cd ${PKGBUILD}
	test -f PKGBUILD_libucimf || wget -O PKGBUILD_libucimf http://aur.archlinux.org/packages/libucimf-svn/libucimf-svn/PKGBUILD
	makepkg -ip PKGBUILD_libucimf

	back_to_scripts
}

make_pkgbuild_of_ucimf-openvanilla-svn(){
	echo "Start to make pkgbuild of ucimf-openvanilla..."
	cd ${PKGBUILD}
	test -f PKGBUILD_ucimf-openvanilla || wget -O PKGBUILD_ucimf-openvanilla http://aur.archlinux.org/packages/ucimf-openvanilla-svn/ucimf-openvanilla-svn/PKGBUILD
	makepkg -ip PKGBUILD_ucimf-openvanilla

	back_to_scripts
}

make_pkgbuild_of_openvanilla-svn(){
	echo "Start to make pkgbuild of openvanilla..."
	cd ${PKGBUILD}
	test -f PKGBUILD_openvanilla || wget -O PKGBUILD_openvanilla http://aur.archlinux.org/packages/openvanilla-svn/openvanilla-svn/PKGBUILD
	makepkg -ip PKGBUILD_openvanilla

	back_to_scripts
}

make_pkgbuild_of_fbterm-ucimf-svn(){
	echo "Start to make pkgbuild of fbterm-ucimf..."
	cd ${PKGBUILD}
	test -f PKGBUILD_fbterm-ucimf || wget -O PKGBUILD_fbterm-ucimf http://aur.archlinux.org/packages/ucimf-fbterm-svn/ucimf-fbterm-svn/PKGBUILD
	makepkg -ip PKGBUILD_fbterm-ucimf

	back_to_scripts
}

make_pkgbuild_of_fbterm(){
	echo "Start to make pkgbuild of fbterm"
	cd ${PKGBUILD}
	yaourt --noconfirm --sync fbterm
	#test -f PKGBUILD_fbterm || wget -O PKGBUILD_fbterm http://aur.archlinux.org/packages/fbterm/fbterm/PKGBUILD
	#makepkg -ip PKGBUILD_fbterm

	back_to_scripts
}


# commands
make_pkgbuild_of_libucimf-svn
make_pkgbuild_of_ucimf-openvanilla-svn
make_pkgbuild_of_openvanilla-svn
make_pkgbuild_of_fbterm-ucimf-svn
make_pkgbuild_of_fbterm

pacman -Q |grep libucimf
pacman -Q |grep ucimf-openvanilla
pacman -Q |grep openvanilla
pacman -Q |grep fbterm-ucimf
pacman -Q |grep fbterm
