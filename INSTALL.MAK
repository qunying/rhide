# Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 

.SUFFIXES:

setgid=
setuid=
SETGID=
SETUID=
SETMODE=0

ifneq ($(setgid),)
SETGID=-g $(setgid)
SETMODE=2
endif

ifneq ($(setuid),)
SETUID=-o $(setuid)
SETMODE=4
endif

EXEC_MODE=-m $(SETMODE)755
DATA_MODE=-m 0644


INSTALL_BIN=install $(EXEC_MODE) $(SETUID) $(SETGID)
INSTALL_DATA=install $(DATA_MODE)
INSTALL_DIR=install -d -m 0755

prefix=/usr/local
bin_dir=bin
data_dir=share
info_dir=info
doc_dir=doc/rhide
# If you change this, you have to set $LOCALEDIR to this directory
locale_prefix=/usr
locale_dir=share/locale

files:=$(shell find $(bin_dir) $(data_dir) $(info_dir) $(doc_dir) \
	-not -type d -print)
LOCALE_FILES=$(addprefix $(locale_prefix)/,\
               $(filter $(locale_dir)/%,$(files)))
ALL_FILES=$(addprefix $(prefix)/,$(filter-out $(locale_dir)/%,\
	    $(files))) $(LOCALE_FILES)

$(locale_prefix)/$(locale_dir)/%: $(locale_dir)/%
	$(INSTALL_DIR) $(dir $@)
	$(INSTALL_DATA) $< $@

$(prefix)/$(bin_dir)/%: $(bin_dir)/%
	$(INSTALL_DIR) $(dir $@)
	$(INSTALL_BIN) $< $@

$(prefix)/%: %
	$(INSTALL_DIR) $(dir $@)
	$(INSTALL_DATA) $< $@

all install: $(ALL_FILES)
