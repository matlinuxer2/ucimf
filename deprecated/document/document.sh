#!/bin/bash

# 
# 這個 scripts 將會把 google code 上和 source codes 相關的文件及紀錄收到
# /build/doc 這個資料夾下。讓文件的整合能先一次集中到某個地方，不致於
# 散落各地。使用者可以用 Browser 來看 /build/doc/index.html 方便的瀏覽
# 各項文件
# 

ROOT="$( dirname $(echo $0))/../.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh


DOCUMENT_SCRIPTS=${SCRIPTS}/document
DOC=${BUILD}/doc
DOCUMENT=${DOC}/data
WIKIDIR=${ROOT}/wiki
WIKIHTMLDIR=${DOCUMENT}/html


test -d ${BUILD} || mkdir -p ${BUILD}

test -d ${DOC} && rm -rvf ${DOC}
mkdir -p ${DOC}
mkdir -p ${DOCUMENT}

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
	pushd .

	cd ${WIKIHTMLDIR}

	PAGE_URL="http://code.google.com/p/ucimf/"

	wget -O this.html $PAGE_URL

	tidy -q -asxhtml -numeric -utf8 < this.html > this.xml
	${DOCUMENT_SCRIPTS}/gchome.py this.xml > ${WIKIHTMLDIR}/main.html
	echo '<script type="text/javascript" src="http://www.ohloh.net/p/315926/widgets/project_basic_stats.js"></script>' >> ${WIKIHTMLDIR}/main.html

	rm this.html this.xml

	popd
}

gather_GoogleCodeWikiHtml(){
	pushd .

	cd ${WIKIHTMLDIR}
	for PAGE in "TOCArticles"
	do
		echo $PAGE
		PAGE_URL="http://code.google.com/p/ucimf/wiki/$PAGE"
		# -k will make links to local viewable
		wget -nc -np -r -k -c -nd $PAGE_URL

		#tidy -q -asxhtml -numeric -utf8 < $PAGE.html > $PAGE.xml
		#${DOCUMENT_SCRIPTS}/gcw.py $PAGE.xml > "$DOCUMENT/$PAGE.txt"
		#rm $PAGE.html $PAGE.xml
	done 

	popd 
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
	pushd .

	cd ${DOCUMENT_SCRIPTS}
	cp -av {index.html,list.html} ${DOC}

	popd 

}

gather_README
gather_INSTALL
gather_ChangeLog
gather_GoogleCodeHomepage
gather_GoogleCodeWikiHtml
gather_GoogleCodeWikiTxt
gather_Index
