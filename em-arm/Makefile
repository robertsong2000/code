#
# DESCRIPTION:
#   Makefile for Exception Monitor module
#   Linux kernel 2.6 version

# With non GPL files, use following license

# Copyright 2004,2005,2006,2008,2009 Sony Corporation.

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

#  Copyright 2004-2006,2008 Sony Corporation.

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


EM_TOPDIR	:= $(shell pwd)/

EM_TARGET	:= exception_monitor
EM_OBJS		:= sony/exception.o
EM_CONFIG	:= $(shell if [ -f .config ]; then echo y; else echo n; fi)

ifeq ($(EM_CONFIG),y)
include $(EM_TOPDIR).config
EM_ARCH		= $(CONFIG_EM_ARCH)
include $(EM_TOPDIR)sony/arch/$(EM_ARCH)/Arch.make
EM_OBJS		+= sony/arch/$(EM_ARCH)/exception-$(EM_ARCH).o
  ifeq ($(CONFIG_EM_NOTIFY), y)
EM_OBJS		+= sony/emnotify.o
  endif

#
# CFLAGS is replaced by KBUILD_CFLAGS since 2.6.24 kernel
#
VERSION_CODE    := $(shell cat $(KBUILD)/include/linux/version.h  | \
	                   grep -e "^\#define LINUX_VERSION_CODE" | \
	                   cut -d " " -f 3)
VCODE_2624       = $(shell expr 2 \* 65536 + 6 \* 256 + 24)
KERNEL_LT_2624  := $(shell if [ $(VERSION_CODE) -lt $(VCODE_2624) ]; \
	                   then echo y; else echo n; fi)

  ifeq ($(KERNEL_LT_2624),y)
CFLAGS	        += -I$(EM_TOPDIR)sony/include
  else
KBUILD_CFLAGS	+= -I$(EM_TOPDIR)sony/include
  endif
  ifeq ($(CONFIG_EM_DISASSEMBLE),y)
    ifeq ($(CONFIG_EM_DISASSEMBLE_BSD),y)
EM_OBJS		+= bsd/arch/$(EM_ARCH)/db_disasm.o
    endif
    ifeq ($(CONFIG_EM_DISASSEMBLE_GNU),y)
      ifeq ($(EM_ARCH),i686)
EM_OBJS		+= internal/arch/$(EM_ARCH)/i386-dis.o
      else
EM_OBJS		+= internal/arch/$(EM_ARCH)/$(EM_ARCH)-dis.o
        ifneq ($(EM_ARCH),arm)
EM_OBJS		+= internal/arch/$(EM_ARCH)/$(EM_ARCH)-opc.o
        endif
      endif
      ifeq ($(KERNEL_LT_2624),y)
CFLAGS          += -I$(EM_TOPDIR)internal/include
      else
KBUILD_CFLAGS   += -I$(EM_TOPDIR)internal/include
      endif
    endif
  endif

  ifeq ($(CONFIG_EM_DEMANGLE),y)
    ifeq ($(CONFIG_EM_DEMANGLE_GNU),y)
EM_OBJS		+= internal/demangle/cp-demangle.o
      ifeq ($(KERNEL_LT_2624),y)
CFLAGS		+= -I$(EM_TOPDIR)internal/include
      else
KBUILD_CFLAGS	+= -I$(EM_TOPDIR)internal/include
      endif
    endif
  endif
endif

ifdef ANOTHERINC
VERDIR = $(ANOTHERINC)
else
VERDIR = $(TARGETDIR)/usr/include
endif

ifeq ($(KERNEL_LT_2624),y)
CFLAGS        += -I$(VERDIR) -I$(EM_TOPDIR)
else
KBUILD_CFLAGS += -I$(VERDIR) -I$(EM_TOPDIR)
endif

INSTALLDIR = $(TARGETDIR)/lib/modules/

obj-m = $(EM_TARGET).o
$(EM_TARGET)-objs = $(EM_OBJS)

all:
	make EM_TOPDIR=$(EM_TOPDIR) -C $(KBUILD) M=$(shell pwd) modules

install: all
	install -d $(INSTALLDIR)
	install -m 644 $(EM_TARGET).ko $(INSTALLDIR)

clean:
	make EM_TOPDIR=$(EM_TOPDIR) -C $(KBUILD) M=$(shell pwd) clean

distclean: clean
	rm -rf .config* autoconf.h

release: distclean
	rm -rf internal TODO.txt testsuite

# EOF
