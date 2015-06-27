#!/bin/bash

aptitude install sudo \
  automake \
  autoconf \
  libtool \
  subversion \
  gnupg-agent\
  debhelper \
  reprepro \
  fakeroot \
  g++ \
  libfreetype6-dev \
  libfontconfig-dev \
  libltdl-dev \
  pkg-config \
  lsb-release


## # for multiple version
## SOURCE_DIR="."
## pushd .
## cd $SOURCE_DIR
##         wget http://tjworld.net/raw-attachment/wiki/Linux/Ubuntu/Packages/PPAPublishMultipleReleases/ppa_publish
##         chmod +x ppa_publish
##         ./ppa_publish dput-ppa lenny
## popd
## 
