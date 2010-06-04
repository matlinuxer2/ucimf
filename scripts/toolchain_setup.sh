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
autoconf	Debian lenny autoconf
g++		Debian lenny g++
ltdl-dev	Debian lenny libltdl3-dev
freetype2-dev	Debian lenny libfreetype6-dev
fontconfig-dev	Debian lenny libfontconfig1-dev
make		Debian lenny make
curses-dev	Debian lenny libncurses5-dev

autoconf	Ubuntu hardy autoconf
g++		Ubuntu hardy g++
ltdl-dev	Ubuntu hardy libltdl3-dev
freetype2-dev	Ubuntu hardy libfreetype6-dev
fontconfig-dev	Ubuntu hardy libfontconfig1-dev
make		Ubuntu hardy make
curses-dev	Ubuntu hardy libncurses5-dev

autotool Gentoo n/a automake

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

pkg_autoconf=$(get_pkg_name autoconf)
pkg_gplusplus=$(get_pkg_name g++)
pkg_ltdl_dev=$(get_pkg_name ltdl-dev)
pkg_freetype2_dev=$(get_pkg_name freetype2-dev)
pkg_fontconfig_dev=$(get_pkg_name fontconfig-dev)
pkg_make=$(get_pkg_name make)
pkg_curses_dev=$(get_pkg_name curses-dev) 
all_tools="$pkg_autoconf $pkg_gplusplus $pkg_ltdl_dev $pkg_freetype2_dev $pkg_fontconfig_dev $pkg_make $pkg_curses_dev"

case $Distributor_ID in
    Debian) echo "apt-get install $all_tools" ;;
    Ubuntu) echo "apt-get install $all_tools" ;;
    Gentoo) echo "emerge $all_tools" ;;
    *) echo "Your distribution($Distributor_ID) haven't been included the check list yet."
esac
