#!/bin/bash

ROOT="$( readlink -f $(dirname $0)/../..)" &&  source ${ROOT}/scripts/env.sh

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

	
echo "===Testing console case 01===" | tee -a ${ERROR_LOG}
LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${SCRIPTS}/debug/trace.gdb ${BUILD}/bin/console01 >>${ERROR_LOG} 2>&1

echo "===Run again message===" | tee -a ${ERROR_LOG}
LC_CTYPE=zh_TW.UTF-8 ${BUILD}/bin/console01 >>${ERROR_LOG} 2>&1


# Create convient bug report pack
BUG_REPORT_DIR="${SCRIPTS}/_report"

test -d ${BUG_REPORT_DIR} && rm -rf ${BUG_REPORT_DIR} 
mkdir -p ${BUG_REPORT_DIR}

cp ${LIBUCIMF}/config.log  ${BUG_REPORT_DIR}/libucimf.config.log
cp ${UCIMFOV}/config.log  ${BUG_REPORT_DIR}/ucimf-openvanilla.config.log
cp ${ERROR_LOG} ${BUG_REPORT_DIR}/error.log
cp ${HOME}/.ucimf-log ${BUG_REPORT_DIR}/ucimf-log

pushd .
cd ${BUG_REPORT_DIR}/.. && tar -czf ${SCRIPTS}/bug_report.tar.gz $(basename $BUG_REPORT_DIR)
popd 
