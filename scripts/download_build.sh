#!/bin/sh

FBTERM_HOME="http://fbterm.googlecode.com/"
FBTERM="fbterm-1.6"

UCIMF_HOME="http://ucimf.googlecode.com/"
LIBUCIMF="libucimf-2.2.9.1"
UCIMF_OPENVANILLA="ucimf-openvanilla-2.10.6"
OPENVANILLA_MODULES="openvanilla-modules-0.8.0_14"
FBTERM_UCIMF="fbterm_ucimf-0.2.6"
USERMANUAL="UserManual.pdf"

#DIALOG_HOME="http://www.hightek.org/dialog/"
#DIALOG="dialog-0.7"
DIALOG_HOME="ftp://ftp.invisible-island.net/dialog/"
DIALOG="dialog-1.1-20080819"

SOURCE_DIR=${HOME}/source
BUILD_DIR=${HOME}/build
PREFIX_DIR=${HOME}/fbterminal

mkdir -pv ${SOURCE_DIR} ${BUILD_DIR}

if [ "${PREFIX_DIR}" != "/usr" ]; then
	export LD_LIBRARY_PATH=${PREFIX_DIR}/lib
	export PKG_CONFIG_PATH=${LD_LIBRARY_PATH}/pkgconfig
fi


get_source ()
{
	cd ${SOURCE_DIR} &&

	wget -nc -c ${FBTERM_HOME}/files/${FBTERM}.tar.gz &&

	wget -nc -c ${UCIMF_HOME}files/${LIBUCIMF}.tar.gz &&
	wget -nc -c ${UCIMF_HOME}files/${UCIMF_OPENVANILLA}.tar.gz &&
	wget -nc -c ${UCIMF_HOME}files/${OPENVANILLA_MODULES}.tar.gz &&
	wget -nc -c ${UCIMF_HOME}files/${FBTERM_UCIMF}.tar.gz &&
	wget -nc -c ${UCIMF_HOME}files/${USERMANUAL} &&

	wget -nc -c ${DIALOG_HOME}${DIALOG}.tgz

	if [ $? != 0 ]; then
		echo "GET SOURCE:$0 ERROR!"
		exit 1
	fi
}


# autobuild $1 $2 $3 ...
# autobuild "libucimf-2.2.9"
# autobuild "libucimf-2.2.9" "--sysconfdir=/etc"
autobuild ()
{
	tar -xvf ${SOURCE_DIR}/$1.* -C ${BUILD_DIR} &&
	cd ${BUILD_DIR}/$1 &&

	./configure --prefix=${PREFIX_DIR} $2 &&
	make &&
	make install

	if [ $? != 0 ]; then
		echo "PACKAGE AUTOBUILD:$0 ERROR!"
		exit 1
	fi
}


get_source

if [ "${PREFIX_DIR}" != "/usr" ]; then
	autobuild "${LIBUCIMF}"
	[ "${PREFIX_DIR}" != "/usr" ] && sed -i "s@/usr@${PREFIX_DIR}@g" ${PREFIX_DIR}/etc/ucimf.conf
else
	autobuild "${LIBUCIMF}" "--sysconfdir=/etc"
fi
autobuild ${UCIMF_OPENVANILLA}

#autobuild ${OPENVANILLA_MODULES} "--with-zh_CN"
autobuild ${OPENVANILLA_MODULES}

autobuild ${FBTERM}

autobuild ${FBTERM_UCIMF}

autobuild ${DIALOG}


echo ""
echo "===NOTE==="
echo "FbTerm binary's permission setting is:"
echo ""
ls -l ${PREFIX_DIR}/bin/fbterm
echo ""


echo ""
echo "===NOTE==="
echo "If fbterm could not get CTRL-SPACE triggerred correctly,"
echo "please login in as root user and try to"
echo ""
echo " chown 0.0 ${PREFIX_DIR}/bin/fbterm"
echo " chmod u+s ${PREFIX_DIR}/bin/fbterm"
echo ""
echo "to make fbterm grab key succesfully."

#===========================================
echo ""
echo "===NOTE==="
echo "To start excution, just set enviroment and enter command below:"
echo ""
echo "  export LANG=zh_CN.UTF-8"
echo "  export LC_ALL=zh_CN.UTF-8"
echo "  export PATH=${PREFIX_DIR}/bin:\$PATH"
echo "  fbterm -i fbterm_ucimf"
echo ""
