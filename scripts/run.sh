#!/bin/bash

ROOT="$( dirname $(echo $0))/.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh


LC_CTYPE=zh_TW.UTF-8 fbterm -i ${BUILD}/bin/fbterm_ucimf 2>/dev/null
