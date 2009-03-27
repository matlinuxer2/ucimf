#!/bin/bash


# for tarball
yaourt -S --noconfirm libucimf 
yaourt -S --noconfirm ucimf-openvanilla 
yaourt -S --noconfirm openvanilla 
yaourt -S --noconfirm fbterm
yaourt -S --noconfirm fbterm-ucimf 

# list installed packages
pacman -Q |grep libucimf
pacman -Q |grep ucimf-openvanilla
pacman -Q |grep openvanilla
pacman -Q |grep fbterm-ucimf
pacman -Q |grep fbterm
