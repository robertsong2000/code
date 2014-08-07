#!/bin/sh

# With non GPL files, use following license

# Copyright 2005-2006,2008  Sony Corporation

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

#  Copyright 2005-2006,2008 Sony Corporation.

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


LANG=C
outfile="autoconf.h"
date=`date`

if [ "$1" == "" ] ; then
    if [ ! -f ".config" ] ; then
	echo "file: .config: file not found."
	exit 1
    fi
    infile=".config"
else
    if [ ! -f $1 ] ; then
	echo "file: $1: file not found. "
	exit 1
    fi
    infile=$1
fi

cat > ${outfile} <<EOF
/*
 *  ${outfile}  - This is automatically generated. Do not edit.
 *
 * Created date: ${date}
 *
 * Sony CONFIDENTIAL
 *
 * Copyright 2005  Sony Corporation
 *
 */
#ifndef AUTOCONF_H
#define AUTOCONF_H
EOF

#
# config file rule:
#   - comment line must start with `#' charater
#   - configure feature must start with `CONFIG_EM_'
#
cat ${infile} | grep -v "^#" | grep "^CONFIG_EM_" | \
    awk 'BEGIN{FS="="}{print "#define",$1,$2}' | sed 's, y$, 1,' >> ${outfile}
ret=$?

if [ "$ret" != "0" ] ; then
    echo "create file: ${outfile}: failed."
    exit 1
fi

cat >> ${outfile} <<EOF
#endif /* AUTOCONF_H */
EOF

exit 0
# EOF
