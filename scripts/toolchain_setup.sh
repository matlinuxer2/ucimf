#!/bin/sh

Distributor_ID=`lsb_release -i | awk '{print $3}'`
Distributor_Codename=`lsb_release -c | awk '{print $2}'`

lookup_table(){
    local toolname=$1

	grep "^$toolname" - << EOF
autotool Ubuntu hardy autotoolll
autotool Debian lenny autotoolll
EOF

}

get_pkg_name()
{
    local toolname=$1
	lookup_table $toolname | grep $Distributor_ID | grep $Distributor_Codename | awk '{print $4}'
}

pkg_autotool=$(get_pkg_name autotool)
all_tools="$pkg_autotool"

case $Distributor_Codename in
    lenny) echo "apt-get install $all_tools" ;;
    hardy) echo "apt-get install $all_tools" ;;
    *) echo "Your Distributor Codename => $Distributor_Codename"
esac
