#!/bin/bash

PREFIX=YOUR_COSTOM_PATH;
export LD_LIBRARY_PATH=$PREFIX/lib/ ;
export PATH=$PREFIX/bin/:$PATH ;
export OVMODULE_DIR=$PREFIX/lib/openvanilla/ ;

export UCIMF_FONTPATH=/usr/share/fonts/truetype/arphic/ukai.ttf ;
#export UCIMF_FONTPATH=/usr/X11R6/lib/X11/fonts/misc/b16_b.pcf.gz ;

export LC_CTYPE=zh_TW.UTF-8 ;

/bin/bash;

