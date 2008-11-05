#!/bin/bash
source ./inc.sh

read -t 10 -p "Choose console: [f(bterm)/j(fbterm)]" myconsole

if [[ $myconsole == "j" ]]
then
  echo "Build jfbterm as console program"
else
  echo "Build FbTerm as console program"
fi

build_libucimf
setup_libucimf
build_ucimf-openvanilla
build_openvanilla

if [[ $myconsole == "j" ]]
then
  echo "Build jfbterm as console program"
  build_console_fbterm
else
  echo "Build FbTerm as console program"
  build_console_jfbterm
fi
