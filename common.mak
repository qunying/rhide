# Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
#

.SUFFIXES: .mak .gpr

all::

ifeq ($(strip $(list_file)),)
list_file=list
endif

ifeq ($(strip $(po_list)),)
po_list=po_list
endif

ifeq ($(strip $(logfile)),)
logfile=logfile
endif

# I have changed data/locale to share/locale !!!!!!!!!!!!!!!!!!

locale_dir=$(prefix)/share/locale
PACKAGE=rhide
# for the DJGPP archives $(PACKAGE_FILE)$(FILE_VERSION) must not
# exceed 8 characters, the well known DOS limitation :-(
PACKAGE_FILE=rhid
PACKAGE_DIR=rhide
VERSION=1.4.4.x
RHIDE_MAJOR=$(word 1,$(subst ., ,$(VERSION)))
RHIDE_MINOR=$(subst $(RHIDE_MAJOR),,$(VERSION))
# for the DJGPP archives
FILE_VERSION=$(subst .,,$(VERSION))
DIR_VERSION_Linux=-$(VERSION)
# for the DJGPP directories
DIR_VERSION_DJGPP=-$(RHIDE_MAJOR).$(subst .,,$(RHIDE_MINOR))
DIR_VERSION=$(DIR_VERSION_$(RHIDE_OS))

DJP=
use_djp=no

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


ifneq ($(strip $(DJDIR)),)
export rhide_OS:=DJGPP
INSTALL_DIR=ginstall -d -m 0755
INSTALL_DATA=ginstall $(DATA_MODE)
INSTALL_PROGRAM=ginstall -s $(EXEC_MODE) $(SETUID) $(SETGID)
DJP=djp
use_djp=yes
endif

ifeq ($(strip $(rhide_OS)),)
export rhide_OS:=$(shell uname)
endif

ifeq ($(rhide_OS),DJGPP)
exe=.exe
else
exe=
endif

SETMODE=0
setgid=
setuid=
ifeq ($(strip $(rhide_OS)),Linux)
INSTALL_DIR=install -d -m 0755
INSTALL_DATA=install $(DATA_MODE)
INSTALL_PROGRAM=install -s $(EXEC_MODE) $(SETUID) $(SETGID)
endif

# This makes top_dir as a relative path to srcdir
empty=
space=$(empty) $(empty)
top_dir:=$(strip $(subst $(RHIDESRC),,$(srcdir)))
ifeq ($(top_dir),)
top_dir:=.
else
top_dir:=$(subst /, ,$(top_dir))
top_dir:=$(patsubst %,../,$(top_dir))
top_dir:=$(subst $(space),,$(top_dir))
top_dir:=$(patsubst %/,%,$(top_dir))
endif

obj_dir:=$(shell pwd)
ifeq ($(top_obj_dir),)
export top_obj_dir:=$(shell cd $(top_dir); pwd; cd $(obj_dir))
endif

gpr2mak:=$(wildcard $(top_dir)/gpr2mak.exe)

ifeq ($(strip $(gpr2mak)),)
ifneq ($(rhide_OS),DJGPP)
gpr2mak:=$(shell which gpr2mak)
else
gpr2mak:=$(wildcard $(DJDIR)/bin/gpr2mak.exe)
endif
endif

ifeq ($(strip $(prefix)),)
ifeq ($(rhide_OS),DJGPP)
prefix=$(DJDIR)
else
prefix=/usr/local
endif
endif

install_datadir=share/$(PACKAGE)
install_docdir=doc/$(PACKAGE)
install_infodir=info
install_bindir=bin
install_localedir=share/locale

rhideb=$(PACKAGE_FILE)$(FILE_VERSION)b
rhides=$(PACKAGE_FILE)$(FILE_VERSION)s
rhide=$(PACKAGE_DIR)$(DIR_VERSION)

ifeq ($(rhide_OS),DJGPP)
copy_dir=f:/_
else
copy_dir=/usr/tmp/_
endif

FLAGS_TO_PASS=\
	CFLAGS="$(CFLAGS)" \
	LDFLAGS="$(LDFLAGS)" \
	RHIDESRC="$(RHIDESRC)" \
	rhide_OS="$(rhide_OS)" \
	top_obj_dir="$(top_obj_dir)" \
	install_datadir=$(install_datadir) \
	install_docdir=$(install_docdir) \
	install_infodir=$(install_infodir) \
	install_bindir=$(install_bindir) \
	install_localedir=$(install_localedir) \
	DATA_MODE="$(DATA_MODE)" \
	EXEC_MODE="$(EXEC_MODE)" \
	SETUID="$(SETUID)" \
	SETGID="$(SETGID)" \
	prefix=$(prefix)

%.sub: Makefile
	$(MAKE) -C $* $(FLAGS_TO_PASS) SUBDIR_TARGET=$(SUBDIR_TARGET) \
	  $(SUBDIR_TARGET) logfile=../__log__
	touch __log__
	cat __log__ >> $(logfile)
	rm -f __log__

subdir_do: $(addsuffix .sub,$(sort $(subdirs)))

O_SRC_FILES=$(wildcard $(addprefix $(obj_dir)/,$(sort $(src_files))))
_O_SRC_FILES=$(subst $(obj_dir)/,,$(O_SRC_FILES))
__SRC_FILES=$(filter-out $(_O_SRC_FILES),$(sort $(src_files)))
S_SRC_FILES=$(wildcard $(addprefix $(srcdir)/,$(__SRC_FILES)))
_S_SRC_FILES=$(subst $(srcdir)/,,$(S_SRC_FILES))
_SRC_FILES=$(filter-out $(_O_SRC_FILES) $(_S_SRC_FILES),$(sort $(src_files)))
SRC_FILES=$(O_SRC_FILES) $(S_SRC_FILES) $(_SRC_FILES)

ifneq ($(strip $(subdirs)),)
copy_src_files:: $(addsuffix .cpy,$(sort $(subdirs)))

update__srcdir:: $(addsuffix .sub,$(subdirs))
endif

copy_src_files:: $(SRC_FILES)
ifneq ($(SRC_FILES),)
	@echo Copying files from $(srcdir)
	@$(INSTALL_DIR) $(prefix)
	@cp -p $(SRC_FILES) $(prefix)
	@echo $(addprefix $(list_prefix),$(sort $(src_files))) >> $(list_file)
endif

po_list:: $(po_files)
	echo $(addprefix $(po_prefix),$(po_files)) > $(po_list)
ifneq ($(strip $(po_subdirs)),)
# special case for po_subdirs=..
ifeq ($(strip $(po_subdirs)),..)
this_dir:=$(notdir $(shell pwd))
po_list::
	$(MAKE) -C .. $(FLAGS_TO_PASS) po_list=$(this_dir)/$(this_dir).pst \
	  po_prefix=$(po_prefix)../ po_list
	cat $(this_dir).pst >> $(po_list)
	rm $(this_dir).pst
else
po_sub_files=$(addsuffix .pst,$(po_subdirs))
po_list:: $(po_sub_files)
	cat $(po_sub_files) >> $(po_list)
	rm $(po_sub_files)
endif
endif

%.pst: Makefile
	$(MAKE) -C $* $(FLAGS_TO_PASS) po_list=../$@ \
	  po_prefix=$(po_prefix)$*/ po_list

update__srcdir:: $(update_files)
ifeq ($(strip $(update_files)),)
	@echo Nothing to do for updating $(srcdir)
else
ifeq ($(obj_dir),$(srcdir))
	@echo Nothing to do for updating $(srcdir)
else
	cp -fp $(update_files) $(srcdir)
endif
endif

%.cpy: Makefile
	@-mkdir -p $(prefix)/$*
	@-rm -f $*/$(list_file)
	@$(MAKE) -C $* $(FLAGS_TO_PASS) prefix=$(prefix)/$* \
	  --no-print-directory \
	  list_file=$(list_file) list_prefix=$(list_prefix)$*/ copy_src_files
	@cat $*/$(list_file) >> $(list_file)
	@-rm -f $*/$(list_file)

ifneq ($(strip $(gpr2mak)),)
%.mak: %.gpr
ifeq ($(rhide_OS),DJGPP)
	$(gpr2mak) -d -r- -o - $< \
	  | sed -e 's,	$(DJDIR),	$$(DJDIR),g' \
	        -e '/^		$$(DJDIR).*\\$$/d' \
	        -e 's,^		$$(DJDIR)[^\\]*$$,,' \
	        -e 's,	$(RHIDESRC),	$$(RHIDESRC),g' \
		-e 's,^		$(obj_dir)/,		,' \
		-e 's,	$(top_obj_dir),	$$(top_obj_dir),g' \
	  $(USER_GPR2MAK_SEDS) > $@
else
	$(gpr2mak) -d -r- -o - $< \
	  | sed -e 's,	$(RHIDESRC),	$$(RHIDESRC),g' \
	        -e 's,	/usr/include,	$$(USRINC),g' \
	        -e 's,	/usr/lib/gcc-lib,	$$(USRINC),g' \
	        -e '/^		$$(USRINC).*\\$$/d' \
	        -e 's,^		$$(USRINC)[^\\]*$$,,' \
		-e 's,^		$(obj_dir)/,		,' \
		-e 's,	$(top_obj_dir),	$$(top_obj_dir),g' \
	  $(USER_GPR2MAK_SEDS) > $@
endif
endif

ifneq ($(strip $(project)),)
-include $(addsuffix .mak,$(project))
endif

ifeq ($(strip $(objdir)),)
check.objdir:
	@echo ERROR: You must set the variable objdir
	@redir -e nul -o nul
else
# make objdir absolute
ifeq ($(strip $(rhide_OS)),DJGPP)
override objdir:=$(strip $(subst \,/,$(shell truename $(subst /,\,$(objdir)))))
else
override objdir:=`mkdir -p $(objdir); cd $(objdir); pwd`
endif

check.objdir:
	@echo Using `$(objdir)\' as object directory

#ifeq ($(word 2,$(subst :, ,$(objdir))),)
#	@echo ERROR: You must set the variable $$(objdir) to an absolute directory
#	@redir -e nul -o nul
#endif
endif

create.objdir::
	@echo Configuring $(objdir) ..
	-mkdir $(objdir)

%.cfo: Makefile
	$(MAKE) -C $* $(FLAGS_TO_PASS) objdir=$(objdir)/$* config

.PHONY: config

ifeq ($(wildcard $(top_dir)/rhide.src),)
config::
	@echo ERROR: The target 'config.objdir' is valid only in the source tree
	@redir -e nul -o nul
else
config:: check.objdir create.objdir
	update $(srcdir)/Makefile $(objdir)/Makefile
ifneq ($(cfg_files),)
	cp -fp $(addprefix $(srcdir)/,$(cfg_files)) $(objdir)
endif
ifneq ($(project),)
	cp -fp $(addprefix $(srcdir)/,$(addsuffix .gpr,$(project))) $(objdir)
	cp -fp $(addprefix $(srcdir)/,$(addsuffix .mak,$(project))) $(objdir)
endif
ifneq ($(subdirs),)
config:: $(addsuffix .cfo,$(subdirs))
endif
endif

$(RHIDESRC)/tvision/include/tv.h:
	$(MAKE) -C $(top_dir)/tvpatch patches

install.data::
install.bin::
install.info::
install.doc::

install::
	rm -f $(logfile) $(instfile)

ifneq ($(strip $(install_data_files)),)
install.data:: $(install_data_files)
	$(INSTALL_DIR) $(prefix)/$(install_datadir)
	$(INSTALL_DATA) $^ $(prefix)/$(install_datadir)
	echo $(addprefix $(install_datadir)/,$(install_data_files)) >> $(logfile)
endif

ifneq ($(strip $(install_info_files)),)
install.info:: $(install_info_files)
	$(INSTALL_DIR) $(prefix)/$(install_infodir)
	$(INSTALL_DATA) $^ $(prefix)/$(install_infodir)
	echo $(addprefix $(install_infodir)/,$(install_info_files)) >> $(logfile)
endif

ifneq ($(strip $(install_doc_files)),)
install.doc:: $(install_doc_files)
	$(INSTALL_DIR) $(prefix)/$(install_docdir)
	$(INSTALL_DATA) $^ $(prefix)/$(install_docdir)
	echo $(addprefix $(install_docdir)/,$(install_doc_files)) >> $(logfile)
endif

ifneq ($(strip $(install_bin_files)),)
INSTALL_BIN_FILES=$(addsuffix $(exe),$(install_bin_files))
install.bin:: $(INSTALL_BIN_FILES)
	$(INSTALL_DIR) $(prefix)/$(install_bindir)
	$(INSTALL_PROGRAM) $^ $(prefix)/$(install_bindir)
ifeq ($(use_djp),yes)
	$(DJP) $(addprefix $(prefix)/$(install_bindir)/,$(INSTALL_BIN_FILES))
endif
	echo $(addprefix $(install_bindir)/,$(INSTALL_BIN_FILES)) >> $(logfile)
endif

ifneq ($(SUBDIR_TARGET),)
install:: $(addsuffix .sub,$(subdirs))
endif

install:: install.data install.info install.doc install.bin
