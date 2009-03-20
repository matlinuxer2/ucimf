#!/bin/bash
# ACCEPT_KEYWORDS="**" 可以 enable *-9999.ebuild 的 svn 套件
layman -s gentoo-taiwan
USE="ucimf" ACCEPT_KEYWORDS="~*" emerge jfbterm 
ACCEPT_KEYWORDS="**" emerge fbterm fbterm-ucimf libucimf ucimf-openvanilla openvanilla-modules 
qlist --installed --verbose |grep openvanilla
qlist --installed --verbose |grep ucimf
qlist --installed --verbose |grep fbterm

