#
#define abs_path
#cd $(1) pwd
#endef
abs_path=$(shell sh -c "cd $(1) && pwd")
#abs_path=$(1)
first_dir=$(subst $(2),,$(word 1,$(foreach dir,$(1),$(wildcard $(dir)$(2)))))

RHIDE_OS_=$(RHIDE_OS)
ifeq ($(strip $(RHIDE_OS_)),)
ifneq ($(strip $(DJDIR)),)
RHIDE_OS_:=DJGPP
else
RHIDE_OS_:=$(patsubst CYGWIN%,CYGWIN,$(shell uname))
endif
endif
override RHIDE_OS:=$(RHIDE_OS_)

RHIDE_SRC_=$(RHIDE_SRC)
ifeq ($(strip $(RHIDE_SRC_)),)
override RHIDE_SRC_:=$(shell pwd)
endif
override RHIDE_SRC:=$(RHIDE_SRC_)

SEARCH_TVSRC_DJGPP=$(DJDIR)/contrib/tvision \
	$(RHIDE_SRC)/../tvision

SEARCH_TVSRC_Linux=/usr/local/src/tvision \
	/usr/src/tvision \
	$(RHIDE_SRC)/../tvision

SEARCH_SETSRC_DJGPP=$(DJDIR)/contrib/setedit \
	$(RHIDE_SRC)/../setedit

SEARCH_SETSRC_Linux=/usr/local/src/setedit \
	/usr/src/setedit \
	$(RHIDE_SRC)/../setedit

SEARCH_GDBSRC_DJGPP=$(DJDIR)/gnu/gdb-5.0 \
	$(RHIDE_SRC)/../gdb-5.0

SEARCH_SETSRC_Linux=/usr/local/src/gdb-5.0 \
	/usr/src/gdb-5.0 \
	$(RHIDE_SRC)/../gdb-5.0

TVSRC_=$(TVSRC)
ifeq ($(strip $(TVSRC_)),)
override TVSRC_=$(call abs_path,$(call first_dir,\
	$(SEARCH_TVSRC_$(RHIDE_OS)),/include/tv.h))
endif
override TVSRC:=$(TVSRC_)

SETSRC_=$(SETSRC)
ifeq ($(strip $(SETSRC_)),)
override SETSRC_=$(call abs_path,$(call first_dir,\
	$(SEARCH_SETSRC_$(RHIDE_OS)),/include/ceditor.h))
endif
override SETSRC:=$(SETSRC_)

GDB_SRC_=$(GDB_SRC)
ifeq ($(strip $(GDB_SRC_)),)
override GDB_SRC_=$(call abs_path,$(call first_dir,\
	$(SEARCH_GDBSRC_$(RHIDE_OS)),/gdb/gdbtypes.h))
endif
override GDB_SRC:=$(GDB_SRC_)

TVOBJ_=$(TVOBJ)
ifeq ($(TVOBJ_),)
override TVOBJ_=$(TVSRC)/$(patsubst DJGPP,djgpp,$(patsubst Linux,linux,$(RHIDE_OS)))
endif
override TVOBJ:=$(TVOBJ_)

SETOBJ_=$(SETOBJ)
ifeq ($(SETOBJ_),)
override SETOBJ_=$(SETSRC)/makes
endif
override SETOBJ:=$(SETOBJ_)

config_file=config.env

echo_vars:
	@echo RHIDE_OS=$(RHIDE_OS)
	@echo RHIDE_SRC=$(RHIDE_SRC)
	@echo TVSRC=$(TVSRC)
	@echo SETSRC=$(SETSRC)
	@echo GDB_SRC=$(GDB_SRC)
	@echo TVOBJ=$(TVOBJ)
	@echo SETOBJ=$(SETOBJ)

create_config:
	@echo RHIDE_SRC=$(RHIDE_SRC) > $(config_file)
	@echo TVSRC=$(TVSRC) >> $(config_file)
	@echo SETSRC=$(SETSRC) >> $(config_file)
	@echo GDB_SRC=$(GDB_SRC) >> $(config_file)
	@echo TVOBJ=$(TVOBJ) >> $(config_file)
	@echo SETOBJ=$(SETOBJ) >> $(config_file)