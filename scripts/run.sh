#!/bin/bash
source ./inc.sh

LC_CTYPE=zh_TW.UTF-8 ${BUILD}/bin/fbterm -i ${BUILD}/bin/fbterm_ucimf 2>/dev/null
