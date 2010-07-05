#!/bin/bash


function update_launchpad_file(){
	local THE_BRANCH=$1
	pushd . 
	cd $THE_BRANCH 
		bzr merge lp:~pkg-ime/ucimf/$THE_BRANCH
	popd
}

function update_with_debian_official(){
	local debian_dir=$1
	local debian_url=$2
	local debian_tgz=$(basename $2)

	pushd . ; cd $debian_dir
	wget --continue $debian_url
	tar -zxvf $debian_tgz
	popd

}

update_launchpad_file debian-fbterm-ucimf 
update_launchpad_file debian-libucimf 
update_launchpad_file debian-openvanilla-modules 
update_launchpad_file debian-ucimf-openvanilla 

update_with_debian_official debian-fbterm-ucimf http://ftp.de.debian.org/debian/pool/main/f/fbterm-ucimf/fbterm-ucimf_0.2.8-1.debian.tar.gz
update_with_debian_official debian-libucimf http://ftp.de.debian.org/debian/pool/main/libu/libucimf/libucimf_2.3.5-1.debian.tar.gz
update_with_debian_official debian-openvanilla-modules http://ftp.de.debian.org/debian/pool/main/o/openvanilla-modules/openvanilla-modules_0.8.0.14+dfsg1-1.debian.tar.gz
update_with_debian_official debian-ucimf-openvanilla http://ftp.de.debian.org/debian/pool/main/u/ucimf-openvanilla/ucimf-openvanilla_2.10.6-1.debian.tar.gz
