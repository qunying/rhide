/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include "rhide.h"

#define Uses_TProject
#define Uses_TOptions
#define Uses_ideFunctions
#include <libide.h>

#define Uses_TDirList
#include <libtvuti.h>

#include <rhutils.h>

/*
  This functions tries to find the RCS filename for 'name'.
  In 'fname' it stores a newly allocated string with the
  name, which is != NULL when it was found (returning
  True) or = NULL when not found (returning False).

  !!!!!!!WARNING!!!!!!!!!
  Call this function only when you are sure that the file
  does not exist, otherwise RHIDE might be confused.

  This functions calls the external program 'rlog' for
  determining the filename, that means it is SLOW!!!!!!!!!

  rlog can be overwritten with the spec $(RHIDE_RLOG)
  which defaults to $(shell rlog -R $(rlog_arg)), where
  $(rlog_arg) is substituted with the actual filename.
  If you overwrite this variable, remember to include
  somewhere in this spec the variable $(rlog_arg).

  One example for overwriting it might be what I'm
  using for RHIDE (I use it on Linux and W95 with
  LFN enabled)

  rcs_suffix=,v
  rcs_prefix=RCS/
  rcs_basedir=$(dir $(rlog_arg))
  rcs_basename=$(notdir $(rlog_arg))
  rcs_name=$(rcs_basedir)$(rcs_prefix)$(rcs_basename)$(rcs_suffix)
  RHIDE_RLOG=$(wildcard $(rcs_name))

  If you'r running under plain DOS you might need the
  following (assuming you have int the directory where
  your sources are a file called RCS which contains
  the absolute filename to the subdirectory _RCS relative
  to the source directory and you have set RCSINIT=-x):

  rcs_prefix=_RCS/
  rcs_basedir=$(dir $(rlog_arg))
  rcs_basename=$(notdir $(rlog_arg))
  rcs_name=$(rcs_basedir)$(rcs_prefix)$(rcs_basename)
  RHIDE_RLOG=$(wildcard $(rcs_name))

  !!!WARNING!!!
  If you set $(RHIDE_RLOG) to something else be sure to
  know what you do!!
*/

Boolean
FindRCSFile(const char *name, char *&fname, char *&sname)
{
  insert_variable("rlog_arg", name);
  fname = expand_rhide_spec("$(RHIDE_RLOG)");
  if (*fname)
  {
    sname = string_dup(name);
    return True;
  }
  string_free(fname);
  int i, count = Options.SrcDirs->getCount();

  for (i = 0; i < count; i++)
  {
    sname = expand_rhide_spec((char *) Options.SrcDirs->at(i));
    string_cat(sname, "/", name, NULL);
    insert_variable("rlog_arg", sname);
    fname = expand_rhide_spec("$(RHIDE_RLOG)");
    if (*fname)
      return True;
    string_free(fname);
    string_free(sname);
  }
  return False;
}

void
CheckoutRCSFile(const char *sname, const char *fname, int rw)
{
  char *arg = NULL;

  string_cat(arg, rw ? "-l " : "", sname, " ", fname, NULL);
  insert_variable("co_arg", arg);
  string_free(arg);
  arg = expand_rhide_spec("$(RHIDE_CO)");
  string_free(arg);
}
