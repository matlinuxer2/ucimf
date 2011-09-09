#!/bin/bash


# for tarball
pacman --remove --noconfirm fbterm-ucimf 
pacman --remove --noconfirm fbterm
pacman --remove --noconfirm openvanilla 
pacman --remove --noconfirm ucimf-openvanilla 
pacman --remove --noconfirm ucimf-sunpinyin
pacman --remove --noconfirm libucimf 

# for svn
pacman --remove --noconfirm fbterm-ucimf-svn
pacman --remove --noconfirm fbterm-svn
pacman --remove --noconfirm openvanilla-svn 
pacman --remove --noconfirm ucimf-openvanilla-svn 
pacman --remove --noconfirm libucimf-svn 

# clean temporary file
rm -rf /tmp/pacman-tmp-*
