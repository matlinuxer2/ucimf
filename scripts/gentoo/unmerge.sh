emerge --unmerge jfbterm
emerge --unmerge fbterm
emerge --unmerge fbterm-ucimf 
emerge --unmerge libucimf
emerge --unmerge ucimf-openvanilla
emerge --unmerge openvanilla-modules 
qlist --installed --verbose |grep openvanilla
qlist --installed --verbose |grep ucimf
qlist --installed --verbose |grep fbterm
