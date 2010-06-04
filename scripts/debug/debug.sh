#!/bin/bash

ROOT="$( dirname $( readlink -f $0))/../.."
source $ROOT/scripts/env.sh

ERROR_LOG=${BUILD}/error.log

rm ${ERROR_LOG}

echo "===Check SVN Revision===" | tee -a ${ERROR_LOG}
svn info | grep 'Revision' >> ${ERROR_LOG}

echo "===Dump enviroment variables===" | tee -a ${ERROR_LOG}
env >> ${ERROR_LOG}

echo "===Check Overlay Files===" | tee -a ${ERROR_LOG}
echo "Checking ${HOME}/.ucimf.conf" | tee -a ${ERROR_LOG}
ls -lR ${HOME}/.ucimf.conf | tee -a ${ERROR_LOG}

echo "Checking /etc/ucimf.conf" | tee -a ${ERROR_LOG}
ls -lR /etc/ucimf.conf | tee -a ${ERROR_LOG}

echo "Checking /lib/ucimf/" | tee -a ${ERROR_LOG}
ls -lR /lib/ucimf/ | tee -a ${ERROR_LOG}

echo "Checking /lib/openvanilla/" | tee -a ${ERROR_LOG}
ls -lR /lib/openvanilla/ | tee -a ${ERROR_LOG}

echo "===Verify Files===" | tee -a ${ERROR_LOG}
ls -lR ${BUILD}/share/ucimf/ | tee -a ${ERROR_LOG}
ls -lR ${BUILD}/lib/ucimf/ | tee -a ${ERROR_LOG}
ls -lR ${BUILD}/lib/openvanilla/ | tee -a ${ERROR_LOG}

echo "===Verify Settings===" | tee -a ${ERROR_LOG}
ls -lR ${BUILD}/etc/ | tee -a ${ERROR_LOG}
cat   ${BUILD}/etc/ucimf.conf | tee -a ${ERROR_LOG}

echo "===Verify Libraries===" | tee -a ${ERROR_LOG}
pkg-config --version freetype2 | tee -a ${ERROR_LOG}
pkg-config --modversion freetype2 | tee -a ${ERROR_LOG}


echo "===Testing Console===" | tee -a ${ERROR_LOG}
LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${SCRIPTS}/debug/trace.gdb fbterm >> ${ERROR_LOG} 2>&1

echo "===Run again message===" | tee -a ${ERROR_LOG}
./run.sh 2>>${ERROR_LOG} >> ${ERROR_LOG}

	
echo "===Console Terminal failed, try testing===" | tee -a ${ERROR_LOG}
build_console_testing

echo "===Testing console case 01===" | tee -a ${ERROR_LOG}
cd ${BUILD}
LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${SCRIPTS}/debug/trace.gdb ${BUILD}/bin/console01 >>${ERROR_LOG} 2>&1

echo "===Run again message===" | tee -a ${ERROR_LOG}
LC_CTYPE=zh_TW.UTF-8 ${BUILD}/bin/console01 >>${ERROR_LOG} 2>&1
if (( $? )) 
then
	echo "====Rebuild libucimf==="
	build_libucimf
	setup_libucimf
fi

echo "===Testing console case 02===" | tee -a ${ERROR_LOG}
cd ${BUILD}
LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${SCRIPTS}/debug/trace.gdb ${BUILD}/bin/console02 >>${ERROR_LOG} 2>&1

echo "===Testing console case 03===" | tee -a ${ERROR_LOG}
cd ${BUILD}
FONT_PATH=${BUILD}/share/ucimf/unifont.pcf gdb -batch -x ${SCRIPTS}/debug/trace.gdb ${BUILD}/bin/console03 >>${ERROR_LOG} 2>&1


# Create convient bug report pack
test -d ${BUILD}/BugReport && rm -rf ${BUILD}/BugReport 

mkdir -p ${BUILD}/BugReport
cp ${ROOT}/libucimf/config.log  ${BUILD}/BugReport/libucimf.config.log
cp ${ROOT}/ucimf-openvanilla/config.log  ${BUILD}/BugReport/ucimf_openvanilla.config.log
cp ${ROOT}/console/fbterm_ucimf/config.log ${BUILD}/BugReport/fbterm_ucimf.config.log
cd ${BUILD}
tar -czf BugReport.tar.gz BugReport/
