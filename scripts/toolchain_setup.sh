#!/bin/sh

# TODO: 這裡需要檢查 lsb_release 的存在
# TODO: 如果沒有 lsb_release 的話，或許可以讓使用者指定 distribution 跟 codename 的參數
# TODO: 像是 `toolchain_setup.sh <distribution> <codename>`

Distributor_ID=`lsb_release -i | awk '{print $3}'`
Distributor_Codename=`lsb_release -c | awk '{print $2}'`

# 
#  顯示 inline 的 cmd_name <--> pkg_name 的對應表格
#  目前表格內的資料，預計會是 ucimf upstream 開發時所需要安裝的開發工具
#
#  表格格式為:  <cmd_name> <Distribution> <Codename> <pkg_name>
#  允許一個 <cmd_name> 對應多個 <pkg_name>
#
show_table(){
	cat << EOF
autotool Ubuntu hardy autotoolll
autotool Debian lenny autotoolll
autotool Gentoo n/a automake
autotool Gentoo n/a autotoolll
automake
autoconf
libtool
subversion
fakeroot
g++
libfreetype
libfontconfig
libltdl
pkg-config
EOF

}

# 
#  傳 <query_cmd> 這個參數給進來，然後這個函式會以查表的方式來
#  找出相對應的 <pkg_name> 並顯示出來
#
get_pkg_name()
{
	local query_cmd=$1
	result=$( show_table | grep "^$query_cmd" 2>/dev/null \
                     | grep $Distributor_ID  2>/dev/null \
                     | grep $Distributor_Codename 2>/dev/null \
                     | awk '{print $4}' \
                     | xargs echo -n  )

	if [ "x" != "x$result" ]; then
            echo "$result"
        else
            echo "<$result>'s package not found." > /dev/stderr
        fi
}

pkg_autotool=$(get_pkg_name autotool)
all_tools="$pkg_autotool"

case $Distributor_ID in
    Debian) echo "apt-get install $all_tools" ;;
    Ubuntu) echo "apt-get install $all_tools" ;;
    Gentoo) echo "emerge $all_tools" ;;
    *) echo "Your distribution($Distributor_ID) haven't been included the check list yet."
esac
