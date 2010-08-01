#!/bin/bash

ROOT="$( readlink -f $(dirname $0)/../..)" 
SCRIPTS_ARCH="$ROOT/scripts/arch/"
SCRIPTS_ARCH_SUBMIT="$SCRIPTS_ARCH/submit/"


fetch_file_from () {
	local NAME="$1"
	local URL="$2"

	if [ ! -f $NAME ]; then
		wget -O "$NAME" "$URL"
	fi
}

fetch_pkgbuild_from_aur () {
	local PKGNAME="$1"

	install -d "$SCRIPTS_ARCH_SUBMIT"

	pushd . ; cd "$SCRIPTS_ARCH_SUBMIT"
		fetch_file_from PKGBUILD-$PKGNAME.orig http://aur.archlinux.org/packages/$PKGNAME/$PKGNAME/PKGBUILD
	popd
}

copy_pkgbuild_from_local () {
	local PKGNAME="$1"

	install -d "$SCRIPTS_ARCH_SUBMIT"

	pushd . ; cd "$SCRIPTS_ARCH_SUBMIT"
		local TARGET_FILE="$SCRIPTS_ARCH/$PKGNAME/PKGBUILD"
		test -f $TARGET_FILE && cp -v $TARGET_FILE PKGBUILD-$PKGNAME 
	popd
}

fetch_and_diff () {
	local PKGNAME="$1"
	fetch_pkgbuild_from_aur $PKGNAME
	copy_pkgbuild_from_local $PKGNAME

	install -d "$SCRIPTS_ARCH_SUBMIT"

	pushd . ; cd "$SCRIPTS_ARCH_SUBMIT"
		diff -Naur PKGBUILD-$PKGNAME.orig PKGBUILD-$PKGNAME > PKGBUILD-$PKGNAME.diff
	popd
}

fetch_and_diff libucimf
fetch_and_diff ucimf-openvanilla 
fetch_and_diff openvanilla-modules
fetch_and_diff fbterm-ucimf
	
fetch_and_diff libucimf-svn
fetch_and_diff ucimf-openvanilla-svn
fetch_and_diff fbterm-ucimf-svn
