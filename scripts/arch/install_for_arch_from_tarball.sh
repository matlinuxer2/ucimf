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
		fetch_file_from PKGBUILD http://aur.archlinux.org/packages/$PKGNAME/PKGBUILD
		makepkg -i --noconfirm
		makepkg -f --source 
	popd
}

( pacman -Q | egrep "^fbterm " >/dev/null ) || yaourt -S --noconfirm fbterm
make_pkgbuild_from_aur libucimf
make_pkgbuild_from_aur openvanilla-modules
make_pkgbuild_from_aur ucimf-openvanilla
# dependencies: yaourt intltool git scons sunpinyin-git
make_pkgbuild_from_aur ucimf-sunpinyin
make_pkgbuild_from_aur fbterm-ucimf

pacman -Q |egrep "ucimf|openvanilla|fbterm"
