#!/bin/sh

#
# setup script for exception monitor
#

# With non GPL files, use following license

# Copyright 2005,2006,2008,2009  Sony Corporation

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions, and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.

# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

# Otherwise with GPL files, use following license

#  Copyright 2005-2006,2008  Sony Corporation

#  This program is free software; you can redistribute  it and/or modify it
#  under  the terms of  the GNU General  Public License as published by the
#  Free Software Foundation;  version 2 of the  License.

#  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
#  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
#  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
#  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
#  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
#  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
#  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#  You should have received a copy of the  GNU General Public License along
#  with this program; if not, write  to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#
# variables
#
top=`dirname $0`
disas="off"
demangle="off"
log="/root/exception.txt"
board=""

#internal use
license="CONFIG_EM_LICENSE=\"Dual BSD/GPL\""

#
# functions
#

print_usage()
{
cat <<EOF
Usage: $0 board=<board> [disas=<disas>][demangle=<demangle>][log=<filename>]
          <board>    := your target board name
          <disas>    := { off(default), bsd, gnu }
          <demangle> := { off(default), bsd, gnu }
EOF
}

warning()
{
    echo "WARNING: "$@
}

error()
{
    echo "ERROR: "$@
}

abort()
{
    error $@
    exit 1
}

check_ret()
{
    if [ "$ret" != "0" ] ; then
	abort "bad return value at: "$@
    fi
}

check_board()
{
    if [ "$board" == "" ] ; then
	abort "board not specified"
    fi
    if [ ! -f ${top}/board/${board}_defconfig ] ; then
	abort "file not found: ${board}_defconfig"
    fi
}

check_license()
{
    case $1 in
	off)
	    ;;
	internal | gnu)
	    license="CONFIG_EM_LICENSE=\"Dual BSD/GPL\""
	    ;;
	bsd)
	    if [ "$disas" == "bsd" ] && [ "$demangle" == "off" ]; then
		license="CONFIG_EM_LICENSE=\"BSD\""
	    fi
	    ;;
	*)
	    error "unknown format."
	    print_usage
	    exit 1
	    ;;
    esac
}

check_disas()
{
    check_license $disas
}

check_demangle()
{
    check_license $demangle
}

check_license_config()
{
    . ${top}/board/${board}_defconfig
    if [ "$CONFIG_EM_ATOMIC_CALLBACK" = "y" ]; then
	    license="CONFIG_EM_LICENSE=\"Dual BSD/GPL\""
    fi
}

check_log()
{
    if [ ${log} != "/root/exception.txt" ] ; then
	echo "  exception log output dir changed to: ${log}."
    fi
}

parse_opts()
{
    while [ "$#" != "0" ] ; do
	arg=$1
	case $arg in
	    board=*)
		board=${arg#board=}
		;;
	    disas=*)
		disas=${arg#disas=}
		;;
	    demangle=*)
		demangle=${arg#demangle=}
		;;
	    log=*)
		log=${arg#log=}
		;;
	    *)
		error "unknown option: $arg"
		print_usage
		exit 1
		;;
	esac
	shift
    done
}

set_config()
{
    # setup .config file
    config_file=${top}/.config
    cp -f ${top}/board/${board}_defconfig ${config_file}
    ret=$?
    check_ret "cp .config"

    if [ "$disas" != "off" ] ; then
	echo "CONFIG_EM_DISASSEMBLE=y"              >>${config_file}
	if [ "${disas}" == "bsd" ] ; then
	    echo "CONFIG_EM_DISASSEMBLE_BSD=y"      >>${config_file}
	elif [ "${disas}" == "internal" -o "${disas}" == "gnu" ]; then
	    echo "CONFIG_EM_DISASSEMBLE_GNU=y"      >>${config_file}
	fi
    else
	echo "# CONFIG_EM_DISASSEMBLE is not set"   >>${config_file}
    fi

    if [ "$demangle" != "off" ] ; then
	echo "CONFIG_EM_DEMANGLE=y"                 >>${config_file}
	if [ "${demangle}" == "bsd" ] ; then
	    echo "CONFIG_EM_DEMANGLE_BSD=y"         >>${config_file}
	elif [ "${demangle}" == "internal" -o "${demangle}" == "gnu" ]; then
	    echo "CONFIG_EM_DEMANGLE_GNU=y"         >>${config_file}
	fi
    else
	echo "# CONFIG_EM_DEMANGLE is not set"      >>${config_file}
    fi
    echo ${license} >>${config_file}
    echo "CONFIG_EM_LOGFILENAME=\"${log}\"" >>${config_file}

    # setup autoconf.h
    if [ ! -x ${top}/scripts/makeconf.sh ] ; then
	error "script not executable: makeconf.sh"
    fi
    ${top}/scripts/makeconf.sh ${config_file}
    ret=$?
    check_ret "makeconf.sh"
}

set_symlink()
{
    config_file=${top}/.config
    if [ ! -f ${config_file} ] ; then
	error "file not found: ${config_file}"
    fi
    arch=`grep CONFIG_EM_ARCH ${config_file} | sed 's,CONFIG_EM_ARCH=,,'`
    if [ "$disas" == "bsd" -o "$demangle" == "bsd" ] ; then
	case $arch in
	    arm)
		(cd ${top}/bsd/include && \
		    rm -f arm && rm -f powerpc && rm -f machine && \
		    ln -s arch/arm/include arm && \
		    ln -s arch/arm/include machine )
		ret=$?
		check_ret "rm & symlink bsd include"
		(cd ${top}/sony/include && \
		    rm -f bsd && \
		    ln -s ../../bsd/include bsd )
		ret=$?
		check_ret "rm & symlink bsd include"
		;;
	    ppc)
		(cd ${top}/bsd/include && \
		    rm -f arm && rm -f powerpc && rm -f machine && \
		    ln -s arch/powerpc/include powerpc && \
		    ln -s arch/powerpc/include machine )
		ret=$?
		check_ret "rm & symlink bsd include"
		;;
	    *)
		error "unknown arch $arch"
		;;
	esac
    fi
}

#
# start point
#

if [ "$1" == "" ] ; then
    print_usage
    exit 0
fi

parse_opts $@
check_board
check_disas
check_demangle
check_license_config
check_log

set_config
set_symlink

echo "Done setup for $board"
echo "Next, do: PROMPT> make"

# EOF
