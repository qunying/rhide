# Copyright (C) 1996-2003 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
# gpr2mak -d -r- -o - libide.gpr
vpath_src=$(RHIDESRC)/libide
vpath %.c $(vpath_src)
vpath %.cc $(vpath_src)
vpath %.cpp $(vpath_src)
vpath %.C $(vpath_src)
vpath %.cxx $(vpath_src)
vpath %.s $(vpath_src)
vpath %.S $(vpath_src)
vpath %.p $(vpath_src)
vpath %.pas $(vpath_src)
vpath %.f $(vpath_src)
vpath %.for $(vpath_src)
vpath %.F $(vpath_src)
vpath %.fpp $(vpath_src)
vpath %.i $(vpath_src)
vpath %.ii $(vpath_src)
vpath %.m $(vpath_src)
vpath %.asm $(vpath_src)
vpath %.nsm $(vpath_src)
vpath_header=$(RHIDESRC)/libide/include $(RHIDESRC)/libtvuti/include\
	$(RHIDESRC)/librhuti $(TV_INC) $(SETSRC)/include\
	$(SETSRC)/settvuti/include
vpath %.h $(vpath_header)
vpath %.hpp $(vpath_header)
vpath %.ha $(vpath_header)
vpath %.hd $(vpath_header)
FLAGS_FOR_SUBPROJECTS=RHIDE_OS_="$(RHIDE_OS_)" CFLAGS="$(CFLAGS)"\
	CXXFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)" CPPFLAGS="$(CPPFLAGS)"
RHIDE_OS=$(RHIDE_OS_)
ifeq ($(strip $(RHIDE_OS)),)
ifneq ($(strip $(DJDIR)),)
RHIDE_OS_:=DJGPP
else
RHIDE_OS_:=$(patsubst CYGWIN%,CYGWIN,$(shell uname))
endif
endif

INCLUDE_DIRS=$(RHIDESRC)/libide/include $(RHIDESRC)/libtvuti/include\
	$(RHIDESRC)/librhuti $(TV_INC) $(SETSRC)/include\
	$(SETSRC)/settvuti/include
LIB_DIRS=
C_DEBUG_FLAGS=-g
C_OPT_FLAGS=-O2
C_WARN_FLAGS=
C_C_LANG_FLAGS=
C_CXX_LANG_FLAGS=
C_P_LANG_FLAGS=
C_FPC_LANG_FLAGS=
C_F_LANG_FLAGS=
C_ADA_LANG_FLAGS=
LIBS=
LD_EXTRA_FLAGS=
C_EXTRA_FLAGS=-DRHIDE
LOCAL_OPT=$(subst ___~~~___, ,$(subst $(notdir $<)___,,$(filter $(notdir\
	$<)___%,$(LOCAL_OPTIONS))))

OBJFILES=commands.o ideclass.o names/ndepcoll.o names/ndepende.o\
	names/nflagcol.o names/nflagent.o names/nideedit.o names/nidefile.o\
	names/noptions.o names/nproject.o stream/sdepcoll.o\
	stream/sdepende.o stream/sflagcol.o stream/sflagent.o\
	stream/sideedit.o stream/sidefile.o stream/soptions.o\
	stream/sproject.o tdepende.o tflagcol.o tflagent.o tflaglis.o\
	tfname.o tideedit.o tidefile.o toptions.o tprjlist.o tprjwind.o\
	tproject.o
ALL_OBJFILES=commands.o ideclass.o names/ndepcoll.o names/ndepende.o\
	names/nflagcol.o names/nflagent.o names/nideedit.o names/nidefile.o\
	names/noptions.o names/nproject.o stream/sdepcoll.o\
	stream/sdepende.o stream/sflagcol.o stream/sflagent.o\
	stream/sideedit.o stream/sidefile.o stream/soptions.o\
	stream/sproject.o tdepende.o tflagcol.o tflagent.o tflaglis.o\
	tfname.o tideedit.o tidefile.o toptions.o tprjlist.o tprjwind.o\
	tproject.o
LIBRARIES=
SOURCE_NAME=$<
OUTFILE=$@
SPECIAL_CFLAGS=
SPECIAL_LDFLAGS=
PROG_ARGS=
SRC_DIRS=$(RHIDESRC)/libide
WUC=
EDITORS=
MAIN_TARGET=libide.a
PROJECT_ITEMS=commands.cc ideclass.cc names/names.gpr stream/stream.gpr\
	tdepende.cc tflagcol.cc tflagent.cc tflaglis.cc tfname.cc\
	tideedit.cc tidefile.cc toptions.cc tprjlist.cc tprjwind.cc\
	tproject.cc
DEFAULT_MASK=*.[cghmps]*
RHIDE_BIN_DIR=d:/obj/rhide
PASCAL_TYPE=GPC
GET_HOME=$(HOME)
CLEAN_FILES=$(MAIN_TARGET) $(OBJFILES)
DEFAULT_GREP_MASK=*.[cfhmnps]*
RHIDE_ADA_BIND_FILE=$(addprefix _,$(setsuffix .c,$(OUTFILE)))
RHIDE_AR=ar
RHIDE_ARFLAGS=rcs
RHIDE_AS=$(RHIDE_GCC)
RHIDE_CO=$(shell co -q $(co_arg))
RHIDE_COMPILE.C.ii=$(RHIDE_COMPILE.cc.ii)
RHIDE_COMPILE.C.o=$(RHIDE_COMPILE.cc.o)
RHIDE_COMPILE.C.s=$(RHIDE_COMPILE.cc.s)
RHIDE_COMPILE.F.o=$(RHIDE_COMPILE.f.o)
RHIDE_COMPILE.adb.o=$(RHIDE_COMPILE_ADA)
RHIDE_COMPILE.asm.o=$(RHIDE_COMPILE.nsm.o)
RHIDE_COMPILE.c.i=$(subst -c $(SOURCE_NAME),-E\
	$(SOURCE_NAME),$(RHIDE_COMPILE_C))
RHIDE_COMPILE.c.o=$(RHIDE_COMPILE_C)
RHIDE_COMPILE.c.s=$(subst -c $(SOURCE_NAME),-S\
	$(SOURCE_NAME),$(RHIDE_COMPILE_C))
RHIDE_COMPILE.cc.ii=$(subst -c $(SOURCE_NAME),-E\
	$(SOURCE_NAME),$(RHIDE_COMPILE_CC))
RHIDE_COMPILE.cc.o=$(RHIDE_COMPILE_CC)
RHIDE_COMPILE.cc.s=$(subst -c $(SOURCE_NAME),-S\
	$(SOURCE_NAME),$(RHIDE_COMPILE_CC))
RHIDE_COMPILE.cpp.ii=$(RHIDE_COMPILE.cc.ii)
RHIDE_COMPILE.cpp.o=$(RHIDE_COMPILE.cc.o)
RHIDE_COMPILE.cpp.s=$(RHIDE_COMPILE.cc.s)
RHIDE_COMPILE.cxx.ii=$(RHIDE_COMPILE.cc.ii)
RHIDE_COMPILE.cxx.o=$(RHIDE_COMPILE.cc.o)
RHIDE_COMPILE.cxx.s=$(RHIDE_COMPILE.cc.s)
RHIDE_COMPILE.f.o=$(RHIDE_COMPILE_FORTRAN)
RHIDE_COMPILE.for.o=$(RHIDE_COMPILE.f.o)
RHIDE_COMPILE.fpp.o=$(RHIDE_COMPILE.f.o)
RHIDE_COMPILE.i.s=$(RHIDE_COMPILE.c.s)
RHIDE_COMPILE.ii.s=$(RHIDE_COMPILE.cc.s)
RHIDE_COMPILE.nsm.o=$(RHIDE_COMPILE_NASM)
RHIDE_COMPILE.p.o=$(RHIDE_COMPILE_PASCAL)
RHIDE_COMPILE.pas.o=$(RHIDE_COMPILE.p.o)
RHIDE_COMPILE.pas.o=$(RHIDE_COMPILE.p.o)
RHIDE_COMPILE.pas.s=$(RHIDE_COMPILE.pas.s.$(PASCAL_TYPE))
RHIDE_COMPILE.pas.s.FPC=$(RHIDE_COMPILE_FPC) -a -s
RHIDE_COMPILE.pas.s.GPC=$(subst -c $(SOURCE_NAME),-S\
	$(SOURCE_NAME),$(RHIDE_COMPILE_GPC))
RHIDE_COMPILE.pp.o=$(RHIDE_COMPILE_FPC)
RHIDE_COMPILE.s.o=$(RHIDE_COMPILE_ASM)
RHIDE_COMPILE_ADA=$(RHIDE_GCC) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS)\
	$(C_OPT_FLAGS)  $(C_WARN_FLAGS) $(C_C_LANG_FLAGS) $(C_EXTRA_FLAGS)\
	$(LOCAL_OPT)  $(C_ADA_LANG_FLAGS)  $(RHIDE_OS_CFLAGS) $(CPPFLAGS)\
	$(CFLAGS) -c $(SOURCE_NAME) -o $(OUTFILE)
RHIDE_COMPILE_ARCHIVE=$(RHIDE_AR) $(RHIDE_ARFLAGS) $(OUTFILE)\
	$(ALL_OBJFILES)
RHIDE_COMPILE_ASM=$(RHIDE_AS) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS)\
	$(C_OPT_FLAGS)  $(C_WARN_FLAGS) $(C_EXTRA_FLAGS) $(LOCAL_OPT)  -c\
	$(SOURCE_NAME) -o $(OUTFILE)
RHIDE_COMPILE_ASM_FORCE=$(RHIDE_AS) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS)\
	$(C_OPT_FLAGS)  $(C_WARN_FLAGS) $(C_EXTRA_FLAGS) -x assembler\
	$(LOCAL_OPT)  -c $(SOURCE_NAME) -o $(OUTFILE)
RHIDE_COMPILE_C=$(RHIDE_GCC) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS)\
	$(C_OPT_FLAGS)  $(C_WARN_FLAGS) $(C_C_LANG_FLAGS) $(C_EXTRA_FLAGS)\
	$(RHIDE_OS_CFLAGS)  $(CPPFLAGS) $(CFLAGS) $(LOCAL_OPT) -c\
	$(SOURCE_NAME) -o $(OUTFILE)
RHIDE_COMPILE_CC=$(RHIDE_GXX) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS)\
	$(C_OPT_FLAGS)  $(C_WARN_FLAGS) $(C_C_LANG_FLAGS)\
	$(C_CXX_LANG_FLAGS) $(C_EXTRA_FLAGS)  $(RHIDE_OS_CXXFLAGS)\
	$(CPPFLAGS) $(CXXFLAGS) $(LOCAL_OPT)   -c $(SOURCE_NAME) -o\
	$(OUTFILE)
RHIDE_COMPILE_CC_FORCE=$(RHIDE_GXX) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS)\
	$(C_OPT_FLAGS)  $(C_WARN_FLAGS) $(C_C_LANG_FLAGS)\
	$(C_CXX_LANG_FLAGS) $(C_EXTRA_FLAGS)  $(RHIDE_OS_CXXFLAGS)\
	$(CPPFLAGS) $(CXXFLAGS) -x c++ $(LOCAL_OPT)   -c $(SOURCE_NAME) -o\
	$(OUTFILE)
RHIDE_COMPILE_C_FORCE=$(RHIDE_GCC) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS)\
	$(C_OPT_FLAGS)  $(C_WARN_FLAGS) $(C_C_LANG_FLAGS) $(C_EXTRA_FLAGS)\
	-x c $(RHIDE_OS_CFLAGS)  $(CPPFLAGS) $(CFLAGS) $(LOCAL_OPT) -c\
	$(SOURCE_NAME) -o $(OUTFILE)
RHIDE_COMPILE_FORTRAN=$(RHIDE_G77) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS)\
	$(C_OPT_FLAGS)  $(C_WARN_FLAGS) $(C_F_LANG_FLAGS) $(C_EXTRA_FLAGS)\
	$(LOCAL_OPT)  -c $(SOURCE_NAME) -o $(OUTFILE)
RHIDE_COMPILE_FORTRAN_FORCE=$(RHIDE_G77) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS)\
	$(C_OPT_FLAGS)  $(C_WARN_FLAGS) $(C_F_LANG_FLAGS) $(C_EXTRA_FLAGS)\
	-x f77 $(LOCAL_OPT)  -c $(SOURCE_NAME) -o $(OUTFILE)
RHIDE_COMPILE_FPC=$(RHIDE_FPC) $(RHIDE_FPC_FLAGS) -E- $(SOURCE_NAME)
RHIDE_COMPILE_FPC_FORCE=$(RHIDE_FPC) $(RHIDE_FPC_FLAGS) -B -E-\
	$(SOURCE_NAME)
RHIDE_COMPILE_GPC=$(RHIDE_GPC) $(RHIDE_GPC_FLAGS) $(LOCAL_OPT)  -c\
	$(SOURCE_NAME) -o $(OUTFILE)
RHIDE_COMPILE_GPC_FORCE=$(RHIDE_GPC) $(RHIDE_GPC_FLAGS) -x pascal\
	$(LOCAL_OPT)  -c $(SOURCE_NAME) -o $(OUTFILE)
RHIDE_COMPILE_LINK=$(RHIDE_LD) $(RHIDE_LIBDIRS) $(C_EXTRA_FLAGS) -o\
	$(OUTFILE)  $(OBJFILES) $(LIBRARIES) $(LDFLAGS) $(RHIDE_LDFLAGS)\
	$(RHIDE_LIBS)
RHIDE_COMPILE_LINK_ADA=gnatbl $(RHIDE_LIBDIRS) $(C_EXTRA_FLAGS) -o\
	$(OUTFILE)  $(setsuffix .ali,$(OUTFILE)) $(LIBRARIES) $(LDFLAGS) \
	$(RHIDE_LDFLAGS) $(RHIDE_LIBS)
RHIDE_COMPILE_LINK_ADA_BIND=$(RHIDE_GNATBIND) -o $(RHIDE_ADA_BIND_FILE)\
	$(setsuffix .ali,$(OUTFILE))
RHIDE_COMPILE_LINK_ADA_LINK=$(RHIDE_LD) $(RHIDE_LIBDIRS) $(C_EXTRA_FLAGS) -o\
	$(OUTFILE)  $(RHIDE_ADA_BIND_FILE) $(OBJFILES) $(LIBRARIES)\
	$(LDFLAGS)  $(RHIDE_LDFLAGS) $(RHIDE_LIBS)
RHIDE_COMPILE_LINK_DLL=$(RHIDE_LD) $(RHIDE_LIBDIRS) $(C_EXTRA_FLAGS) -o\
	$(OUTFILE)  $(OBJFILES) $(LIBRARIES) $(LDFLAGS) $(RHIDE_LDFLAGS)  \
	$(RHIDE_SHARED_LDFLAGS) $(RHIDE_LIBS)
RHIDE_COMPILE_LINK_FPC=echo 'separate linking for FPK is not supported.\
	Please define a main source file in Project/Primary file.' 1>&2
RHIDE_COMPILE_LINK_FPC_AUTOMAKE=$(RHIDE_FPC) -o$(OUTFILE) $(SOURCE_NAME)\
	$(RHIDE_FPC_FLAGS) -E+
RHIDE_COMPILE_LINK_GPC=$(RHIDE_LD_PASCAL) $(RHIDE_LIBDIRS) $(C_EXTRA_FLAGS)\
	-o $(OUTFILE)  $(OBJFILES) $(LIBRARIES) $(RHIDE_LDFLAGS) $(LDFLAGS)\
	$(RHIDE_LIBS)
RHIDE_COMPILE_LINK_GPC_AUTOMAKE=$(RHIDE_LD_PASCAL) $(RHIDE_LIBDIRS)  -o\
	$(OUTFILE) --automake $(RHIDE_GPC_FLAGS)  $(SOURCE_NAME)\
	$(LIBRARIES) $(LDFLAGS) $(RHIDE_LDFLAGS) $(RHIDE_LIBS)
RHIDE_COMPILE_LINK_PASCAL=$(RHIDE_COMPILE_LINK_$(PASCAL_TYPE))
RHIDE_COMPILE_LINK_PASCAL_AUTOMAKE=$(RHIDE_COMPILE_LINK_$(PASCAL_TYPE)_AUTOMAKE)
RHIDE_COMPILE_NASM=$(RHIDE_NASM) -f $(RHIDE_NASM_TARGET) $(LOCAL_OPT) -o\
	$(OUTFILE)  $(SOURCE_NAME)
RHIDE_COMPILE_PASCAL=$(RHIDE_COMPILE_$(PASCAL_TYPE))
RHIDE_COMPILE_PASCAL_FORCE=$(RHIDE_COMPILE_$(PASCAL_TYPE)_FORCE)
RHIDE_CONFIG_DIRS=. $(RHIDE_SHARE) $(GET_HOME)   $(RHIDE_CONFIG_DIRS_COMMON)\
	 $(addsuffix /SET,$(RHIDE_CONFIG_DIRS_COMMON))  $(SET_FILES)
RHIDE_CONFIG_DIRS_$(RHIDE_OS)=/usr/local/share/rhide /usr/share/rhide \
	/local/share/rhide /share/rhide
RHIDE_CONFIG_DIRS_COMMON=$(RHIDE_CONFIG_DIRS_$(RHIDE_OS))\
	$(RHIDE_BIN_DIR)/../share/rhide
RHIDE_CONFIG_DIRS_DJGPP=$(DJDIR)/share/rhide
RHIDE_CONFIG_DIRS_DJGPP=/usr/local/share/rhide /usr/share/rhide \
	/local/share/rhide /share/rhide
RHIDE_EMPTY=
RHIDE_FPC=ppc386
RHIDE_FPC_FLAGS=$(C_FPC_LANG_FLAGS) $(LOCAL_OPT) $(addprefix\
	-Up,$(INCLUDE_DIRS))  $(C_EXTRA_FLAGS)
RHIDE_FPC_LIBDIRS=$(RHIDE_FPC_LIBDIRS_$(RHIDE_OS))
RHIDE_FPC_LIBDIRS_$(RHIDE_OS)=/usr/local/lib /usr/lib /lib
RHIDE_FPC_LIBDIRS_DJGPP=$(DJDIR)/lib
RHIDE_FPC_LIBDIRS_DJGPP=/usr/local/lib /usr/lib /lib
RHIDE_FPC_LINK_FLAGS=$(RHIDE_FPC_LINK_FLAGS_$(RHIDE_OS))
RHIDE_FPC_LINK_FLAGS_$(RHIDE_OS)=$(RHIDE_LIBDIRS) $(addprefix\
	-L,$(RHIDE_FPC_LIBDIRS))
RHIDE_FPC_LINK_FLAGS_DJGPP=-O coff-go32-exe $(RHIDE_LIBDIRS) $(addprefix\
	-L,$(RHIDE_FPC_LIBDIRS))
RHIDE_FPC_LINK_FLAGS_DJGPP=$(RHIDE_LIBDIRS) $(addprefix\
	-L,$(RHIDE_FPC_LIBDIRS))
RHIDE_FSDB=fsdb $(OUTFILE) $(addprefix -p ,$(SRC_DIRS)) $(PROG_ARGS)
RHIDE_G77=g77
RHIDE_GCC=gcc
RHIDE_GDB=gdb $(OUTFILE) $(addprefix -d ,$(SRC_DIRS))
RHIDE_GNATBIND=gnatbind
RHIDE_GPC=gpc
RHIDE_GPC_FLAGS=$(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS)\
	$(C_WARN_FLAGS)  $(C_P_LANG_FLAGS) $(C_EXTRA_FLAGS)
RHIDE_GPROF=gprof $(OUTFILE)
RHIDE_GREP=grep -n $(prompt arguments for GREP,$(WUC) $(DEFAULT_GREP_MASK))
RHIDE_GXX=$(RHIDE_GCC)
RHIDE_INCLUDES=$(SPECIAL_CFLAGS) $(addprefix -I,$(INCLUDE_DIRS))
RHIDE_LD=$(RHIDE_GCC)
RHIDE_LDFLAGS=$(SPECIAL_LDFLAGS) $(addprefix -Xlinker ,$(LD_EXTRA_FLAGS))
RHIDE_LD_FPC=$(RHIDE_FPC) -E+
RHIDE_LD_PASCAL=gpc
RHIDE_LIBDIRS=$(addprefix -L,$(LIB_DIRS))
RHIDE_LIBS=$(addprefix -l,$(LIBS) $(RHIDE_TYPED_LIBS) $(RHIDE_OS_LIBS))
RHIDE_NASM=nasm
RHIDE_NASM_TARGET=$(RHIDE_NASM_TARGET_$(RHIDE_OS))
RHIDE_NASM_TARGET_DJGPP=coff
RHIDE_NASM_TARGET_Linux=elf
RHIDE_PATH_SEPARATOR=$(RHIDE_PATH_SEPARATOR_$(RHIDE_OS))
RHIDE_PATH_SEPARATOR_$(RHIDE_OS)=:
RHIDE_PATH_SEPARATOR_DJGPP=;
RHIDE_PATH_SEPARATOR_DJGPP=:
RHIDE_RLOG=$(shell rlog -R $(rlog_arg))
RHIDE_RM=rm
RHIDE_SHARED_LDFLAGS=$(RHIDE_SHARED_LDFLAGS_$(RHIDE_OS))
RHIDE_SHARED_LDFLAGS_$(RHIDE_OS)=
RHIDE_SHARED_LDFLAGS_DJGPP=
RHIDE_SHARED_LDFLAGS_Linux=-shared
RHIDE_SPACE=$(RHIDE_EMPTY) $(RHIDE_EMPTY)
RHIDE_STANDARD_INCLUDES=$(RHIDE_STANDARD_INCLUDES_$(RHIDE_OS))
RHIDE_STANDARD_INCLUDES_$(RHIDE_OS)=$(addprefix /usr/,include include/sys\
	include/g++ include/g++/std)
RHIDE_STANDARD_INCLUDES_DJGPP=$(addprefix $(DJDIR)/,include include/sys\
	lang/cxx lang/cxx/std)
RHIDE_STANDARD_INCLUDES_DJGPP=$(addprefix /usr/,include include/sys\
	include/g++ include/g++/std)
RHIDE_TYPED_LIBS=$(foreach\
	suff,$(RHIDE_TYPED_LIBS_SUFFIXES),$(RHIDE_TYPED_LIBS$(suff)))
RHIDE_TYPED_LIBS.C=$(RHIDE_TYPED_LIBS.cc)
RHIDE_TYPED_LIBS.F=$(RHIDE_TYPED_LIBS.f)
RHIDE_TYPED_LIBS.adb=gnat
RHIDE_TYPED_LIBS.cc=$(RHIDE_TYPED_LIBS_$(RHIDE_OS).cc)
RHIDE_TYPED_LIBS.cpp=$(RHIDE_TYPED_LIBS.cc)
RHIDE_TYPED_LIBS.cxx=$(RHIDE_TYPED_LIBS.cc)
RHIDE_TYPED_LIBS.f=g2c m
RHIDE_TYPED_LIBS.f=g2c m
RHIDE_TYPED_LIBS.for=$(RHIDE_TYPED_LIBS.f)
RHIDE_TYPED_LIBS.fpp=$(RHIDE_TYPED_LIBS.f)
RHIDE_TYPED_LIBS.ii=$(RHIDE_TYPED_LIBS.cc)
RHIDE_TYPED_LIBS.l=fl
RHIDE_TYPED_LIBS.m=objc
RHIDE_TYPED_LIBS.p=$(RHIDE_TYPED_LIBS_$(PASCAL_TYPE))
RHIDE_TYPED_LIBS.pas=$(RHIDE_TYPED_LIBS.p)
RHIDE_TYPED_LIBS.pp=$(RHIDE_TYPED_LIBS_FPC)
RHIDE_TYPED_LIBS_$(RHIDE_OS).cc=stdc++ m
RHIDE_TYPED_LIBS_DJGPP.cc=stdcxx m
RHIDE_TYPED_LIBS_DJGPP.cc=stdcxx m
RHIDE_TYPED_LIBS_DJGPP.cc=stdcxx m
RHIDE_TYPED_LIBS_DJGPP.cpp=stdcxx m
RHIDE_TYPED_LIBS_DJGPP.cxx=stdcxx m
RHIDE_TYPED_LIBS_FPC=fpc
RHIDE_TYPED_LIBS_GPC=gpc m
RHIDE_TYPED_LIBS_SUFFIXES=$(sort $(foreach item,$(PROJECT_ITEMS),$(suffix\
	$(item))))
_RHIDE_COMPILE_LINK_ADA=$(RHIDE_COMPILE_LINK_ADA_BIND);\
	$(RHIDE_COMPILE_LINK_ADA_LINK);   $(RHIDE_RM)\
	$(RHIDE_ADA_BIND_FILE)
%.o: %.c
	$(RHIDE_COMPILE.c.o)
%.o: %.i
	$(RHIDE_COMPILE_C)
%.o: %.cc
	$(RHIDE_COMPILE.cc.o)
%.o: %.cpp
	$(RHIDE_COMPILE.cpp.o)
%.o: %.cxx
	$(RHIDE_COMPILE.cxx.o)
%.o: %.C
	$(RHIDE_COMPILE.C.o)
%.o: %.ii
	$(RHIDE_COMPILE_CC)
%.o: %.s
	$(RHIDE_COMPILE.s.o)
%.o: %.S
	$(RHIDE_COMPILE_ASM)
%.s: %.c
	$(RHIDE_COMPILE.c.s)
%.s: %.i
	$(RHIDE_COMPILE.i.s)
%.s: %.cc
	$(RHIDE_COMPILE.cc.s)
%.s: %.cpp
	$(RHIDE_COMPILE.cpp.s)
%.s: %.cxx
	$(RHIDE_COMPILE.cxx.s)
%.s: %.C
	$(RHIDE_COMPILE.C.s)
%.o: %.pas
	$(RHIDE_COMPILE.pas.o)
%.o: %.p
	$(RHIDE_COMPILE.p.o)
%.o: %.pp
	$(RHIDE_COMPILE.pp.o)
%.s: %.pas
	$(RHIDE_COMPILE.pas.s)
%.o: %.m
	$(RHIDE_COMPILE_OBJC)
%.o: %.f
	$(RHIDE_COMPILE.f.o)
%.o: %.for
	$(RHIDE_COMPILE.for.o)
%.o: %.F
	$(RHIDE_COMPILE.F.o)
%.o: %.fpp
	$(RHIDE_COMPILE.fpp.o)
%.o: %.asm
	$(RHIDE_COMPILE.asm.o)
%.o: %.nsm
	$(RHIDE_COMPILE.nsm.o)
%.o: %.adb
	$(RHIDE_COMPILE.adb.o)
%.i: %.c
	$(RHIDE_COMPILE.c.i)
%.s: %.c
	$(RHIDE_COMPILE.c.s)
%.ii: %.cc
	$(RHIDE_COMPILE.cc.ii)
%.s: %.cc
	$(RHIDE_COMPILE.cc.s)
%.ii: %.cpp
	$(RHIDE_COMPILE.cpp.ii)
%.s: %.cpp
	$(RHIDE_COMPILE.cpp.s)
%.ii: %.cxx
	$(RHIDE_COMPILE.cxx.ii)
%.s: %.cxx
	$(RHIDE_COMPILE.cxx.s)
%.ii: %.C
	$(RHIDE_COMPILE.C.ii)
%.s: %.C
	$(RHIDE_COMPILE.C.s)
include rhide.env
all::
clean::
	rm -f $(CLEAN_FILES)
DEPS_0= commands.o ideclass.o names/ndepcoll.o names/ndepende.o\
	names/nflagcol.o names/nflagent.o names/nideedit.o names/nidefile.o\
	names/noptions.o names/nproject.o stream/sdepcoll.o\
	stream/sdepende.o stream/sflagcol.o stream/sflagent.o\
	stream/sideedit.o stream/sidefile.o stream/soptions.o\
	stream/sproject.o tdepende.o tflagcol.o tflagent.o tflaglis.o\
	tfname.o tideedit.o tidefile.o toptions.o tprjlist.o tprjwind.o\
	tproject.o 
NO_LINK=
LINK_FILES=$(filter-out $(NO_LINK),$(DEPS_0))
libide.a:: $(DEPS_0)
	$(RHIDE_COMPILE_ARCHIVE)
DEPS_1=commands.cc idecomma.h libide.h rhutils.h libtvuti.h tvutilfu.h
commands.o:: $(DEPS_1)
	$(RHIDE_COMPILE.cc.o)
DEPS_2=ideclass.cc ideenums.h idefunct.h libide.h libtvuti.h
ideclass.o:: $(DEPS_2)
	$(RHIDE_COMPILE.cc.o)
DEPS_3=
.PHONY: names/names.gpr.force
all:: names/names.gpr.force
names/names.gpr.force:
	$(MAKE) -C names/ -f names.mak $(FLAGS_FOR_SUBPROJECTS)
clean::
	$(MAKE) -C names/ -f names.mak $(FLAGS_FOR_SUBPROJECTS) clean

DEPS_4=
.PHONY: stream/stream.gpr.force
all:: stream/stream.gpr.force
stream/stream.gpr.force:
	$(MAKE) -C stream/ -f stream.mak $(FLAGS_FOR_SUBPROJECTS)
clean::
	$(MAKE) -C stream/ -f stream.mak $(FLAGS_FOR_SUBPROJECTS) clean

DEPS_5=tdepende.cc ideenums.h libide.h tdepende.h tfname.h rhutils.h\
	libtvuti.h tparamli.h
tdepende.o:: $(DEPS_5)
	$(RHIDE_COMPILE.cc.o)
DEPS_6=tflagcol.cc libide.h tflagcol.h tflagent.h rhutils.h libtvuti.h
tflagcol.o:: $(DEPS_6)
	$(RHIDE_COMPILE.cc.o)
DEPS_7=tflagent.cc libide.h tflagent.h rhutils.h libtvuti.h tvutilfu.h
tflagent.o:: $(DEPS_7)
	$(RHIDE_COMPILE.cc.o)
DEPS_8=tflaglis.cc idecomma.h libide.h tflagcol.h tflagent.h tflaglis.h\
	rhutils.h libtvuti.h
tflaglis.o:: $(DEPS_8)
	$(RHIDE_COMPILE.cc.o)
DEPS_9=tfname.cc libide.h tfname.h rhutils.h libtvuti.h
tfname.o:: $(DEPS_9)
	$(RHIDE_COMPILE.cc.o)
DEPS_10=tideedit.cc idecomma.h libide.h tideedit.h tidefile.h rhutils.h\
	libtvuti.h
tideedit.o:: $(DEPS_10)
	$(RHIDE_COMPILE.cc.o)
DEPS_11=tidefile.cc idecomma.h ideenums.h idefunct.h libide.h tideedit.h\
	tidefile.h rhutils.h libtvuti.h
tidefile.o:: $(DEPS_11)
	$(RHIDE_COMPILE.cc.o)
DEPS_12=toptions.cc libide.h tflagcol.h toptions.h libtvuti.h tdirlist.h\
	tparamli.h
toptions.o:: $(DEPS_12)
	$(RHIDE_COMPILE.cc.o)
DEPS_13=tprjlist.cc idecomma.h ideenums.h idefunct.h libide.h tdepende.h\
	tfname.h tideedit.h tprjlist.h libtvuti.h tvutilco.h
tprjlist.o:: $(DEPS_13)
	$(RHIDE_COMPILE.cc.o)
DEPS_14=tprjwind.cc idecomma.h libide.h tdepende.h tprjlist.h tprjwind.h\
	libtvuti.h
tprjwind.o:: $(DEPS_14)
	$(RHIDE_COMPILE.cc.o)
DEPS_15=tproject.cc ideenums.h libide.h tdepende.h tfname.h toptions.h\
	tproject.h rhutils.h libtvuti.h tparamli.h
tproject.o:: $(DEPS_15)
	$(RHIDE_COMPILE.cc.o)
all:: libide.a
