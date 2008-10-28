#!/bin/bash

OV_SVN_URL="http://openvanilla.googlecode.com/svn/trunk"
ROOT=$(pwd)

ov_common_unpack() {
    # Fetch svn nessesary source.
    test -d ${ROOT}/Modules || mkdir ${ROOT}/Modules
    cd ${ROOT}/Modules
    echo "Checkout common source"
    svn checkout ${OV_SVN_URL}/Modules/Mk                                                          
    svn checkout ${OV_SVN_URL}/Modules/SharedHeaders 
    svn checkout ${OV_SVN_URL}/Modules/SharedSource 
}

ov_phonetic_unpack() {
    # Fetch module on demand
    test -d ${ROOT}/Modules || mkdir ${ROOT}/Modules
    cd ${ROOT}/Modules
    echo "Checkout phonetic source"
    svn checkout ${OV_SVN_URL}/Modules/OVIMPhonetic 
    
    test -d ${ROOT}/Modules/SharedData || mkdir ${ROOT}/Modules/SharedData
    cd ${ROOT}/Modules/SharedData
    wget --continue ${OV_SVN_URL}/Modules/SharedData/bpmf.cin 
    wget --continue ${OV_SVN_URL}/Modules/SharedData/bpmf-ext.cin 
    wget --continue ${OV_SVN_URL}/Modules/SharedData/punctuations.cin
}


ov_common_unpack
ov_phonetic_unpack
