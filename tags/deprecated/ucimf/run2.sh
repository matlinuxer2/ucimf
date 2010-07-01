#!/bin/bash

ROOT="$( dirname $(echo $0))/.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh

stty -icanon
LC_CTYPE=zh_TW.UTF-8 ${BUILD}/bin/console04 2>/dev/null
stty +icanon
