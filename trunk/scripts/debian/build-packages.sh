#!/bin/bash


function fetch_launchpad_file(){
	local THE_BRANCH=$1
	#test -d $THE_BRANCH || bzr branch lp:~pkg-ime/ucimf/$THE_BRANCH
	test -d $THE_BRANCH || bzr branch lp:~matlinuxer2/ucimf/$THE_BRANCH
}

function fetch_googlecode_file(){
	local THE_TARBALL=$1
	test -f $THE_TARBALL || wget --continue http://ucimf.googlecode.com/files/$THE_TARBALL
}

function find_build_dep(){
	local SEARCH_DIRS=$( echo $@ )
	if [ $# -gt 0 ]
        then
	    echo "There are build dependencies below: "
	    echo ""
	    find $SEARCH_DIRS -name 'control' | xargs grep "Build-Depends"| cut -f3- -d: | sed -e "s/(.*)//g" -e "s/,/\n/g" | sort | uniq | xargs echo -n
	    echo ""
	    echo ""
	fi

}

function time_count {
	local COUNT=$1

	for (( x=$COUNT; $x > 0 ; x-- ))
	do
		echo -n "$x..."
		sleep 1
	done
}

#### Fetching related source files ####
####
#### Thanks to Aron Xu's debianization works!
####
fetch_launchpad_file debian-ucimf-openvanilla 
fetch_launchpad_file debian-openvanilla-modules 
fetch_launchpad_file debian-libucimf 
fetch_launchpad_file debian-fbterm-ucimf 

fetch_googlecode_file libucimf-2.3.1.tar.gz
fetch_googlecode_file ucimf-openvanilla-2.10.7.tar.gz
fetch_googlecode_file openvanilla-modules-0.8.0_14.tar.gz
fetch_googlecode_file fbterm_ucimf-0.2.7.tar.gz

#### Output possible build dependencies ####
find_build_dep debian-ucimf-openvanilla debian-openvanilla-modules debian-libucimf debian-fbterm-ucimf 
time_count 5

#### Make debian packages and merge them into repository tre ####
TEMP_DIR="incoming"

#### http://code.google.com/p/pkgbzr/
#### debbing is a debian packaging tool based on debuild and ppa_publish ####
debbing libucimf-2.3.1.tar.gz debian-libucimf/debian $TEMP_DIR 
debbing ucimf-openvanilla-2.10.7.tar.gz debian-ucimf-openvanilla/debian $TEMP_DIR 
debbing openvanilla-modules-0.8.0_14.tar.gz debian-openvanilla-modules/debian $TEMP_DIR 
debbing fbterm_ucimf-0.2.7.tar.gz debian-fbterm-ucimf/debian $TEMP_DIR 

rm -r __debian*
