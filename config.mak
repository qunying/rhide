#
abs_path=$(shell sh -c "cd $(arg3) && pwd")

first_dir=$(subst $(arg2),,$(word 1,$(foreach dir,$($(arg1)),$(wildcard $(dir)$(arg2)))))

RHIDE_OS_=$(RHIDE_OS)
ifeq ($(strip $(RHIDE_OS_)),)
ifneq ($(strip $(DJDIR)),)
RHIDE_OS_:=DJGPP
else
RHIDE_OS_:=$(patsubst CYGWIN%,CYGWIN,$(shell uname))
endif
endif
override RHIDE_OS:=$(RHIDE_OS_)

RHIDESRC_=$(RHIDESRC)
ifeq ($(strip $(RHIDESRC_)),)
override RHIDESRC_:=$(shell pwd)
endif
override RHIDESRC:=$(RHIDESRC_)

SEARCH_TVSRC_DJGPP=$(DJDIR)/contrib/tvision \
	$(RHIDESRC)/../tvision

SEARCH_TVSRC_Linux=/usr/local/src/tvision \
	/usr/src/tvision \
	$(RHIDESRC)/../tvision

SEARCH_SETSRC_DJGPP=$(DJDIR)/contrib/setedit \
	$(RHIDESRC)/../setedit

SEARCH_SETSRC_Linux=/usr/local/src/setedit \
	/usr/src/setedit \
	$(RHIDESRC)/../setedit

SEARCH_GDBSRC_DJGPP=$(DJDIR)/gnu/gdb-5.1 \
	$(RHIDESRC)/../gdb-5.1

SEARCH_GDBSRC_Linux=/usr/local/src/gdb-5.1 \
	$(RHIDESRC)/../gdb-5.1

TVSRC_=$(TVSRC)
ifeq ($(strip $(TVSRC_)),)
arg2=/include/tv.h
arg1=SEARCH_TVSRC_$(RHIDE_OS)
arg3=$(first_dir)
override TVSRC_=$(abs_path)
endif
override TVSRC:=$(TVSRC_)

SETSRC_=$(SETSRC)
ifeq ($(strip $(SETSRC_)),)
arg2=/include/ceditor.h
arg1=SEARCH_SETSRC_$(RHIDE_OS)
arg3=$(first_dir)
override SETSRC_=$(abs_path)
endif
override SETSRC:=$(SETSRC_)

GDB_SRC_=$(GDB_SRC)
ifeq ($(strip $(GDB_SRC_)),)
arg2=/gdb/gdbtypes.h
arg1=SEARCH_GDBSRC_$(RHIDE_OS)
arg3=$(first_dir)
override GDB_SRC_=$(abs_path)
endif
override GDB_SRC:=$(GDB_SRC_)
ifeq ($(strip $(GDB_OBJ)),)
GDB_OBJ=$(GDB_SRC)
endif

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

ifeq ($(wildcard $(SETOBJ)/libeasyd.a),$(SETOBJ)/libeasyd.a)
SET_LIBS=-L$(SETOBJ)/../libpcre -L$(SETOBJ)/../libbzip2 -leasyd -lsettv -lbz2
else
SET_LIBS=$(SETOBJ)/easydiag.a
endif
SET_LIBS+=-lrhtv

echo_vars:
	@echo RHIDE_OS=$(RHIDE_OS)
	@echo RHIDESRC=$(RHIDESRC)
	@echo TVSRC=$(TVSRC)
	@echo SETSRC=$(SETSRC)
	@echo GDB_SRC=$(GDB_SRC)
	@echo GDB_OBJ=$(GDB_OBJ)
	@echo TVOBJ=$(TVOBJ)
	@echo SETOBJ=$(SETOBJ)
	@echo SET_LIBS=$(SET_LIBS)

create_config:
	@echo export RHIDESRC=$(RHIDESRC) > $(config_file)
	@echo export TVSRC=$(TVSRC) >> $(config_file)
	@echo export SETSRC=$(SETSRC) >> $(config_file)
	@echo export GDB_SRC=$(GDB_SRC) >> $(config_file)
	@echo export GDB_OBJ=$(GDB_OBJ) >> $(config_file)
	@echo export TVOBJ=$(TVOBJ) >> $(config_file)
	@echo export SETOBJ=$(SETOBJ) >> $(config_file)
	@echo export SET_LIBS=$(SET_LIBS) >> $(config_file)
