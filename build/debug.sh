#!/bin/bash
source ./inc.sh

echo "===Testing Console===" | tee  error.log
LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${BUILD}/debug/trace.gdb ${BUILD}/bin/fbterm 2>> error.log >> error.log

echo "===Run again message===" | tee -a error.log
./run.sh 2>>error.log >> error.log

if (( $? )) 
then
	echo "===Check SVN Revision===" | tee -a error.log
	svn info | grep 'Revision' >> error.log

	echo "===Dump enviroment variables===" | tee -a error.log
	env >> error.log

	echo "===Check Overlay Files===" | tee -a error.log
	echo "Checking ${HOME}/.ucimf.conf" | tee -a error.log
	ls -lR ${HOME}/.ucimf.conf | tee -a error.log

	echo "Checking /etc/ucimf.conf" | tee -a error.log
	ls -lR /etc/ucimf.conf | tee -a error.log

	echo "Checking /lib/ucimf/" | tee -a error.log
	ls -lR /lib/ucimf/ | tee -a error.log

	echo "Checking /lib/openvanilla/" | tee -a error.log
	ls -lR /lib/openvanilla/ | tee -a error.log

	echo "===Verify Files===" | tee -a error.log
	ls -lR ${BUILD}/share/ucimf/ | tee -a error.log
	ls -lR ${BUILD}/lib/ucimf/ | tee -a error.log
	ls -lR ${BUILD}/lib/openvanilla/ | tee -a error.log

	echo "===Verify Settings===" | tee -a error.log
	ls -lR ${BUILD}/etc/ | tee -a error.log
	cat   ${BUILD}/etc/ucimf.conf | tee -a error.log

	echo "===Verify Libraries===" | tee -a error.log
	pkg-config --version freetype2 | tee -a error.log
	pkg-config --modversion freetype2 | tee -a error.log
	
	echo "===Console Terminal failed, try testing===" | tee -a error.log
	build_console_testing

	echo "===Testing console case 01===" | tee -a error.log
	cd ${BUILD}
	LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${BUILD}/debug/trace.gdb ${BUILD}/bin/console01 2>>error.log >> error.log

	echo "===Run again message===" | tee -a error.log
	LC_CTYPE=zh_TW.UTF-8 ${BUILD}/bin/console01 2>>error.log >> error.log
	if (( $? )) 
	then
		echo "====Rebuild libucimf==="
		build_libucimf
		setup_libucimf
	fi

	echo "===Testing console case 02===" | tee -a error.log
	cd ${BUILD}
	LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${BUILD}/debug/trace.gdb ${BUILD}/bin/console02 2>>error.log >> error.log

	echo "===Testing console case 03===" | tee -a error.log
	cd ${BUILD}
	FONT_PATH=${BUILD}/share/ucimf/unifont.pcf gdb -batch -x ${BUILD}/debug/trace.gdb ${BUILD}/bin/console03 2>>error.log >> error.log
fi


# Create convient bug report pack
test -d ${BUILD}/BugReport && rm -rf ${BUILD}/BugReport 

mkdir -p ${BUILD}/BugReport
cp ${BUILD}/../libucimf/config.log  ${BUILD}/BugReport/libucimf.config.log
cp ${BUILD}/../ucimf-openvanilla/config.log  ${BUILD}/BugReport/ucimf_openvanilla.config.log
cp ${BUILD}/../console/jfbterm/jfbterm-0.4.7/config.log ${BUILD}/BugReport/jfbterm.config.log
cp ${BUILD}/../console/fbterm/fbterm-1.2/config.log ${BUILD}/BugReport/fbterm.config.log
cp ${BUILD}/../console/dummy/config.log ${BUILD}/BugReport/dummy.config.log
cp ${BUILD}/../OpenVanilla/Modules/config.log ${BUILD}/BugReport/OpenVanillaModules.config.log
cd ${BUILD}
tar -czf BugReport.tar.gz BugReport/
