#!/bin/bash

yaourt -S --noconfirm libucimf-svn
yaourt -S --noconfirm ucimf-openvanilla-svn
yaourt -S --noconfirm openvanilla-svn
yaourt -S --noconfirm fbterm-svn
yaourt -S --noconfirm fbterm-ucimf-svn

# list installed packages
pacman -Q |grep libucimf
pacman -Q |grep ucimf-openvanilla
pacman -Q |grep openvanilla
pacman -Q |grep fbterm-ucimf
pacman -Q |grep fbterm
