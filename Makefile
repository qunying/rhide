# Copyright (C) 1996-2001 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
-include config.env
ifeq ($(strip $(RHIDESRC)),)
error!! You have to set the variable RHIDESRC
endif

_srcdir=$(RHIDESRC)
include $(_srcdir)/makefile.src
