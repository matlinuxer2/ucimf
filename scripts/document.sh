#!/bin/bash

# 
# 這個 scripts 將會把 google code 上和 source codes 相關的文件及紀錄收到
# /build/doc 這個資料夾下。讓文件的整合能先一次集中到某個地方，不致於
# 散落各地。使用者可以用 Browser 來看 /build/doc/index.html 方便的瀏覽
# 各項文件
# 

source ./inc.sh

test -d ${BUILD} || mkdir -p ${BUILD}

DOC=${BUILD}/doc
DOCUMENT=${DOC}/data
test -d ${DOC} && rm -rvf ${DOC}
mkdir -p ${DOC}
mkdir -p ${DOCUMENT}

WIKIDIR=${ROOT}/wiki

WIKIHTMLDIR=${DOCUMENT}/html
test -d ${WIKIHTMLDIR} && rm -rvf ${WIKIHTMLDIR}
mkdir -p ${WIKIHTMLDIR}

# An auto script for document gathering and build.



gather_README(){

	test -f ${DOCUMENT}/README
	for DIR in "LIBUCIMF" "UCIMFOV" "OV" "FBTERMUCIMF"
	do
		dir=$( eval echo \$${DIR})
		name=$( basename ${dir} )
		echo "::::::::::::::" >> ${DOCUMENT}/README
		echo "${name}/README" >>  ${DOCUMENT}/README
		echo "::::::::::::::" >> ${DOCUMENT}/README

		cat ${dir}/README >> ${DOCUMENT}/README
	done
}

gather_INSTALL(){
	test -f ${DOCUMENT}/INSTALL
	for DIR in "LIBUCIMF" "UCIMFOV" "OV" "FBTERMUCIMF"
	do
		dir=$( eval echo \$${DIR})
		name=$( basename ${dir} )
		echo "::::::::::::::" >> ${DOCUMENT}/INSTALL
		echo "${name}/INSTALL" >>  ${DOCUMENT}/INSTALL
		echo "::::::::::::::" >> ${DOCUMENT}/INSTALL

		cat ${dir}/INSTALL >> ${DOCUMENT}/INSTALL
	done
}

gather_ChangeLog(){

	test -f ${DOCUMENT}/ChangeLog
	for DIR in "LIBUCIMF" "UCIMFOV" "FBTERMUCIMF"
	do
		dir=$( eval echo \$${DIR})
		name=$( basename ${dir} )
		echo "::::::::::::::" >> ${DOCUMENT}/ChangeLog
		echo "${name}/ChangeLog" >>  ${DOCUMENT}/ChangeLog
		echo "::::::::::::::" >> ${DOCUMENT}/ChangeLog

		cat ${dir}/ChangeLog >> ${DOCUMENT}/ChangeLog
	done

}

#gather_Doxygen(){
#
#}

gather_GoogleCodeHomepage(){


	cd ${WIKIHTMLDIR}

	PAGE_URL="http://code.google.com/p/ucimf/"

	wget -O this.html $PAGE_URL

	tidy -q -asxhtml -numeric -utf8 < this.html > this.xml
	${SCRIPTS}/gchome.py this.xml > ${WIKIHTMLDIR}/main.html
	rm this.html this.xml

	cd -
}

gather_GoogleCodeWikiHtml(){


	cd ${WIKIHTMLDIR}
	for PAGE in "TOCArticles"
	do
		echo $PAGE
		PAGE_URL="http://code.google.com/p/ucimf/wiki/$PAGE"
		# -k will make links to local viewable
		wget -nc -np -r -k -c -nd $PAGE_URL

		#tidy -q -asxhtml -numeric -utf8 < $PAGE.html > $PAGE.xml
		#${SCRIPTS}/gcw.py $PAGE.xml > "$DOCUMENT/$PAGE.txt"
		#rm $PAGE.html $PAGE.xml
	done 
	cd -
}

gather_GoogleCodeWikiTxt(){
	WIKITXTDIR=${DOCUMENT}/txt
	test -d ${WIKITXTDIR} && rm -rvf ${WIKITXTDIR}
	mkdir -p ${WIKITXTDIR}

	cd ${WIKITXTDIR}
	for PAGE in `ls ${WIKIDIR} |grep -e '.*\.wiki'`
	do
		echo $PAGE
		cp ${WIKIDIR}/$PAGE -v ${WIKITXTDIR}/${PAGE%.wiki}.txt
	done 
}

gather_Index(){
	cd ${SCRIPTS}
	cp -av document/{index.html,list.html} ${DOC}
	cd -
}

gather_README
gather_INSTALL
gather_ChangeLog
gather_GoogleCodeHomepage
gather_GoogleCodeWikiHtml
gather_GoogleCodeWikiTxt
gather_Index
