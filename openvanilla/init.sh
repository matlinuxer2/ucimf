#!/bin/bash

OV_SVN_URL="http://openvanilla.googlecode.com/svn/trunk"
ROOT=$(pwd)

ov_common_unpack() {
	# Fetch svn nessesary source.

	test -d ${ROOT}/Modules || mkdir ${ROOT}/Modules
	cd ${ROOT}/Modules
	echo "Checkout common source"
	wget --continue ${OV_SVN_URL}/Modules/Makefile                                                         
	svn checkout ${OV_SVN_URL}/Modules/Mk                                                          
	svn checkout ${OV_SVN_URL}/Modules/SharedHeaders 
	svn checkout ${OV_SVN_URL}/Modules/SharedSource 

	cd ${ROOT}
	svn checkout ${OV_SVN_URL}/Framework

	test -d ${ROOT}/Modules/SharedHeaders/OpenVanilla  || ln -s ${ROOT}/Framework/Headers ${ROOT}/Modules/SharedHeaders/OpenVanilla
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
	patch -p0 < patches/module_makefile.patch
}


ov_common_unpack
ov_array_unpack
ov_generic_unpack
ov_phonetic_unpack
ov_poj_unpack
ov_patches
