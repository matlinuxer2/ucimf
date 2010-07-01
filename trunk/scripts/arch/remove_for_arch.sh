#!/bin/bash


# for tarball
pacman --remove fbterm-ucimf 
pacman --remove fbterm
pacman --remove openvanilla 
pacman --remove ucimf-openvanilla 
pacman --remove libucimf 

# for svn
pacman --remove fbterm-ucimf-svn
pacman --remove fbterm-svn
pacman --remove openvanilla-svn 
pacman --remove ucimf-openvanilla-svn 
pacman --remove libucimf-svn 

# clean temporary file
rm -rf /tmp/pacman-tmp-*
