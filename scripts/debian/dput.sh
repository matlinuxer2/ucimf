#!/bin/bash


ROOT="$( dirname $(echo $0))/../.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh


DEBIAN="${SCRIPTS}/debian"
REPO_DIR="${DEBIAN}/debian_archive"
DPUT_DIR="${REPO_DIR}/mini-dinstall/incoming"
DPUT_CONF="${DEBIAN}/dput.conf"
MINI_DINSTALL_CONF="${DEBIAN}/mini-dinstall.conf"

test -d $DPUT_DIR || mkdir -p $DPUT_DIR
test -d $REPO_DIR || mkdir -p $REPO_DIR


cat > $MINI_DINSTALL_CONF <<EOD

[DEFAULT]
archivedir = $REPO_DIR
#archive_style = flat
archive_style = simple-subdir
architectures = all, i386, amd64
poll_time = 10
extra_keyrings = ~/.gnupg/pubring.gpg
verify_sigs = 0
use_dnotify = 0
mail_on_success = 0
mail_log_level = NONE

[jaunty]
[unstable]

EOD

cat > $DPUT_CONF <<EOD

[local]
method                  = local
incoming                = $DPUT_DIR
run_dinstall            = 0
post_upload_command     = /usr/bin/mini-dinstall --config=$MINI_DINSTALL_CONF --batch 

EOD

#dput --config $DPUT_CONF --host-list
dput --config=$DPUT_CONF --no-upload-log local $1

rm $MINI_DINSTALL_CONF
rm $DPUT_CONF
