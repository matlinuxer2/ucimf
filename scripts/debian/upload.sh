#!/bin/bash

ROOT="$( dirname $(echo $0))/../.."
if [ "`echo "$ROOT" | cut -c1`" != "/" ];
then
        ROOT="$(pwd)/$ROOT"
fi

source $ROOT/scripts/env.sh

REPOS="${SCRIPTS}/debian/repos"
LFTP_SCRIPT="$SCRIPTS/debian/lftp.script"

pushd .

cd $REPOS

cat > $LFTP_SCRIPT << EOF
open sftp://mat@people.debian.org.tw
cd public_html/debian
mirror -R dists 
mirror -R pool
EOF

lftp -f $LFTP_SCRIPT
rm $LFTP_SCRIPT

popd
