# Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
ifeq ($(strip $(RHIDESRC)),)
export RHIDESRC:=$(shell pwd)
endif
ifeq ($(strip $(SETSRC)),)
export SETSRC:=$(RHIDESRC)/../setedit-0.4.39
export SETOBJ:=$(RHIDESRC)/../setedit-0.4.39/makes
endif
ifeq ($(strip $(TVSRC)),)
export TVSRC:=$(RHIDESRC)/../librhtv-1.0.9
export TVOBJ:=$(RHIDESRC)/../librhtv-1.0.9/linux
endif
ifeq ($(strip $(GDB_SRC)),)
export GDB_SRC:=$(RHIDESRC)/../gdb-4.18
export GDB_OBJ:=$(RHIDESRC)/../gdb-4.18
endif

_srcdir=$(RHIDESRC)
include $(_srcdir)/makefile.src

