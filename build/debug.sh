#!/bin/bash
source ./inc.sh

echo "===Testing Console===" | tee  error.log
LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${BUILD}/debug/trace.gdb ${BUILD}/bin/fbterm >> error.log

./run.sh

if (( $? )) 
then
	echo "Console Terminal failed, try testing"
	build_console_testing
	cd ${BUILD}
	echo "===Testing console case 01===" | tee -a error.log
	LC_CTYPE=zh_TW.UTF-8 gdb -batch -x ${BUILD}/debug/trace.gdb ${BUILD}/bin/console01 >> error.log
fi

