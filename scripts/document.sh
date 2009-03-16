#!/bin/bash
source ./inc.sh

DOCUMENT=${ROOT}/doc
test -d ${DOCUMENT} && rm -rvf ${DOCUMENT}
mkdir -p ${DOCUMENT}

# An auto script for document gathering and build.



gather_README(){

	test -f ${DOCUMENT}/README
	for DIR in "LIBUCIMF" "UCIMFOV" "OV" "FBTERMUCIMF"
	do
		dir=$( eval echo \$${DIR})
		name=$( basename ${dir} )
		echo "${name}...Done"
		cat ${dir}/README >> ${DOCUMENT}/README.${name}
	done
}

gather_INSTALL(){
	test -f ${DOCUMENT}/INSTALL
	for DIR in "LIBUCIMF" "UCIMFOV" "OV" "FBTERMUCIMF"
	do
		dir=$( eval echo \$${DIR})
		name=$( basename ${dir} )
		echo "${name}...Done"
		cat ${dir}/INSTALL >> ${DOCUMENT}/INSTALL.${name}
	done
}

gather_ChangeLog(){

	test -f ${DOCUMENT}/ChangeLog
	for DIR in "LIBUCIMF" "UCIMFOV" "FBTERMUCIMF"
	do
		dir=$( eval echo \$${DIR})
		name=$( basename ${dir} )
		echo "${name}...Done"
		cat ${dir}/ChangeLog >> ${DOCUMENT}/ChangeLog.${name}
	done

}

#gather_Doxygen(){
#
#}

gather_GoogleCodeWiki(){

	for PAGE in "Gentoo" "Debian" "Arch" "FAQ"
	do
		echo $PAGE
		PAGE_URL="http://code.google.com/p/ucimf/wiki/$PAGE"
		wget --continue -O $PAGE.html $PAGE_URL

		tidy -q -asxhtml -numeric -utf8 < $PAGE.html > $PAGE.xml
		${SCRIPTS}/gcw.py $PAGE.xml > "$DOCUMENT/$PAGE.txt"
		rm $PAGE.html $PAGE.xml
	done 
}

gather_README
gather_INSTALL
gather_ChangeLog
gather_GoogleCodeWiki
