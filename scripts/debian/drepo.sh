#!/bin/bash

ROOT="$( dirname $(echo $0))/../.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh


DEBIAN="${SCRIPTS}/debian"
DPUT_DIR="${DEBIAN}/pkgs"
DPUT_CONF="${DEBIAN}/dput.conf"

install -d $DPUT_DIR

cat > $DPUT_CONF <<EOD

[local]
method                  = local
incoming                = $DPUT_DIR
run_dinstall            = 0

EOD

#dput --config $DPUT_CONF --host-list
dput --config=$DPUT_CONF --no-upload-log local $1

rm $DPUT_CONF



MYCODENAME=$(lsb_release -c | cut -c11-)
MYREPO="${DEBIAN}/debrepo"
MYTMP="${DEBIAN}/tmp"
MYPOOL="UCIMF"
MYPACKAGES="$DPUT_DIR"

install -d $MYTMP
install -d $MYREPO/conf

cat > $MYREPO/conf/distributions <<EOD

Codename: $MYCODENAME
Components: main contrib non-free
Architectures: i386 amd64 source
Description: Repository of UCIMF
SignWith: default

EOD

cat > $MYREPO/conf/incoming <<EOD

Name: $MYPOOL
IncomingDir: $MYPACKAGES
TempDir: $MYTMP
Multiple: Yes
Allow: $MYCODENAME unstable

EOD

reprepro -b $MYREPO createsymlinks
reprepro -b $MYREPO processincoming $MYPOOL
