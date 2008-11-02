#!/bin/bash

ROOT=$( dirname $(pwd) )
LIBUCIMF=${ROOT}/libucimf/
UCIMFOV=${ROOT}/ucimf-openvanilla/
OV=${ROOT}/openvanilla/
CONSOLE=${ROOT}/console/fbterm/
BUILD=${ROOT}/build/

export LIBRARY_PATH=${BUILD}/lib/
export LD_LIBRARY_PATH=${BUILD}/lib/
export LD_RUN_PATH=${BUILD}/lib/
export PATH=${PATH}:${BUILD}/bin/

build_libucimf(){
	echo "Start to build libucimf"
	cd ${LIBUCIMF}
	autoreconf -i --force
	UCIMF_CONF_PATH=${BUILD}/etc/ ./configure --prefix=${BUILD}
	make
	make install
}

check_libucimf(){
	# 檢查設定檔
	echo "系統設定檔:"
	FILE=/etc/ucimf.conf
	test -f ${FILE} \
	&& ls -l ${FILE} \
	&& cat ${FILE} |grep UCIMF_FONTPATH \
	&& cat ${FILE} |grep IMF_MODULE_DIR
	echo "個人設定檔:"
	FILE=${HOME}/.ucimf.conf
	test -f ${FILE} \
	&& ls -l ${FILE} \
	&& cat ${FILE} |grep UCIMF_FONTPATH \
	&& cat ${FILE} |grep IMF_MODULE_DIR
	
}

setup_libucimf(){
	FILE=${BUILD}/etc/ucimf.conf
	#cat ${FILE} | sed 's/.*UCIMF_FONTPATH.*=.*//g' | sed 's/.*IMF_MODULE_DIR.*=.*//g' 
	sed -i 's/^.*UCIMF_FONTPATH.*$//g' ${FILE}
	sed -i 's/^.*IMF_MODULE_DIR.*$//g' ${FILE}
	sed -i "1a UCIMF_FONTPATH = ${BUILD}/share/ucimf/unifont.pcf" ${FILE}
	sed -i "1a IMF_MODULE_DIR = ${BUILD}/lib/ucimf/" ${FILE}

	echo "測試設定檔:"
	FILE=${BUILD}/etc/ucimf.conf
	test -f ${FILE} \
	&& ls -l ${FILE} \
	&& cat ${FILE} |grep UCIMF_FONTPATH| sed 's/^.*=//g' \
	&& cat ${FILE} |grep IMF_MODULE_DIR| sed 's/^.*=//g'

	echo "檢查字型檔:"
	FONTFILE=$(cat ${FILE} |grep UCIMF_FONTPATH| sed 's/^.*=//g')
	ls -l ${FONTFILE}
}

build_ucimf-openvanilla(){
	echo "Start to build ucimf-openvanilla"
	cd ${UCIMFOV}
	autoreconf -i --force
	OV_MODULEDIR=${BUILD}/lib/openvanilla ./configure --prefix=${BUILD}
	make
	make install
}

build_openvanilla(){
	echo "Start to build openvanilla"
	cd ${OV}
	./init.sh
	cd Modules/
	make
	make install INSTALL_PREFIX=${BUILD}
}

build_console_fbterm(){
	echo "Start to build fbterm"
	cd ${CONSOLE}
	./init.sh
	cd fbterm-1.2/
	LDFLAGS="-L${BUILD}/lib" LIBS="-lucimf" CPPFLAGS="-I${BUILD}/include" ./configure --prefix=${BUILD} 
	make
	make install
}


try(){
	LC_CTYPE=zh_TW.UTF-8 ${BUILD}/bin/fbterm
}

#build_libucimf
#setup_libucimf
#build_ucimf-openvanilla
#build_openvanilla
#build_console_fbterm
try
