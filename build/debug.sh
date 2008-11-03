#!/bin/bash
source ./inc.sh

echo "===Testing Console===" | tee  error.log
LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${BUILD}/debug/trace.gdb ${BUILD}/bin/fbterm >> error.log

./run.sh

if (( $? )) 
then
	echo "===Dump enviroment variables===" | tee -a error.log
	env >> error.log

	echo "===Verify Files===" | tee -a error.log
	ls -l ${BUILD}/share/ucimf/ | tee -a error.log
	ls -lR ${BUILD}/lib/ucimf/ | tee -a error.log
	ls -lR ${BUILD}/lib/openvanilla/ | tee -a error.log

	echo "===Verify Libraries===" | tee -a error.log
	pkg-config --version freetype2 | tee -a error.log
	pkg-config --modversion freetype2 | tee -a error.log
	
	echo "===Console Terminal failed, try testing===" | tee -a error.log
	build_console_testing

	echo "===Testing console case 01===" | tee -a error.log
	cd ${BUILD}
	LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${BUILD}/debug/trace.gdb ${BUILD}/bin/console01 >> error.log
fi

