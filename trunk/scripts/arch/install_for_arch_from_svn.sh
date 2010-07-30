#!/bin/bash

ROOT="$( readlink -f $(dirname $0)/../..)" 
SCRIPTS_ARCH="$ROOT/scripts/arch/"


fetch_file_from () {
	local NAME="$1"
	local URL="$2"

	if [ ! -f $NAME ]; then
		wget -O "$NAME" "$URL"
	fi
}

make_pkgbuild_from_aur () {
	local PKGNAME="$1"

	install -d "$SCRIPTS_ARCH/$PKGNAME"

	pushd . ; cd "$SCRIPTS_ARCH/$PKGNAME"
		fetch_file_from PKGBUILD http://aur.archlinux.org/packages/$PKGNAME/$PKGNAME/PKGBUILD
		makepkg -i
	popd
}

make_pkgbuild_from_aur libucimf-svn
make_pkgbuild_from_aur ucimf-openvanilla-svn
make_pkgbuild_from_aur openvanilla-modules-svn
make_pkgbuild_from_aur fbterm-ucimf-svn
make_pkgbuild_from_aur fbterm

pacman -Q |egrep "ucimf|openvanilla|fbterm"
