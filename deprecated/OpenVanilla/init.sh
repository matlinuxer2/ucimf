#!/bin/bash

OV_SVN_URL="http://openvanilla.googlecode.com/svn/trunk"
ROOT=$(pwd)

ov_common_unpack() {
	# Fetch svn nessesary source.

	test -d ${ROOT}/Modules || mkdir ${ROOT}/Modules
	cd ${ROOT}/Modules
	echo "Checkout common source"
	svn checkout ${OV_SVN_URL}/Modules/SharedHeaders 
	svn checkout ${OV_SVN_URL}/Modules/SharedSource 

	cd ${ROOT}
	svn checkout ${OV_SVN_URL}/Framework

	test -d ${ROOT}/Modules/SharedHeaders/OpenVanilla || mkdir ${ROOT}/Modules/SharedHeaders/OpenVanilla 
	cp -a ${ROOT}/Framework/Headers/*.h ${ROOT}/Modules/SharedHeaders/OpenVanilla/
}

ov_array_unpack() {
	# Fetch module on demand
	test -d ${ROOT}/Modules || mkdir ${ROOT}/Modules
	cd ${ROOT}/Modules
	echo "Checkout phonetic source"
	svn checkout ${OV_SVN_URL}/Modules/OVIMArray

	test -d ${ROOT}/Modules/SharedData || mkdir ${ROOT}/Modules/SharedData
	cd ${ROOT}/Modules/SharedData
	wget -nc ${OV_SVN_URL}/Modules/SharedData/array-shortcode.cin 
	wget -nc ${OV_SVN_URL}/Modules/SharedData/array-special.cin
	wget -nc ${OV_SVN_URL}/Modules/SharedData/array30.cin
}

ov_generic_unpack() {
	# Fetch module on demand
	test -d ${ROOT}/Modules || mkdir ${ROOT}/Modules
	cd ${ROOT}/Modules
	echo "Checkout phonetic source"
	svn checkout ${OV_SVN_URL}/Modules/OVIMGeneric

	test -d ${ROOT}/Modules/SharedData || mkdir ${ROOT}/Modules/SharedData
	cd ${ROOT}/Modules/SharedData
	wget -nc ${OV_SVN_URL}/Modules/SharedData/cj.cin
	wget -nc ${OV_SVN_URL}/Modules/SharedData/dayi3.cin
	wget -nc ${OV_SVN_URL}/Modules/SharedData/ehq-symbols.cin
	wget -nc ${OV_SVN_URL}/Modules/SharedData/pinyin.cin
	wget -nc ${OV_SVN_URL}/Modules/SharedData/simplex.cin
	wget -nc ${OV_SVN_URL}/Modules/SharedData/wubizixing.cin
}


ov_phonetic_unpack() {
	# Fetch module on demand
	test -d ${ROOT}/Modules || mkdir ${ROOT}/Modules
	cd ${ROOT}/Modules
	echo "Checkout phonetic source"
	svn checkout ${OV_SVN_URL}/Modules/OVIMPhonetic 

	test -d ${ROOT}/Modules/SharedData || mkdir ${ROOT}/Modules/SharedData
	cd ${ROOT}/Modules/SharedData
	wget -nc ${OV_SVN_URL}/Modules/SharedData/bpmf.cin 
	wget -nc ${OV_SVN_URL}/Modules/SharedData/bpmf-ext.cin 
	wget -nc ${OV_SVN_URL}/Modules/SharedData/punctuations.cin
}

ov_poj_unpack() {
	# Fetch module on demand
	test -d ${ROOT}/Modules || mkdir ${ROOT}/Modules
	cd ${ROOT}/Modules
	echo "Checkout phonetic source"
	svn checkout ${OV_SVN_URL}/Modules/OVIMPOJ-Holo

	test -d ${ROOT}/Modules/SharedData || mkdir ${ROOT}/Modules/SharedData
	cd ${ROOT}/Modules/SharedData
	wget -nc ${OV_SVN_URL}/Modules/SharedData/poj-holo.cin
}

ov_patches(){
	cd ${ROOT}
	cp -v patches/configure.ac Modules/
	SRC="patches/"
	DST="Modules/"
	sync_makefile_am
}

sync_makefile_am(){
	cd $ROOT
	test -d $DST || mkdir -p $DST
	for make_am in `find $SRC -name 'Makefile.am'`
	do
		dir=`dirname ${make_am#${SRC}}`
		test -d ${DST}/${dir} || mkdir -p ${DST}/${dir}
		cp -v $make_am ${DST}/${dir}
	done
}

ov_autotoolize()
{
	cd ${ROOT}/Modules
	autoreconf -i --force
}

ov_mods_build()
{
	cd ${ROOT}/Modules
	./configure --prefix=${ROOT}/openvanilla && make && make install 
	make distcheck && mv -v *.tar.gz  ${ROOT}
	
	cd ${ROOT}
	tar -cjf openvanilla-modules-0.8.0.tar.bz2 openvanilla/
	rm -rf openvanilla/
}

ov_common_unpack
#ov_array_unpack   # failed to build
ov_generic_unpack
ov_phonetic_unpack
#ov_poj_unpack
ov_patches
ov_autotoolize
ov_mods_build
