# Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
ifeq ($(strip $(RHIDESRC)),)
export RHIDESRC:=$(shell pwd)
endif

include $(RHIDESRC)/common.mak

ifeq ($(strip $(SETSRC)),)
ifeq ($(rhide_OS),DJGPP)
export SETSRC:=$(DJDIR)/contrib/setedit
else
export SETSRC:=$(RHIDESRC)/../setedit-0.4.39
endif
export SETOBJ:=$(SETSRC)/makes
endif

ifeq ($(strip $(TVSRC)),)
ifeq ($(rhide_OS),DJGPP)
export TVSRC:=$(DJDIR)/contrib/tvision
export TVOBJ:=$(TVSRC)/djgpp
else
export TVSRC:=$(RHIDESRC)/../librhtv-1.0.9
export TVOBJ:=$(TVSRC)/linux
endif
endif

ifeq ($(strip $(GDB_SRC)),)
ifeq ($(rhide_OS),DJGPP)
export GDB_SRC:=$(DJDIR)/gnu/gdb-4.18
export GDB_OBJ:=$(GDB_SRC)
else
export GDB_SRC:=$(RHIDESRC)/../gdb-4.18
export GDB_OBJ:=$(GDB_SRC)
endif
endif

_srcdir=$(RHIDESRC)
include $(_srcdir)/makefile.src

