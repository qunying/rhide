# Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
ifeq ($(strip $(RHIDESRC)),)
export RHIDESRC:=$(shell pwd)
endif

_srcdir=$(RHIDESRC)
include $(_srcdir)/makefile.src

