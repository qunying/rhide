# Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
ifeq ($(strip $(RHIDESRC)),)
error You must set the environment variable RHIDESRC
endif

srcdir=$(RHIDESRC)/test
VPATH=$(srcdir)
project=abstorel

po_files=

src_files=$(po_files) makefile abstorel.gpr abstorel

update_files=abstorel.gpr abstorel.mak
cfg_files=

subdirs=
po_subdirs=

include $(RHIDESRC)/common.mak
