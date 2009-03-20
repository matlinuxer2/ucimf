#!/bin/bash
layman -s gentoo-taiwan
USE="ucimf" ACCEPT_KEYWORDS="~*" emerge jfbterm 
ACCEPT_KEYWORDS="~*" emerge fbterm fbterm-ucimf libucimf ucimf-openvanilla openvanilla-modules 
qlist -Iv openvanilla ucimf fbterm 
