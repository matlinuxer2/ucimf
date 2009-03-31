#!/bin/bash
# 
# Thanks to the Eric Zhao's script
#
#-----------------install--------------------
export LD_LIBRARY_PATH=${HOME}/local/lib
export PKG_CONFIG_PATH=${LD_LIBRARY_PATH}/pkgconfig

cd

wget -c http://ucimf.googlecode.com/files/UserManual.pdf
wget -c http://ucimf.googlecode.com/files/UserManual.txt

wget -c http://ucimf.googlecode.com/files/libucimf-2.2.8.tar.gz
tar -xvf libucimf-2.2.8.tar.gz
cd libucimf-2.2.8/
./configure --prefix=${HOME}/local
make
make install
cd
rm -rf libucimf-2.2.8/

sed -i "s@/usr@${HOME}/local@g" ${HOME}/local/etc/ucimf.conf


wget -c http://ucimf.googlecode.com/files/ucimf-openvanilla-2.10.5.tar.gz
tar -xvf ucimf-openvanilla-2.10.5.tar.gz
cd ucimf-openvanilla-2.10.5/
./configure --prefix=${HOME}/local
make
make install
cd
rm -rf ucimf-openvanilla-2.10.5/


wget -c http://ucimf.googlecode.com/files/openvanilla-modules-0.8.0_9.tar.gz
tar -xvf openvanilla-modules-0.8.0_9.tar.gz
cd openvanilla-modules-0.8.0_9/
./configure --prefix=${HOME}/local --with-zh_CN
make
make install
cd
rm -rf openvanilla-modules-0.8.0_9/


wget -c http://fbterm.googlecode.com/files/fbterm-1.4.tar.gz
tar -xvf fbterm-1.4.tar.gz
cd fbterm-1.4/
./configure --prefix=${HOME}/local
make
make install
cd
rm -rf fbterm-1.4/


wget -c http://ucimf.googlecode.com/files/fbterm_ucimf-0.2.4.tar.gz
tar -xvf fbterm_ucimf-0.2.4.tar.gz
cd fbterm_ucimf-0.2.4/
./configure --prefix=${HOME}/local
make
make install
cd
rm -rf fbterm_ucimf-0.2.4/


wget -c http://ftp.debian.org/debian/pool/main/d/dialog/dialog_1.1-20080819.orig.tar.gz
tar -xvf dialog_1.1-20080819.orig.tar.gz
cd dialog-1.1-20080819/
./configure --prefix=${HOME}/local
make
make install
cd
rm -rf dialog-1.1-20080819/

echo ""
echo "===NOTE==="
echo "FbTerm binary's permission setting is:"
echo ""
ls -l ${HOME}/local/bin/fbterm
echo ""


echo ""
echo "===NOTE==="
echo "If fbterm could not get CTRL-SPACE triggerred correctly, try to "
echo ""
echo "  chown 0.0 ${HOME}/local/bin/fbterm"
echo "  chmod u+s ${HOME}/local/bin/fbterm"
echo ""
echo "to make fbterm grab key succesfully."

#===========================================
echo ""
echo "===NOTE==="
echo "To start excution, just set enviroment and enter command below:"
echo ""
echo "  export LANG=zh_CN.UTF-8"
echo "  export LC_ALL=zh_CN.UTF-8"
echo "  export PATH=${HOME}/local/bin:\$PATH"
echo "  fbterm -i fbterm_ucimf"
echo ""
