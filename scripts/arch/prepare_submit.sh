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
		# 把在 AUR 的 PKGBUILD 下載下來作比對
		fetch_file_from PKGBUILD-$PKGNAME.orig http://aur.archlinux.org/packages/$PKGNAME/PKGBUILD
	popd
}

copy_pkgbuild_from_local () {
	local PKGNAME="$1"

	install -d "$SCRIPTS_ARCH_SUBMIT"

	# prepare source package for upload
	pushd . ; cd "$SCRIPTS_ARCH/$PKGNAME"
		makepkg --source --force
	popd

	pushd . ; cd "$SCRIPTS_ARCH_SUBMIT"
		local TARGET_FILE="$SCRIPTS_ARCH/$PKGNAME/PKGBUILD"
		local TARGET_FILE2=$( find $SCRIPTS_ARCH/$PKGNAME -name "$PKGNAME-*.src.tar.gz" | xargs ls -t | head -n 1 )
		test -f $TARGET_FILE && cp -v $TARGET_FILE PKGBUILD-$PKGNAME 
		test -f $TARGET_FILE2 &&  cp -v $TARGET_FILE2 .
	popd
}

fetch_and_diff () {
	local PKGNAME="$1"
	fetch_pkgbuild_from_aur $PKGNAME
	copy_pkgbuild_from_local $PKGNAME

	install -d "$SCRIPTS_ARCH_SUBMIT"

	pushd . ; cd "$SCRIPTS_ARCH_SUBMIT"
		# 比本地端的 PKGBUILD 和線上的不同之處
		diff -Naur PKGBUILD-$PKGNAME.orig PKGBUILD-$PKGNAME > PKGBUILD-$PKGNAME.diff
	popd
}

show_upload_hint () {
	install -d "$SCRIPTS_ARCH_SUBMIT"

	echo ""
	echo "Difference between local and online PKGBUILD: "
	echo ""
	for x in `find $SCRIPTS_ARCH_SUBMIT -type f -name '*.diff'`
	do 
		if [ -s "$x" ]
		then 
			echo "$x" 
		fi
	done

	echo ""
	echo "Upload helper scripts below: "
	echo ""
	for x in `find $SCRIPTS_ARCH_SUBMIT -type f -name '*.src.tar.gz'`
	do 
		echo "aurploader -k -a $x"
	done

}

fetch_and_diff libucimf
fetch_and_diff ucimf-openvanilla 
fetch_and_diff openvanilla-modules
fetch_and_diff fbterm-ucimf
	
fetch_and_diff libucimf-svn
fetch_and_diff ucimf-openvanilla-svn
fetch_and_diff fbterm-ucimf-svn

show_upload_hint
