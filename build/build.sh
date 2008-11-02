#!/bin/bash
source ./inc.sh

build_libucimf
setup_libucimf
build_ucimf-openvanilla
build_openvanilla
build_console_fbterm
