/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/utsname.h>
#define Uses_TWindow
#define Uses_TEvent
#define Uses_TStringCollection
#define Uses_TProject
#define Uses_TOptions
#define Uses_TDependency
#define Uses_TDepCollection
#define Uses_TFlagCollection
#define Uses_TCEditor_Internal
#define Uses_TWindowList
#define Uses_TSCollection
#define Uses_ideFunctions
#define Uses_ideCommands
#define Uses_TParamList
#define Uses_TDirList
#define Uses_TWindowList
#define Uses_tvutilFunctions
#include <ceditor.h>
#include "rhide.h"
#include "rhidehis.h"

#include <libide.h>

#include <libtvuti.h>
#include <rhutils.h>


/* *INDENT-OFF* */
#define SAME_LIBS(x,y)\
 "RHIDE_TYPED_LIBS."#x,\
 "$(RHIDE_TYPED_LIBS."#y")"

#define SAME_COMPILE(from,to,ref) \
 "RHIDE_COMPILE."#from"."#to,\
 "$(RHIDE_COMPILE."#ref"."#to")"

static char *default_variables[] = {
 "RHIDE_GCC",
 "gcc",

 "RHIDE_AS",
 "$(RHIDE_GCC)",

 "RHIDE_GXX",
 "$(RHIDE_GCC)",

 "RHIDE_GPC",
 "gpc",

 "RHIDE_FPC",
 "ppc386",

 "RHIDE_AR",
 "ar",

 "RHIDE_LD",
 "$(RHIDE_GCC)",

 "RHIDE_G77",
 "g77",

 "RHIDE_NASM",
 "nasm",

 "RHIDE_LD_PASCAL",
 "gpc",

 "RHIDE_LD_FPC",
 "$(RHIDE_FPC) -E+",

 "RHIDE_GNATBIND",
 "gnatbind",

 "RHIDE_RM",
 "rm",

 "RHIDE_ARFLAGS",
 "rcs",

 "RHIDE_TYPED_LIBS.f",
 "g2c m",

 SAME_LIBS(for,f),
 SAME_LIBS(F,f),
 SAME_LIBS(fpp,f),

 "RHIDE_TYPED_LIBS_GPC",
 "gpc m",

 "RHIDE_TYPED_LIBS_FPC",
 "fpc",

 "RHIDE_TYPED_LIBS.p",
 "$(RHIDE_TYPED_LIBS_$(PASCAL_TYPE))",

 SAME_LIBS(pas,p),

 "RHIDE_TYPED_LIBS.pp",
 "$(RHIDE_TYPED_LIBS_FPC)",

 "RHIDE_TYPED_LIBS_$(RHIDE_OS).cc",
 "stdc++ m",

 "RHIDE_TYPED_LIBS_DJGPP.cc",
 "stdcxx m",

 "RHIDE_TYPED_LIBS.cc",
 "$(RHIDE_TYPED_LIBS_$(RHIDE_OS).cc)",

 SAME_LIBS(cpp,cc),
 SAME_LIBS(cxx,cc),
 SAME_LIBS(C,cc),
 SAME_LIBS(ii,cc),

 "RHIDE_TYPED_LIBS.l",
 "fl",

 "RHIDE_TYPED_LIBS.m",
 "objc",

 "RHIDE_TYPED_LIBS.adb",
 "gnat",

 "RHIDE_TYPED_LIBS_SUFFIXES",
 "$(sort $(foreach item,$(PROJECT_ITEMS),$(suffix $(item))))",

 "RHIDE_TYPED_LIBS",
 "$(foreach suff,$(RHIDE_TYPED_LIBS_SUFFIXES),$(RHIDE_TYPED_LIBS$(suff)))",

 "RHIDE_INCLUDES",
 "$(SPECIAL_CFLAGS) $(addprefix -I,$(INCLUDE_DIRS))",

 "RHIDE_LIBDIRS",
 "$(addprefix -L,$(LIB_DIRS))",

 "RHIDE_LIBS",
 "$(addprefix -l,$(LIBS) $(RHIDE_TYPED_LIBS) $(RHIDE_OS_LIBS))",

 "RHIDE_LDFLAGS",
 "$(SPECIAL_LDFLAGS) $(addprefix -Xlinker ,$(LD_EXTRA_FLAGS))",

 "RHIDE_NASM_TARGET_DJGPP",
 "coff",

 "RHIDE_NASM_TARGET_Linux",
 "elf",

 "RHIDE_NASM_TARGET",
 "$(RHIDE_NASM_TARGET_$(RHIDE_OS))",

 "RHIDE_COMPILE_NASM",
 "$(RHIDE_NASM) -f $(RHIDE_NASM_TARGET) $(LOCAL_OPT) -o $(OUTFILE)\
  $(SOURCE_NAME)",

 "RHIDE_COMPILE_FORTRAN",
 "$(RHIDE_G77) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS)\
  $(C_WARN_FLAGS) $(C_F_LANG_FLAGS) $(C_EXTRA_FLAGS) $(LOCAL_OPT)\
  -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_COMPILE_FORTRAN_FORCE",
 "$(RHIDE_G77) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS)\
  $(C_WARN_FLAGS) $(C_F_LANG_FLAGS) $(C_EXTRA_FLAGS) -x f77 $(LOCAL_OPT)\
  -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_COMPILE_C",
 "$(RHIDE_GCC) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS)\
  $(C_WARN_FLAGS) $(C_C_LANG_FLAGS) $(C_EXTRA_FLAGS) $(RHIDE_OS_CFLAGS)\
  $(CPPFLAGS) $(CFLAGS) $(LOCAL_OPT) -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_COMPILE_C_FORCE",
 "$(RHIDE_GCC) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS)\
  $(C_WARN_FLAGS) $(C_C_LANG_FLAGS) $(C_EXTRA_FLAGS) -x c $(RHIDE_OS_CFLAGS)\
  $(CPPFLAGS) $(CFLAGS) $(LOCAL_OPT) -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_COMPILE_CC",
 "$(RHIDE_GXX) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS)\
  $(C_WARN_FLAGS) $(C_C_LANG_FLAGS) $(C_CXX_LANG_FLAGS) $(C_EXTRA_FLAGS)\
  $(RHIDE_OS_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(LOCAL_OPT) \
  -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_COMPILE_CC_FORCE",
 "$(RHIDE_GXX) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS)\
  $(C_WARN_FLAGS) $(C_C_LANG_FLAGS) $(C_CXX_LANG_FLAGS) $(C_EXTRA_FLAGS)\
  $(RHIDE_OS_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS) -x c++ $(LOCAL_OPT) \
  -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_COMPILE_ASM",
 "$(RHIDE_AS) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS)\
  $(C_WARN_FLAGS) $(C_EXTRA_FLAGS) $(LOCAL_OPT)\
  -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_COMPILE_ASM_FORCE",
 "$(RHIDE_AS) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS)\
  $(C_WARN_FLAGS) $(C_EXTRA_FLAGS) -x assembler $(LOCAL_OPT)\
  -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_GPC_FLAGS",
 "$(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS) $(C_WARN_FLAGS)\
  $(C_P_LANG_FLAGS) $(C_EXTRA_FLAGS)",

 "RHIDE_COMPILE_GPC",
 "$(RHIDE_GPC) $(RHIDE_GPC_FLAGS) $(LOCAL_OPT)\
  -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_COMPILE_GPC_FORCE",
 "$(RHIDE_GPC) $(RHIDE_GPC_FLAGS) -x pascal $(LOCAL_OPT)\
  -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_FPC_FLAGS",
 "$(C_FPC_LANG_FLAGS) $(LOCAL_OPT) $(addprefix -Up,$(INCLUDE_DIRS))\
  $(C_EXTRA_FLAGS)",

 "RHIDE_COMPILE_FPC",
 "$(RHIDE_FPC) $(RHIDE_FPC_FLAGS) -E- $(SOURCE_NAME)",
 
 "RHIDE_SHARED_LDFLAGS_$(RHIDE_OS)",
 "",
 
 "RHIDE_SHARED_LDFLAGS_Linux",
 "-shared",

 "RHIDE_SHARED_LDFLAGS",
 "$(RHIDE_SHARED_LDFLAGS_$(RHIDE_OS))",

 "RHIDE_COMPILE_FPC_FORCE",
 "$(RHIDE_FPC) $(RHIDE_FPC_FLAGS) -B -E- $(SOURCE_NAME)",

 "RHIDE_COMPILE_LINK_DLL",
 "$(RHIDE_LD) $(RHIDE_LIBDIRS) $(C_EXTRA_FLAGS) -o $(OUTFILE)\
  $(OBJFILES) $(LIBRARIES) $(LDFLAGS) $(RHIDE_LDFLAGS) \
  $(RHIDE_SHARED_LDFLAGS) $(RHIDE_LIBS)",


 "RHIDE_COMPILE_LINK",
 "$(RHIDE_LD) $(RHIDE_LIBDIRS) $(C_EXTRA_FLAGS) -o $(OUTFILE)\
  $(OBJFILES) $(LIBRARIES) $(LDFLAGS) $(RHIDE_LDFLAGS) $(RHIDE_LIBS)",

 "RHIDE_COMPILE_LINK_GPC",
 "$(RHIDE_LD_PASCAL) $(RHIDE_LIBDIRS) $(C_EXTRA_FLAGS) -o $(OUTFILE)\
  $(OBJFILES) $(LIBRARIES) $(RHIDE_LDFLAGS) $(LDFLAGS) $(RHIDE_LIBS)",

 "RHIDE_COMPILE_LINK_GPC_AUTOMAKE",
 "$(RHIDE_LD_PASCAL) $(RHIDE_LIBDIRS)\
  -o $(OUTFILE) --automake $(RHIDE_GPC_FLAGS)\
  $(SOURCE_NAME) $(LIBRARIES) $(LDFLAGS) $(RHIDE_LDFLAGS) $(RHIDE_LIBS)",

 "RHIDE_COMPILE_PASCAL",
 "$(RHIDE_COMPILE_$(PASCAL_TYPE))",

 "RHIDE_COMPILE_PASCAL_FORCE",
 "$(RHIDE_COMPILE_$(PASCAL_TYPE)_FORCE)",

 "RHIDE_COMPILE_LINK_PASCAL_AUTOMAKE",
 "$(RHIDE_COMPILE_LINK_$(PASCAL_TYPE)_AUTOMAKE)",

 "RHIDE_COMPILE_LINK_PASCAL",
 "$(RHIDE_COMPILE_LINK_$(PASCAL_TYPE))",

 "RHIDE_FPC_LIBDIRS_$(RHIDE_OS)",
 "/usr/local/lib /usr/lib /lib",

 "RHIDE_FPC_LIBDIRS_DJGPP",
 "$(DJDIR)/lib",

 "RHIDE_FPC_LIBDIRS",
 "$(RHIDE_FPC_LIBDIRS_$(RHIDE_OS))",

 "RHIDE_FPC_LINK_FLAGS_$(RHIDE_OS)",
 "$(RHIDE_LIBDIRS) $(addprefix -L,$(RHIDE_FPC_LIBDIRS))",

 "RHIDE_FPC_LINK_FLAGS_DJGPP",
 "-O coff-go32-exe $(RHIDE_LIBDIRS) $(addprefix -L,$(RHIDE_FPC_LIBDIRS))",

 "RHIDE_FPC_LINK_FLAGS",
 "$(RHIDE_FPC_LINK_FLAGS_$(RHIDE_OS))",

 "RHIDE_COMPILE_LINK_FPC",
 "echo 'separate linking for FPK is not supported. Please define"
 " a main source file in Project/Primary file.' 1>&2",

 "RHIDE_COMPILE_LINK_FPC_AUTOMAKE",
 "$(RHIDE_FPC) -o$(OUTFILE) $(SOURCE_NAME) $(RHIDE_FPC_FLAGS) -E+",

 "RHIDE_COMPILE_ARCHIVE",
 "$(RHIDE_AR) $(RHIDE_ARFLAGS) $(OUTFILE) $(ALL_OBJFILES)",

 "RHIDE_COMPILE_ADA",
 "$(RHIDE_GCC) $(RHIDE_INCLUDES) $(C_DEBUG_FLAGS) $(C_OPT_FLAGS)\
  $(C_WARN_FLAGS) $(C_C_LANG_FLAGS) $(C_EXTRA_FLAGS) $(LOCAL_OPT)\
  $(C_ADA_LANG_FLAGS)\
  $(RHIDE_OS_CFLAGS) $(CPPFLAGS) $(CFLAGS) -c $(SOURCE_NAME) -o $(OUTFILE)",

 "RHIDE_ADA_BIND_FILE",
 "$(addprefix _,$(setsuffix .c,$(OUTFILE)))",

 "RHIDE_COMPILE_LINK_ADA_BIND",
 "$(RHIDE_GNATBIND) -o $(RHIDE_ADA_BIND_FILE) $(setsuffix .ali,$(OUTFILE))",

 "RHIDE_COMPILE_LINK_ADA_LINK",
 "$(RHIDE_LD) $(RHIDE_LIBDIRS) $(C_EXTRA_FLAGS) -o $(OUTFILE)\
  $(RHIDE_ADA_BIND_FILE) $(OBJFILES) $(LIBRARIES) $(LDFLAGS)\
  $(RHIDE_LDFLAGS) $(RHIDE_LIBS)",

 "_RHIDE_COMPILE_LINK_ADA",
 "$(RHIDE_COMPILE_LINK_ADA_BIND); $(RHIDE_COMPILE_LINK_ADA_LINK);\
   $(RHIDE_RM) $(RHIDE_ADA_BIND_FILE)",

 "RHIDE_COMPILE_LINK_ADA",
 "gnatbl $(RHIDE_LIBDIRS) $(C_EXTRA_FLAGS) -o $(OUTFILE)\
  $(setsuffix .ali,$(OUTFILE)) $(LIBRARIES) $(LDFLAGS)\
  $(RHIDE_LDFLAGS) $(RHIDE_LIBS)",

 "RHIDE_COMPILE.c.o",
 "$(RHIDE_COMPILE_C)",

 "RHIDE_COMPILE.cc.o",
 "$(RHIDE_COMPILE_CC)",

 "RHIDE_COMPILE.p.o",
 "$(RHIDE_COMPILE_PASCAL)",

 SAME_COMPILE(pas,o,p),

 /* .pp is a fpc specific extension */
 "RHIDE_COMPILE.pp.o",
 "$(RHIDE_COMPILE_FPC)",

 "RHIDE_COMPILE.pas.s.GPC",
 "$(subst -c $(SOURCE_NAME),-S $(SOURCE_NAME),$(RHIDE_COMPILE_GPC))",

 "RHIDE_COMPILE.pas.s.FPC",
 "$(RHIDE_COMPILE_FPC) -a -s",

 "RHIDE_COMPILE.pas.s",
 "$(RHIDE_COMPILE.pas.s.$(PASCAL_TYPE))",

 "RHIDE_COMPILE.f.o",
 "$(RHIDE_COMPILE_FORTRAN)",

 "RHIDE_COMPILE.nsm.o",
 "$(RHIDE_COMPILE_NASM)",

 "RHIDE_COMPILE.s.o",
 "$(RHIDE_COMPILE_ASM)",

 "RHIDE_COMPILE.c.s",
 "$(subst -c $(SOURCE_NAME),-S $(SOURCE_NAME),$(RHIDE_COMPILE_C))",

 "RHIDE_COMPILE.c.i",
 "$(subst -c $(SOURCE_NAME),-E $(SOURCE_NAME),$(RHIDE_COMPILE_C))",

 "RHIDE_COMPILE.i.s",
 "$(RHIDE_COMPILE.c.s)",

 "RHIDE_COMPILE.cc.s",
 "$(subst -c $(SOURCE_NAME),-S $(SOURCE_NAME),$(RHIDE_COMPILE_CC))",

 "RHIDE_COMPILE.cc.ii",
 "$(subst -c $(SOURCE_NAME),-E $(SOURCE_NAME),$(RHIDE_COMPILE_CC))",

 "RHIDE_COMPILE.ii.s",
 "$(RHIDE_COMPILE.cc.s)",

 SAME_COMPILE(cpp,o,cc),
 SAME_COMPILE(cxx,o,cc),
 SAME_COMPILE(C,o,cc),

 SAME_COMPILE(pas,o,p),

 SAME_COMPILE(for,o,f),
 SAME_COMPILE(F,o,f),
 SAME_COMPILE(fpp,o,f),

 SAME_COMPILE(asm,o,nsm),

 SAME_COMPILE(cpp,s,cc),
 SAME_COMPILE(cxx,s,cc),
 SAME_COMPILE(C,s,cc),

 SAME_COMPILE(cpp,ii,cc),
 SAME_COMPILE(cxx,ii,cc),
 SAME_COMPILE(C,ii,cc),

 "RHIDE_COMPILE.adb.o",
 "$(RHIDE_COMPILE_ADA)",

 "RHIDE_FSDB",
 "fsdb $(OUTFILE) $(addprefix -p ,$(SRC_DIRS)) $(PROG_ARGS)",

 "RHIDE_GDB",
 "gdb $(OUTFILE) $(addprefix -d ,$(SRC_DIRS))",

 "DEFAULT_GREP_MASK",
 "*.[cfhmnps]*",

 "RHIDE_GREP",
 "grep -n $(prompt "__("arguments for GREP")",$(WUC) $(DEFAULT_GREP_MASK))",

 "RHIDE_GPROF",
 "gprof $(OUTFILE)",

 "RHIDE_RLOG",
 "$(shell rlog -R $(rlog_arg))",

 "RHIDE_CO",
 "$(shell co -q $(co_arg))",

 "RHIDE_STANDARD_INCLUDES_$(RHIDE_OS)",
 "$(addprefix /usr/,include include/sys include/g++ include/g++/std)",

 "RHIDE_STANDARD_INCLUDES_DJGPP",
 "$(addprefix $(DJDIR)/,include include/sys lang/cxx lang/cxx/std)",

 "RHIDE_STANDARD_INCLUDES",
 "$(RHIDE_STANDARD_INCLUDES_$(RHIDE_OS))",

 "RHIDE_CONFIG_DIRS_$(RHIDE_OS)",
 "/usr/local/share/rhide /usr/share/rhide\
  /local/share/rhide /share/rhide",

 "RHIDE_CONFIG_DIRS_DJGPP",
 "$(DJDIR)/share/rhide",

 "RHIDE_CONFIG_DIRS_COMMON",
 "$(RHIDE_CONFIG_DIRS_$(RHIDE_OS)) $(RHIDE_BIN_DIR)/../share/rhide",

 "RHIDE_CONFIG_DIRS",
 ". $(RHIDE_SHARE) $(GET_HOME) \
  $(RHIDE_CONFIG_DIRS_COMMON)\
  $(addsuffix /SET,$(RHIDE_CONFIG_DIRS_COMMON))\
  $(SET_FILES)",

 "RHIDE_PATH_SEPARATOR_$(RHIDE_OS)",
 ":",

 "RHIDE_PATH_SEPARATOR_DJGPP",
 ";",

 "RHIDE_PATH_SEPARATOR",
 "$(RHIDE_PATH_SEPARATOR_$(RHIDE_OS))",

 "RHIDE_EMPTY",
 "",

 "RHIDE_SPACE",
 "$(RHIDE_EMPTY) $(RHIDE_EMPTY)",

 0,
 0
};
/* *INDENT-ON* */

void
dump_rhide_environment(FILE * f)
{
  int i;

  for (i = 0; i < var_count; i++)
  {
    fprintf(f, "%s=%s\n", vars[i * 2], vars[i * 2 + 1]);
  }
}

#ifdef __DJGPP__
extern char **environ;
#endif


static int variables_inited = 0;
static

  char *_handle_rhide_token(const char *_token, token_func expand_tokens);
static char *ExpandSpec(const char *spec);

static void
init_variables(void)
{
  if (variables_inited)
    return;
  variables_inited = 1;
  int i = 0;

  while (default_variables[i])
  {
    char *variable, *contents, *_variable;

    _variable = default_variables[i];
    variable = ExpandSpec(_variable);
    contents = getenv(_variable);
    if (!contents)
      contents = default_variables[i + 1];
    insert_variable(_variable, contents);
    if (strcmp(variable, _variable) != 0)
    {
      contents = getenv(variable);
      if (!contents)
        contents = default_variables[i + 1];
      insert_variable(variable, contents);
    }
    string_free(variable);
    i += 2;
  }
  // Now check the env for any RHIDE_ variable
  for (i = 0; environ[i]; i++)
  {
    if (strncmp(environ[i], "RHIDE_", 6) == 0)
    {
      char *contents = strchr(environ[i], '=');

      if (!contents)
        continue;
      contents++;
      char var[256];

      memcpy(var, environ[i], (int) (contents - environ[i]) - 1);
      var[(int) (contents - environ[i]) - 1] = 0;
      insert_variable(var, contents);
    }
  }
  variables_inited = 1;
}

static char *
ExpandSpec(const char *spec)
{
  if (!variables_inited)
    init_variables();
  return expand_spec(spec, _handle_rhide_token);
}

char *
build_command(char *program, char *args)
{
  char *spec;
  char *command;

  string_dup(spec, "$(strip ");
  string_cat(spec, program);
  string_cat(spec, " ");
  string_cat(spec, args);
  string_cat(spec, ")");
  command = ExpandSpec(spec);
  string_free(spec);
  return command;
}

static TDependency *_token_dep;

typedef struct
{
  char *name;
  int name_len;
  char *(*func) ();
  char *(*make_func) ();
}
_rhide_tokens;

#define TF(x) static char *rhide_token_##x()
TF(INCLUDE_DIRS);
TF(LIB_DIRS);
TF(C_DEBUG_FLAGS);
TF(C_OPT_FLAGS);
TF(C_WARN_FLAGS);
TF(C_C_LANG_FLAGS);
TF(C_CXX_LANG_FLAGS);
TF(C_P_LANG_FLAGS);
TF(C_FPC_LANG_FLAGS);
TF(C_F_LANG_FLAGS);
TF(C_ADA_LANG_FLAGS);
TF(LIBS);
TF(LD_EXTRA_FLAGS);
TF(C_EXTRA_FLAGS);
TF(LOCAL_OPT);
TF(OBJFILES);
TF(ALL_OBJFILES);
TF(LIBRARIES);
TF(SOURCE_NAME);
TF(OUTFILE);
TF(make_SOURCE_NAME);
TF(make_OUTFILE);
TF(make_LOCAL_OPT);
TF(SPECIAL_CFLAGS);
TF(SPECIAL_LDFLAGS);
TF(PROG_ARGS);
TF(SRC_DIRS);
TF(WUC);
TF(make_WUC);
TF(EDITORS);
TF(make_EDITORS);
TF(RHIDE_OS);
TF(make_RHIDE_OS);
TF(MAIN_TARGET);
TF(PROJECT_ITEMS);
TF(DEFAULT_MASK);
TF(RHIDE_BIN_DIR);
TF(PASCAL_TYPE);
TF(GET_HOME);
TF(make_GET_HOME);
TF(CLEAN_FILES);

static _rhide_tokens rhide_tokens[] = {
#define SF(x,y) {#x,sizeof(#x)-1,rhide_token_##x,y}
  SF(RHIDE_OS, rhide_token_make_RHIDE_OS),
  SF(INCLUDE_DIRS, NULL),
  SF(LIB_DIRS, NULL),
  SF(C_DEBUG_FLAGS, NULL),
  SF(C_OPT_FLAGS, NULL),
  SF(C_WARN_FLAGS, NULL),
  SF(C_C_LANG_FLAGS, NULL),
  SF(C_CXX_LANG_FLAGS, NULL),
  SF(C_P_LANG_FLAGS, NULL),
  SF(C_FPC_LANG_FLAGS, NULL),
  SF(C_F_LANG_FLAGS, NULL),
  SF(C_ADA_LANG_FLAGS, NULL),
  SF(LIBS, NULL),
  SF(LD_EXTRA_FLAGS, NULL),
  SF(C_EXTRA_FLAGS, NULL),
  SF(LOCAL_OPT, rhide_token_make_LOCAL_OPT),
  SF(OBJFILES, NULL),
  SF(ALL_OBJFILES, NULL),
  SF(LIBRARIES, NULL),
  SF(SOURCE_NAME, rhide_token_make_SOURCE_NAME),
  SF(OUTFILE, rhide_token_make_OUTFILE),
  SF(SPECIAL_CFLAGS, NULL),
  SF(SPECIAL_LDFLAGS, NULL),
  SF(PROG_ARGS, NULL),
  SF(SRC_DIRS, NULL),
  SF(WUC, rhide_token_make_WUC),
  SF(EDITORS, rhide_token_make_EDITORS),
  SF(MAIN_TARGET, NULL),
  SF(PROJECT_ITEMS, NULL),
  SF(DEFAULT_MASK, NULL),
  SF(RHIDE_BIN_DIR, NULL),
  SF(PASCAL_TYPE, NULL),
  SF(GET_HOME, rhide_token_make_GET_HOME),
  SF(CLEAN_FILES, NULL),
  {NULL, 0, NULL, NULL}
#undef SF
};

TF(CLEAN_FILES)
{
  return string_dup("$(MAIN_TARGET) $(OBJFILES)");
}

TF(make_GET_HOME)
{
  return string_dup("$(HOME)");
}

TF(GET_HOME)
{
  char *tmp = ExpandSpec("$(HOME)");
  char *c = NULL;

  if (*tmp)
    c = tmp + strlen(tmp) - 1;
  if (c && ((*c == '/') || (*c == '\\')))
    *c = 0;
  return tmp;
}

TF(PASCAL_TYPE)
{
  if (UseFPC)
    return string_dup("FPC");
  else
    return string_dup("GPC");
}

TF(RHIDE_BIN_DIR)
{
  char *retval = string_dup(RHIDE_DIR);

  // RHIDE_DIR has ever the last slash
  if (*retval)
    retval[strlen(retval) - 1] = 0;
  return retval;
}

TF(DEFAULT_MASK)
{
  return string_dup(Project.defaultprojectmask);
}

TF(WUC)
{
  char *tmp;

  return (tmp = WUC())? tmp : string_dup("");
}

TF(make_WUC)
{
  return NULL;
}

TF(PROJECT_ITEMS)
{
  char *retval = NULL;
  int i, count = Project.dependencies->getCount();

  for (i = 0; i < count; i++)
  {
    if (retval)
      string_cat(retval, " ");
    string_cat(retval,
               FName(
                     ((TDependency *) Project.dependencies->
                      at(i))->source_name));
  }
  if (!retval)
    retval = string_dup("");
  return retval;
}

TF(MAIN_TARGET)
{
  return string_dup(FName(Project.dest_name));
}

TF(make_RHIDE_OS)
{
  return string_dup("$(RHIDE_OS_)\n\
ifeq ($(strip $(RHIDE_OS)),)\n\
ifneq ($(strip $(DJDIR)),)\n\
RHIDE_OS_:=DJGPP\n\
else\n\
RHIDE_OS_:=$(patsubst CYGWIN%,CYGWIN,$(shell uname))\n\
endif\n\
endif\n\
");
}

TF(RHIDE_OS)
{
  if (getenv("DJDIR"))
    return string_dup("DJGPP");
  struct utsname u;

  uname(&u);
  if (strncmp(u.sysname, "CYGWIN", 6) == 0)
    return string_dup("CYGWIN");
  else
    return string_dup(u.sysname);
}

TF(make_EDITORS)
{
  return NULL;
}

TF(EDITORS)
{
  char *retval = NULL;

  if (windows)
  {
    int i, count = windows->getCount();

    for (i = 0; i < count; i++)
    {
      TEvent event;
      DeskTopWindow *w = (DeskTopWindow *) windows->at(i);

      event.what = evBroadcast;
      event.message.command = cmEditorAnswer;
      w->window->handleEvent(event);
      if (event.what == evNothing)
      {
        if (retval)
          string_cat(retval, " ");
        string_cat(retval, w->full_name);
      }
    }
  }
  return retval;
}

TF(make_LOCAL_OPT)
{
  return string_dup("$(subst ___~~~___, ,$(subst $(notdir $<)___,,"
                    "$(filter $(notdir $<)___%,$(LOCAL_OPTIONS))))\n");
}

TF(make_SOURCE_NAME)
{
  return string_dup("$<");
}

TF(make_OUTFILE)
{
  return string_dup("$@");
}

static char *
_dirs(TDirList * list)
{
  char *retval;

  list->ToString(retval, " ");
  return retval;
}

TF(SRC_DIRS)
{
  return _dirs(Options.SrcDirs);
}

TF(INCLUDE_DIRS)
{
  return _dirs(Options.include_path);
}

TF(LIB_DIRS)
{
  return _dirs(Options.library_path);
}

static char *
_flags(TFlagCollection * flags)
{
  char *retval;

  flags->ToString(retval, " ");
  return retval;
}

TF(C_DEBUG_FLAGS)
{
  return _flags(Options.debug_flags);
}

TF(C_OPT_FLAGS)
{
  return _flags(Options.opt_flags);
}

TF(C_WARN_FLAGS)
{
  return _flags(Options.warn_flags);
}

TF(C_C_LANG_FLAGS)
{
  return _flags(Options.c_flags);
}

TF(C_CXX_LANG_FLAGS)
{
  return _flags(Options.cxx_flags);
}

TF(C_P_LANG_FLAGS)
{
  return _flags(Options.pascal_flags);
}

TF(C_FPC_LANG_FLAGS)
{
  return _flags(Options.fpc_flags);
}

TF(C_F_LANG_FLAGS)
{
  return _flags(Options.fortran_flags);
}

TF(C_ADA_LANG_FLAGS)
{
  return _flags(Options.ada_flags);
}

static char *
_params(TParamList * params)
{
  char *retval;

  params->ToString(retval);
  return retval;
}

TF(PROG_ARGS)
{
  return _params(Options.ProgArgs);
}

TF(LD_EXTRA_FLAGS)
{
  return _params(Options.link_opt);
}

TF(SPECIAL_LDFLAGS)
{
  char *retval = NULL;

  if (NoStdLib)
  {
    string_dup(retval, "-nostdlib");
  }
  if (ForProfile)
  {
    if (retval)
      string_cat(retval, " ");
    string_cat(retval, "-pg");
  }
  return retval;
}

TF(SPECIAL_CFLAGS)
{
  char *retval = NULL;

  if (NoStdInc)
  {
    if (retval)
      string_cat(retval, " ");
    string_cat(retval, "-nostdinc");
  }
  return retval;
}

TF(C_EXTRA_FLAGS)
{
  return _params(Options.comp_opt);
}

TF(LOCAL_OPT)
{
  return _params(_token_dep->local_options);
}

TF(SOURCE_NAME)
{
  char *retval;

  FindFile(FName(_token_dep->source_name), retval);
  AbsToRelPath(project_directory, retval);
  return retval;
}

TF(OUTFILE)
{
  char *retval;

  FindFile(FName(_token_dep->dest_name), retval);
  AbsToRelPath(project_directory, retval);
  return retval;
}

static char *
_libraries(int check_exclude)
{
  char *retval = NULL;
  char *tmp;
  int i, count = 0;

  if (_token_dep->dependencies)
    count = _token_dep->dependencies->getCount();
  for (i = 0; i < count; i++)
  {
    TDependency *dep = (TDependency *) _token_dep->dependencies->at(i);

    if ((dep->dest_file_type != FILE_LIBRARY) &&
        (dep->dest_file_type != FILE_DLL))
      continue;
    if ((check_exclude == 1) && dep->exclude_from_link)
      continue;
    if ((check_exclude == 2) && !dep->exclude_from_link)
      continue;
    if ((dep->dest_file_type == FILE_LIBRARY) ||
        (dep->dest_file_type == FILE_DLL))
    {
      FindFile(FName(dep->dest_name), tmp);
      AbsToRelPath(project_directory, tmp, NULL, 1);
      if (retval)
        string_cat(retval, " ");
      string_cat(retval, tmp);
      string_free(tmp);
    }
    else
    {
      string_dup(tmp, FName(dep->dest_name));
      AbsToRelPath(project_directory, tmp);
      if (retval)
        string_cat(retval, " ");
      string_cat(retval, tmp);
      string_free(tmp);
    }
  }
  return retval;
}

TF(LIBRARIES)
{
  return _libraries(1);
}

static char *recur_directory = NULL;

static void
recursive_object_files(TDependency * _dep, char *&retval, int check_exclude)
{
  AddToStack();
  if (_PushProject(_dep) == True)
  {
    int i, count = 0;
    char *tmp;

    if (project->dependencies)
      count = project->dependencies->getCount();
    for (i = 0; i < count; i++)
    {
      TDependency *dep = (TDependency *) project->dependencies->at(i);

      if ((check_exclude == 1) && dep->exclude_from_link)
        continue;
      if ((check_exclude == 2) && !dep->exclude_from_link)
        continue;
      if (dep->source_file_type == FILE_PROJECT &&
          dep->dest_file_type != FILE_LIBRARY)
      {
        recursive_object_files(dep, retval, check_exclude);
        continue;
      }
      if (dep->dest_file_type != FILE_OBJECT
          && dep->dest_file_type != FILE_UNKNOWN)
        continue;
      FindFile(FName(dep->dest_name), tmp);
      AbsToRelPath(recur_directory, tmp);
      if (retval)
        string_cat(retval, " ");
      string_cat(retval, tmp);
      string_free(tmp);
    }
    _PopProject();
  }
  RemoveFromStack();
}

static char *
_objfiles(int check_exclude, int use_all_obj)
{
  char *retval = NULL, *tmp;
  int i, count = 0;

  if (_token_dep->dependencies)
    count = _token_dep->dependencies->getCount();
  for (i = 0; i < count; i++)
  {
    TDependency *dep = (TDependency *) _token_dep->dependencies->at(i);

    if ((check_exclude == 1) && dep->exclude_from_link)
      continue;
    if ((check_exclude == 2) && !dep->exclude_from_link)
      continue;
    if (dep->source_file_type == FILE_PROJECT &&
        (use_all_obj ||
         (dep->dest_file_type != FILE_LIBRARY) &&
         (dep->dest_file_type != FILE_EXE) &&
         (dep->dest_file_type != FILE_COFF) &&
	 (dep->dest_file_type != FILE_DLL)))
    {
      string_dup(recur_directory, project_directory);
      recursive_object_files(dep, retval, check_exclude);
      string_free(recur_directory);
      continue;
    }
    if (dep->dest_file_type != FILE_OBJECT && dep->dest_file_type != FILE_UNKNOWN)	// unknown files also
      continue;
    FindFile(FName(dep->dest_name), tmp);
    AbsToRelPath(project_directory, tmp);
    if (retval)
      string_cat(retval, " ");
    string_cat(retval, tmp);
    string_free(tmp);
  }
  if (!retval)
    retval = string_dup("");
  return retval;
}

TF(OBJFILES)
{
  return _objfiles(1, 0);
}

TF(ALL_OBJFILES)
{
  return _objfiles(1, 1);
}

TF(LIBS)
{
  char *retval;

  AddLibraries(retval);
  return retval;
}

typedef struct
{
  char *name;
  int name_len;
  char *(*func) (char *);
}
_rhide_function;

#define FF(x) static char *rhide_function_##x(char *_arg)
FF(compilespec);
FF(outfile);
FF(prompt);
FF(objfiles);
FF(libraries);
FF(targets);

static _rhide_function rhide_functions[] = {
#define _FF(x) {#x,sizeof(#x)-1,rhide_function_##x}
  _FF(compilespec),
  _FF(outfile),
  _FF(prompt),
  _FF(objfiles),
  _FF(libraries),
  _FF(targets),
  {NULL, 0, NULL}
};

static token_func _expand_tokens;

FF(objfiles)
{
  int check_exclusive;

  if (sscanf(_arg, "%d", &check_exclusive) == 1)
    return _objfiles(check_exclusive, 0);
  else
    return string_dup("");
}

FF(libraries)
{
  int check_exclusive;

  if (sscanf(_arg, "%d", &check_exclusive) == 1)
    return _libraries(check_exclusive);
  else
    return string_dup("");
}

FF(targets)
{
  int check_exclusive;
  char *retval;

  if (sscanf(_arg, "%d", &check_exclusive) == 1)
  {
    retval = _objfiles(check_exclusive, 0);
    if (*retval)
      string_cat(retval, " ");
    string_cat(retval, _libraries(check_exclusive));
  }
  else
    retval = string_dup("");
  return retval;
}

FF(compilespec)
{
  char *arg = expand_rhide_spec(_arg);
  char *retval = NULL;
  ccIndex index;

  if (Project.dependencies->search(arg, index) == False)
  {
    retval = string_dup("");
  }
  else
  {
    Boolean is_user;
    TDependency *dep = (TDependency *) Project.dependencies->at(index);
    char *_spec = GetCompilerSpec(dep, is_user);

    retval = BuildCompiler(dep, _spec);
    string_free(_spec);
  }
  string_free(arg);
  return retval;
}

FF(outfile)
{
  char *arg = expand_rhide_spec(_arg);
  char *retval = NULL;
  ccIndex index;

  if (Project.dependencies->search(arg, index) == False)
  {
    retval = string_dup("");
  }
  else
  {
    retval = BuildCompiler((TDependency *) Project.dependencies->at(index),
                           "$(OUTFILE)");
  }
  string_free(arg);
  return retval;
}

FF(prompt)
{
  char *init_val;
  char *retval;

  init_val = find_next_comma(_arg);
  if (!init_val)
    retval = expand_rhide_spec(_arg);
  else
  {
    *init_val = 0;
    retval = expand_rhide_spec(_arg);
    *init_val = ',';
    init_val = expand_rhide_spec(init_val + 1);
  }
  TParamList *params = new TParamList();

  if (init_val)
  {
    params->FromString(init_val);
    string_free(init_val);
  }
  if (editParamList(params, _(retval), RHIDE_History_prompt) != cmOK)
  {
    destroy(params);
    string_free(retval);
    string_dup(retval, "");
    return retval;
  }
  string_free(retval);
  params->ToString(init_val);
  destroy(params);
  retval = expand_rhide_spec(init_val);
  string_free(init_val);
  return retval;
}

static char *
_handle_rhide_function(char *token, token_func expand_tokens)
{
  int i = 0;
  char *retval = NULL;
  token_func __expand_tokens = _expand_tokens;

  _expand_tokens = expand_tokens;
  while (rhide_functions[i].name)
  {
    int name_len = rhide_functions[i].name_len;

    if (strncmp(token, rhide_functions[i].name, name_len) == 0
        && token[name_len] == ' ')
    {
      char *tok;

      tok = rhide_functions[i].func(token + name_len + 1);
      if (!tok)
        tok = string_dup("");
      if (!expand_tokens)
      {
        retval = tok;
        break;
      }
      retval = expand_tokens(tok);
      string_free(tok);
      break;
    }
    i++;
  }
  _expand_tokens = __expand_tokens;
  return retval;
}

static char *
_handle_rhide_token(const char *_token, token_func expand_tokens)
{
  int name_len = strlen(_token) - 3;
  char *token = (char *) alloca(name_len + 1);

  strncpy(token, _token + 2, name_len);
  token[name_len] = 0;
  if (rh_islower(*token))
    return _handle_rhide_function(token, expand_tokens);
  int i = 0;

  while (rhide_tokens[i].name)
  {
    if (strcmp(token, rhide_tokens[i].name) == 0)
    {
      char *tok;

      tok = rhide_tokens[i].func();
      if (!tok)
        return string_dup("");
      if (!expand_tokens)
        return tok;
      char *retval = expand_tokens(tok);

      string_free(tok);
      return retval;
    }
    i++;
  }
  return NULL;
}

static void
WriteRule(FILE * f, char *from, char *to)
{
  TDependency *dep;

  dep = new TDependency();
  InitFName(dep->source_name, from);
  InitFName(dep->dest_name, to);
  dep->source_file_type = get_file_type(from);
  dep->dest_file_type = get_file_type(to);
  dep->compile_id = how_to_compile(dep->source_file_type,
                                   dep->dest_file_type);
  Boolean is_user;
  char *spec = GetCompilerSpec(dep, is_user);

  destroy(dep);
  if (!spec)
    return;
  fprintf(f, "%c%s: %c%s\n\t%s\n", '%', to, '%', from, spec);
  string_free(spec);
}

static void
WriteRules(FILE * f)
{
#define WR(from,to) WriteRule(f,"."#from,"."#to)
  WR(c, o);
  WR(i, o);
  WR(cc, o);
  WR(cpp, o);
  WR(cxx, o);
  WR(C, o);
  WR(ii, o);
  WR(s, o);
  WR(S, o);
  WR(c, s);
  WR(i, s);
  WR(cc, s);
  WR(cpp, s);
  WR(cxx, s);
  WR(C, s);
  WR(pas, o);
  WR(p, o);
  WR(pp, o);
  WR(pas, s);
  WR(p, s);
  WR(m, o);
  WR(m, s);
  WR(f, o);
  WR(for, o);
  WR(F, o);
  WR(fpp, o);
  WR(asm, o);
  WR(nsm, o);
  WR(adb, o);
  WR(c, i);
  WR(c, s);
  WR(cc, ii);
  WR(cc, s);
  WR(cpp, ii);
  WR(cpp, s);
  WR(cxx, ii);
  WR(cxx, s);
  WR(C, ii);
  WR(C, s);
#undef WR
}

void
WriteSpecData(FILE * f)
{
  int i;

  i = 0;
  put_breakline(f, 1, 75,
                "FLAGS_FOR_SUBPROJECTS=RHIDE_OS_=\"$(RHIDE_OS_)\" "
                "CFLAGS=\"$(CFLAGS)\" CXXFLAGS=\"$(CXXFLAGS)\" "
                "LDFLAGS=\"$(LDFLAGS)\" CPPFLAGS=\"$(CPPFLAGS)\"");
  _token_dep = project;
  while (rhide_tokens[i].name)
  {
    char *token;
    char *name;

    if (rhide_tokens[i].make_func)
      token = rhide_tokens[i].make_func();
    else
      token = rhide_tokens[i].func();
    string_dup(name, rhide_tokens[i].name);
    fprintf(f, "%s=", name);
    put_breakline(f, strlen(name) + 1, 75, token ? token : "");
    string_free(token);
    string_free(name);
    i++;
  }
  for (i = 0; i < var_count; i++)
  {
    fprintf(f, "%s=", vars[i * 2]);
    put_breakline(f, strlen(vars[i * 2]), 75, vars[i * 2 + 1]);
  }
  WriteRules(f);
}

char *
BuildCompiler(TDependency * dep, const char *spec)
{
  TDependency *_dep = _token_dep;

  _token_dep = dep;
  char *retval = ExpandSpec(spec);

  _token_dep = _dep;
  return retval;
}

char *
expand_rhide_spec(const char *spec)
{
  return BuildCompiler(project, spec);
}

/*
  search in the environment for a compile spec, for compiling
  files with specified suffixes taken from 'from' and 'to'.
  The name of the variable should

  RHIDE_COMPILE.extfrom.extto

  Example: define the variable
  RHIDE_COMPILE.cp.o
  to tell RHIDE, how to compile files with suffix '.cp' to files
  with suffix '.o'

  If the variable was found, the returned string is malloced, otherwise
  NULL is returned.
*/

static char *
GetUserCompileSpec(const char *from, const char *to)
{
  char *var;

  string_dup(var, "RHIDE_COMPILE");
  if (!from)
    string_cat(var, ".");
  else
  {
    char *dir, *name, *ext;

    split_fname(from, dir, name, ext);
    string_cat(var, ext);
    string_free(dir);
    string_free(name);
    string_free(ext);
  }
  if (!to)
    string_cat(var, ".");
  else
  {
    char *dir, *name, *ext;

    split_fname(to, dir, name, ext);
    string_cat(var, ext);
    string_free(dir);
    string_free(name);
    string_free(ext);
  }
  if (!GetVariable(var))
  {
    string_free(var);
    return NULL;
  }
  char *_var;

  string_dup(_var, "$(");
  string_cat(_var, var);
  string_cat(_var, ")");
  string_free(var);
  return _var;
}

char *
GetCompilerSpec(TDependency * dep, Boolean & is_user)
{
  is_user = False;
  char *user_spec = NULL;

  switch (dep->compiler_type)
  {
    case COMPILER_NASM:
      return string_dup("$(RHIDE_COMPILE_NASM)");
    case COMPILER_C:
      return string_dup("$(RHIDE_COMPILE_C_FORCE)");
    case COMPILER_CC:
      return strdup("$(RHIDE_COMPILE_CC_FORCE)");
    case COMPILER_ASM:
      return string_dup("$(RHIDE_COMPILE_ASM_FORCE)");
    case COMPILER_GPC:
      return string_dup("$(RHIDE_COMPILE_PASCAL_FORCE)");
    case COMPILER_FPC:
      return string_dup("$(RHIDE_COMPILE_FPC_FORCE)");
    case COMPILER_FORTRAN:
      return string_dup("$(RHIDE_COMPILE_FORTRAN_FORCE)");
    case COMPILER_NONE:
      return NULL;
    case COMPILER_AUTO:
      user_spec =
        GetUserCompileSpec(FName(dep->source_name), FName(dep->dest_name));
      if (user_spec)
      {
        if (dep->compile_id == COMPILE_UNKNOWN)
          is_user = True;
        return user_spec;
      }
    case COMPILER_USER:
      switch (dep->compile_id)
      {
#       define C(x) case x: return string_dup("$(RHIDE_"#x")");
          C(COMPILE_C);
          C(COMPILE_CC);
          C(COMPILE_PASCAL);
          C(COMPILE_FPC);
          C(COMPILE_ARCHIVE);
          C(COMPILE_ASM);
          C(COMPILE_OBJC);
        case COMPILE_LINK_PASCAL_AUTOMAKE:
          if (!UseFPC)
            return string_dup("$(RHIDE_COMPILE_LINK_PASCAL_AUTOMAKE)");
          else
            return string_dup("$(RHIDE_COMPILE_LINK_FPC_AUTOMAKE)");
          C(COMPILE_LINK_FPC_AUTOMAKE);
          C(COMPILE_FORTRAN);
          C(COMPILE_NASM);
#       undef C
        default:
          break;
      }
      if (dep->compile_id == COMPILE_LINK)
      {
        int i, count = 0;

        if (dep->dependencies)
          count = dep->dependencies->getCount();
        for (i = 0; i < count; i++)
        {
          FILE_TYPE sft =

            ((TDependency *) dep->dependencies->at(i))->source_file_type;
          if (sft == FILE_PASCAL_SOURCE)
          {
            if (!UseFPC)
              return string_dup("$(RHIDE_COMPILE_LINK_PASCAL)");
            else
              return string_dup("$(RHIDE_COMPILE_LINK_FPC)");
          }
          if (sft == FILE_FPC_SOURCE)
          {
            return string_dup("$(RHIDE_COMPILE_LINK_FPC)");
          }
          if (sft == FILE_ADA_SOURCE)
          {
            return string_dup("$(RHIDE_COMPILE_LINK_ADA)");
          }
        }
        return string_dup("$(RHIDE_COMPILE_LINK)");
      }
      if (dep->compile_id == COMPILE_LINK_DLL)
      {
        return string_dup("$(RHIDE_COMPILE_LINK_DLL)");
      }
      if (dep->compile_id == COMPILE_USER)
      {
        is_user = True;
        return string_dup(dep->compiler);
      }
      return GetUserCompileSpec(FName(dep->source_name),
                                FName(dep->dest_name));
      break;
  }
  return NULL;
}

void
#if (TCEDITOR_VERSION >= 0x000447UL)
InsertEnviromentVar(const char *variable, const char *contents)
#else
InsertEnviromentVar(char *variable, char *contents)
#endif
{
  insert_variable(variable, contents);
}

#ifdef TEST

TProject *project;
char *
WUC()
{
  return NULL;
}

void
AddLibraries(char *&)
{
}
char *project_directory = NULL;

int
main()
{
  int i = 0;

  while (default_variables[i])
  {
    fprintf(stdout, "%s=%s\n", default_variables[i],
            default_variables[i + 1]);
    i += 2;
  }
}

#endif
