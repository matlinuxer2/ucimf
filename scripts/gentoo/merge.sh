#!/bin/bash
layman -s gentoo-zh
USE="ucimf" ACCEPT_KEYWORDS="~*" emerge jfbterm 
ACCEPT_KEYWORDS="~*" emerge fbterm fbterm-ucimf libucimf ucimf-openvanilla openvanilla-modules ucimf-sunpinyin ucimf-chewing
qlist -Iv openvanilla ucimf fbterm 
