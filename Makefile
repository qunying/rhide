ifeq ($(strip $(RHIDESRC)),)
error!! You have to set the variable RHIDESRC
endif

_srcdir=$(RHIDESRC)
include $(_srcdir)/makefile.src
