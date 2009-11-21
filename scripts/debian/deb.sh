#!/bin/bash

get_gpg_signname(){
	result=$( gpg --list-secret-keys| grep uid | head --lines=1 | cut -c 22- )
	echo $result
}

get_fullname(){
	result=$( get_gpg_signname | sed 's/\(.*\)<.*>/\1/' )
	echo $result
}

get_email(){
	result=$( get_gpg_signname | sed 's/.*<\(.*\)>/\1/' )
	echo $result
}

# get package name from configure.ac
# example: `get_packagename ./configure.ac`
get_packagename(){
        result=$( cat $1 | awk  '/AC_INIT/{ split( $0,A,",");print A[1] }'| sed -e 's/AC_INIT//' | tr -d '(' | tr -d ',' | tr -d '[' | tr -d ']' | tr -d ' ' )
	result=$( echo $result | tr '_' '-' )
	echo $result
}

# get package version from configure.ac
# example: `get_packageversion ./configure.ac`
get_packageversion(){
        result=$( cat $1 | awk  '/AC_INIT/{ split( $0,A,",");print A[2] }'| sed -e 's/AC_INIT//' | tr -d '(' | tr -d ',' | tr -d '[' | tr -d ']' | tr -d ' ' )
	result=$( echo $result | tr '_' '-' )
	echo $result
}


# get tarball unpacked dir
# example: get_tarball_dir dummy-0.1.2.tar.gz 
get_tarball_dir(){
	result=$( tar -tf $1 | head --lines=1 )
	echo $result
}



main(){
	tarball=$( realpath $1 )
	topdir=$(get_tarball_dir $tarball )

	tar -zxf $tarball
	packagename=$( get_packagename $topdir/configure.ac )
	packageversion=$( get_packageversion $topdir/configure.ac )
	echo $packagename
	echo $packageversion

	pushd .
	cd $topdir
		dh_make --single \
			--cdbs \
			--createorig \
			--packagename "${packagename}_${packageversion}" \
			--copyright gpl2 \
			--file $tarball

		pushd .
		cd debian
		rm *.ex *.EX compat copyright docs README.Debian  README.source 
		vim changelog
		vim control
		popd
	popd
}


export DEBFULLNAME=$(get_fullname) 
export DEBEMAIL=$(get_email)

main $1
